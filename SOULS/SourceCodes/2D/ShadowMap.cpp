#include "ShadowMap.h"
#include "../DirectX/ConstantBuffer.h"
#include "../DirectX/Camera.h"
#include "../2D/TexManager.h"

#include "../DirectX/d3dx12.h"
#include <d3dcompiler.h>
#include <cassert>

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> ShadowMap::cmdList = nullptr;

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::Init()
{
	//CreateGraphicsPipelineState();

	//CreateVBV();

	CreateConstBuff();

	incrementSizeSRV = DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	incrementSizeRTV = DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	incrementSizeDSV = DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//生成系
	//CreateTexBuff();

	CreateDSV();

	CreateSRV();

	//CreateRTV();
}

void ShadowMap::Draw()
{
	HRESULT result = S_OK;

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	ID3D12DescriptorHeap* ppHeaps[] = { TexManager::GetHeapAdressSRV() };
	//デスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());

	//SRV
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		TexManager::GetGpuHeapStartSRV(),
		srvIndex,
		incrementSizeSRV);
	cmdList->SetGraphicsRootDescriptorTable(
		1, gpuHandle);

	//描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void ShadowMap::PreDraw()
{
	//リソースバリアを変更
	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			TexManager::GetBuffer(inDsvIndex).Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_DEPTH_WRITE)
	);

	//レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		//TexManager::GetHeapAdressRTV()->GetCPUDescriptorHandleForHeapStart();
	//rtvH.ptr += rtvIndex * incrementSizeRTV;
	// 
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		TexManager::GetHeapAdressDSV()->GetCPUDescriptorHandleForHeapStart();
	dsvH.ptr += dsvIndex * incrementSizeDSV;
	//セット
	cmdList->OMSetRenderTargets(0, nullptr, false, &dsvH);

	//ビューポートの設定
	cmdList->RSSetViewports(
		1,
		&CD3DX12_VIEWPORT(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT)
	);
	//シザリング矩形の設定
	cmdList->RSSetScissorRects(
		1,
		&CD3DX12_RECT(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
	);

	//全画面クリア
	//float color[] = { 1.0f,1.0f,1.0f,1.0f };
	//cmdList->ClearRenderTargetView(rtvH, color, 0, nullptr);

	//深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void ShadowMap::PostDraw()
{
	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			TexManager::GetBuffer(inDsvIndex).Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_GENERIC_READ)
	);
}

void ShadowMap::CreateGraphicsPipelineState()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ShadowMapVS.hlsl",				//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		"VSmain",											//エントリーポイント名
		"vs_5_0",											//シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ShadowMapPS.hlsl",				//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		"PSmain",											//エントリーポイント名
		"ps_5_0",											//シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ //xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; //標準設定

	//ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA全てのチャンネルを描画

	blenddesc.BlendEnable = true;

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//共通設定
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;							//描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1;							//1ピクセルにつき1回サンプリング

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0レジスタ

	//ルートパラメータ(定数バッファに送るやつの設定、最初の引数でregisterを選択)
	//分けておくと必要な情報だけ転送したりできてエコ
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//rootparams[2].InitAsDescriptorTable(2, 0, D3D12_SHADER_VISIBILITY_ALL);

	//スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT); //s0レジスタ

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	//ルートシグネチャの生成
	result = DirectXImportant::dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = rootSignature.Get();

	//グラフィックスパイプラインの生成
	result = DirectXImportant::dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

void ShadowMap::CreateVBV()
{
	HRESULT result;

	//頂点バッファ生成
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * 4),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//頂点データ転送
	VertexPosUv vertices[4];

	//左上
	vertices[0].pos = { -1.0f, -1.0f , 0.0f };
	//左下
	vertices[1].pos = { -1.0f, -0.2f, 0.0f };
	//右上
	vertices[2].pos = { -0.2f, -1.0f , 0.0f };
	//右下
	vertices[3].pos = { -0.2f, -0.2f, 0.0f };

	vertices[0].uv = { 0.0f, 1.0f };
	vertices[1].uv = { 0.0f, 0.0f };
	vertices[2].uv = { 1.0f, 1.0f };
	vertices[3].uv = { 1.0f, 0.0f };

	//頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);
}

void ShadowMap::CreateConstBuff()
{
	//定数バッファの生成
	constBuff = ConstantBuffer::CreateConstantBuffer(sizeof(ConstantBuffer_b0));

	//定数バッファにデータ転送
	ConstantBuffer_b0 data;
	data.mat = DirectX::XMMatrixIdentity();

	ConstantBuffer::CopyToVRAM(constBuff, &data, sizeof(ConstantBuffer_b0));
}

void ShadowMap::CreateTexBuff()
{
	HRESULT result;

	//テクスチャリソース設定←mipLevelsってなに←荒さ改善的なの
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	//テクスチャバッファの生成
	float color[] = { 1.0f,1.0f,1.0f,1.0f };
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, color),
		IID_PPV_ARGS(&TexManager::GetBuffer())
	);
	assert(SUCCEEDED(result));

	//テクスチャを仮想生成
	const UINT pixelCount = WINDOW_WIDTH * WINDOW_HEIGHT;
	//画像1行分のデータサイズ
	const UINT rowPitch = sizeof(UINT) * WINDOW_WIDTH;
	//画像全体のデータサイズ
	const UINT depthPitch = rowPitch * WINDOW_HEIGHT;
	//画像イメージ
	UINT* img = new UINT[pixelCount];
	for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

	//テクスチャバッファにデータ転送
	result = TexManager::GetBuffer()->WriteToSubresource(0, nullptr,
		img, rowPitch, depthPitch);
	assert(SUCCEEDED(result));
	delete[] img;
}

void ShadowMap::CreateSRV()
{
	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		TexManager::GetCpuHeapStartSRV(),
		TexManager::GetOffsetSRV(),
		incrementSizeSRV);

	//デスクリプタヒープにSRVを作成
	DirectXImportant::dev->CreateShaderResourceView(
		TexManager::GetBuffer(inDsvIndex).Get(),
		&srvDesc,
		cpuHandle
	);

	srvIndex = TexManager::GetOffsetSRV();
	TexManager::SetIncrementSizeSRV(incrementSizeSRV);

	TexManager::AddOffsetSRV();
}

void ShadowMap::CreateRTV()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		TexManager::GetCpuHeapStartRTV(),
		TexManager::GetOffsetRTV(),
		incrementSizeRTV);

	DirectXImportant::dev->CreateRenderTargetView(
		TexManager::GetBuffer(srvIndex).Get(),
		nullptr,
		cpuHandle
	);

	rtvIndex = TexManager::GetOffsetRTV();

	TexManager::AddOffsetSRV();
	TexManager::AddOffsetRTV();
}

void ShadowMap::CreateDSV()
{
	HRESULT result;

	//深度バッファ用リソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

	//深度バッファ生成
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&TexManager::GetBuffer())
	);
	assert(SUCCEEDED(result));

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		TexManager::GetCpuHeapStartDSV(),
		TexManager::GetOffsetDSV(),
		incrementSizeDSV);

	//デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		TexManager::GetBuffer().Get(),
		&dsvDesc,
		cpuHandle
	);

	dsvIndex = TexManager::GetOffsetDSV();
	inDsvIndex = TexManager::GetOffsetSRV();
	TexManager::SetShadowInDsvIndex(inDsvIndex);

	TexManager::AddOffsetDSV();
}