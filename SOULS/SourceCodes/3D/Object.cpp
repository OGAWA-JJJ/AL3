#include "Object.h"
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include "../2D/TexManager.h"

#pragma comment(lib, "d3dcompiler.lib")

Microsoft::WRL::ComPtr<ID3D12Device> Object::device = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Object::cmdList = nullptr;
ObjPipelineSet Object::pipelineSet;
std::shared_ptr<Light> Object::light = nullptr;

void Object::StaticInit(
	Microsoft::WRL::ComPtr<ID3D12Device> device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	//再初期化チェック
	assert(!Object::device);

	//nullptrチェック
	assert(device);

	Object::device = device;
	Object::cmdList = cmdList;

	//モデルの静的初期化
	Model::StaticInit(device);
}

ObjPipelineSet Object::CreateGraphicsPipeline(const ObjectInitData& objectInitData)
{
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;	//頂点シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;	//ピクセルシェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;	//エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ObjVS.hlsl",					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		objectInitData.m_vsEntryPoint.c_str(),
		"vs_5_0",
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
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ObjPS.hlsl",					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		objectInitData.m_psEntryPoint.c_str(),
		"ps_5_0",
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
		exit(1);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ //xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
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
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//標準設定
	//ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA全てのチャンネルを描画
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

	//図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;							//描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1;							//1ピクセルにつき1回サンプリング

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 レジスタ

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //t1 レジスタ

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[5] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	//通常と影用sampler
	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
	samplerDescs[0] = CD3DX12_STATIC_SAMPLER_DESC(
		0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);

	samplerDescs[1] = CD3DX12_STATIC_SAMPLER_DESC(
		1, D3D12_FILTER_MIN_MAG_MIP_POINT);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams),
		rootparams,
		_countof(samplerDescs),
		samplerDescs,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) {
		assert(0);
	}

	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	//グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	if (FAILED(result)) {
		assert(0);
	}

	return pipelineSet;
}

std::shared_ptr<Object> Object::Create(std::shared_ptr<Model> model)
{
	//3Dオブジェクトのインスタンスを生成
	std::shared_ptr<Object> object = std::make_shared<Object>();
	if (object == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object->Init()) {
		assert(0);
	}

	if (model) {
		object->SetModel(model);
	}

	return object;
}

Object::~Object()
{
}

bool Object::Init()
{
	//nullptrチェック
	assert(device);

	HRESULT result;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	return true;
}

void Object::Update()
{
	HRESULT result;
	DirectX::XMMATRIX matScale, matRot, matTrans;

	//スケール、回転、平行移動行列の計算
	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	this->matRot = matRot;

	//ワールド行列の合成
	matWorld = DirectX::XMMatrixIdentity();	//変形をリセット
	matWorld *= matScale;			//ワールド行列にスケーリングを反映
	matWorld *= matRot;				//ワールド行列に回転を反映
	matWorld *= matTrans;			//ワールド行列に平行移動を反映

	if (isAffine)
	{
		matWorld *= matrix;
	}

	if (isBillboard) {
		const DirectX::XMMATRIX& matBillboard = Camera::BillboardMatrix();

		matWorld = DirectX::XMMatrixIdentity();
		matWorld *= matScale; // ワールド行列にスケーリングを反映
		matWorld *= matRot; // ワールド行列に回転を反映
		matWorld *= matBillboard;
		matWorld *= matTrans; // ワールド行列に平行移動を反映
	}

	// 親オブジェクトがあれば
	if (parent != nullptr) {
		// 親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}

	DirectX::XMMATRIX matViewProjection = Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	DirectX::XMFLOAT3 cameraPos = Camera::GetEye();

	// 定数バッファへデータ転送
	ConstBufferDataB0* constMap0 = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap0);
	constMap0->viewproj = matViewProjection;
	constMap0->world = matWorld;
	constMap0->cameraPos = cameraPos;
	constMap0->color = color;
	constBuffB0->Unmap(0, nullptr);
}

void Object::Draw(const ObjPipelineSet& pipelineSet, bool isShadow)
{
	//nullptrチェック
	assert(device);
	assert(Object::cmdList);

	//モデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	//形式
	Object::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineSet.pipelinestate.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature.Get());
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	if (isShadow)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			TexManager::GetGpuHeapStartSRV(),
			TexManager::GetShadowInDsvIndex(),
			TexManager::GetIncrementSizeSRV());
		cmdList->SetGraphicsRootDescriptorTable(4, gpuHandle);
	}

	//ライトの描画
	light->Draw(cmdList, 3);

	//モデル描画
	model->Draw(cmdList);
}
