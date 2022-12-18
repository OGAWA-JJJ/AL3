#include "TrailManager.h"
#include "../DirectX/Camera.h"
#include "../DirectX/DirectXImportant.h"
#include "../DirectX/ConstantBuffer.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

//TrailRenderer
TrailRenderer::TrailRenderer()
{
}

void TrailRenderer::Init()
{
	HRESULT result = S_FALSE;

#pragma region Vertex

	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexData) * 4),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertBuff));
	if (FAILED(result)) { assert(0); }

	CreateTrail();

	m_vbView.BufferLocation = m_vertBuff->GetGPUVirtualAddress();
	m_vbView.SizeInBytes = sizeof(VertexData) * 4;
	m_vbView.StrideInBytes = sizeof(VertexData);

#pragma endregion

#pragma region Constbuff

	m_constBuff = ConstantBuffer::CreateConstantBuffer(sizeof(ConstBufferData));

#pragma endregion
}

void TrailRenderer::Update()
{
	if (m_isCreate)
	{
#pragma region ConstBuff
		ConstBufferData data;
		data.viewProj = Camera::ViewMatrix() * Camera::PerspectiveMatrix();
		data.color = m_color;
		ConstantBuffer::CopyToVRAM(m_constBuff, &data, sizeof(ConstBufferData));
#pragma endregion

		CheckLife();
	}
}

void TrailRenderer::Draw(
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSigunature,
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState)
{
	if (m_isCreate)
	{
		//Pipeline
		DirectXImportant::cmdList->SetPipelineState(pipelineState.Get());
		//RootSigunature
		DirectXImportant::cmdList->SetGraphicsRootSignature(rootSigunature.Get());
		//PrimitiveTopology
		DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//VB
		DirectXImportant::cmdList->IASetVertexBuffers(0, 1, &m_vbView);

		//CBV
		DirectXImportant::cmdList->
			SetGraphicsRootConstantBufferView(0, m_constBuff->GetGPUVirtualAddress());
		DirectXImportant::cmdList->DrawInstanced(4, 1, 0, 0);
	}
}

void TrailRenderer::CheckLife()
{
	if (m_life == C_CHECK_LIFE)
	{
		m_subAlpha = m_trailRendererData.color.w / C_CHECK_LIFE;
		m_color.w -= m_subAlpha;
	}
	else if (m_life < C_CHECK_LIFE)
	{
		m_color.w -= m_subAlpha;
		if (m_color.w < 0) { m_color.w = 0.0f; }
	}

	if (m_life > 0)
	{
		m_life--;
		if (m_life == 0)
		{
			m_isCreate = false;
		}
	}
}

void TrailRenderer::CreateBuff()
{
	HRESULT result = S_FALSE;

	enum { LB, LT, RB, RT };

	//頂点データ
	VertexData vertices[4];

	vertices[LB].pos = m_trailRendererData.oldPos_bottom;		//左下
	vertices[LT].pos = m_trailRendererData.oldPos_top;			//左上
	vertices[RB].pos = m_trailRendererData.currentPos_bottom;	//右下
	vertices[RT].pos = m_trailRendererData.currentPos_top;		//右上

	//頂点バッファへのデータ転送
	VertexData* vertMap = nullptr;
	result = m_vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result))
	{
		memcpy(vertMap, vertices, sizeof(vertices));
		m_vertBuff->Unmap(0, nullptr);
	}
}

void TrailRenderer::CreateTrail(int intervalFrame)
{
	m_isCreate = true;
	m_life = m_trailRendererData.life;
	m_subAlpha = 0.0f;
	m_color = m_trailRendererData.color;

	CreateBuff();
}

//TrailManager
TrailManager::TrailManager()
{
	CreateGraphicsPipeline();

	m_trailRenderers.resize(C_INIT_MAX_TRAIL);
}

void TrailManager::Init()
{
	for (int i = 0; i < m_maxTrail; i++)
	{
		m_trailRenderers[i].Init();
	}
}

void TrailManager::Update()
{
	for (int i = 0; i < m_maxTrail; i++)
	{
		m_trailRenderers[i].Update();
	}
}

void TrailManager::Draw()
{
	for (int i = 0; i < m_maxTrail; i++)
	{
		m_trailRenderers[i].Draw(m_rootsignature, m_pipelinestate);
	}
}

void TrailManager::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

#pragma region ShaderCompile

	result = D3DCompileFromFile(
		L"Resources/Shaders/TrailVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSmain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result))
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";

		//表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	result = D3DCompileFromFile(
		L"Resources/Shaders/TrailPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSmain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (FAILED(result))
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";

		//表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

#pragma endregion

#pragma region InputElement

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

#pragma endregion

#pragma region Pipeline

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[1] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

	//スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams),
		rootparams,
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//バージョン自動判定のシリアライズ
	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);

	//ルートシグネチャの生成
	result = DirectXImportant::dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_rootsignature));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = m_rootsignature.Get();

	//グラフィックスパイプラインの生成
	result = DirectXImportant::dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&m_pipelinestate));

	if (FAILED(result)) { assert(0); }

#pragma endregion
}

void TrailManager::SetMaxTrail(const int num)
{
	if (num > C_INIT_MAX_TRAIL) { assert(0); }
	m_maxTrail = num;
}
