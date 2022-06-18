#include "Object.h"
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../DirectX/Camera.h"
#include "../Math/BaseCollider.h"
#include "../Math/CollisionManager.h"
#include "../../imgui/ImguiControl.h"

#pragma comment(lib, "d3dcompiler.lib")

ID3D12Device* Object::device = nullptr;
ID3D12GraphicsCommandList* Object::cmdList = nullptr;
PipelineSet Object::pipelineSet;
Light* Object::light = nullptr;

void Object::StaticInit(ID3D12Device* device)
{
	//再初期化チェック
	assert(!Object::device);

	//nullptrチェック
	assert(device);

	Object::device = device;

	//グラフィックパイプラインの生成
	//CreateGraphicsPipeline(objectInitData);

	//モデルの静的初期化
	Model::StaticInit(device);
}

PipelineSet Object::CreateGraphicsPipeline(const ObjectInitData& objectInitData)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; //頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;    //ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; //エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ObjVS.hlsl",    //シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
		objectInitData.m_vsEntryPoint,
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用設定
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
		L"Resources/Shaders/ObjPS.hlsl",    //シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //インクルード可能にする
		objectInitData.m_psEntryPoint,
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //デバッグ用設定
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
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	//ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA全てのチャンネルを描画
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

	gpipeline.NumRenderTargets = 1;    //描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; //0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; //1ピクセルにつき1回サンプリング

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 レジスタ

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //t1 レジスタ

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[5];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	//スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) {
		assert(0);
	}

	gpipeline.pRootSignature = pipelineSet.rootsignature;

	//グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	if (FAILED(result)) {
		assert(0);
	}

	return pipelineSet;
}

void Object::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	//PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Object::cmdList == nullptr);

	//コマンドリストをセット
	Object::cmdList = cmdList;

	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object::PostDraw()
{
	//コマンドリストを解除
	Object::cmdList = nullptr;
}

Object* Object::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Object* object = new Object();
	if (object == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object->Init()) {
		delete object;
		assert(0);
	}

	if (model) {
		object->SetModel(model);
	}

	return object;
}

Object::~Object()
{
	if (collider) {
		CollisionManager::GetInstance()->RemoveCollider(collider);
		delete collider;
	}
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

	//クラス名
	name = typeid(*this).name();

	return true;
}

void Object::Update(bool isShadowCamera)
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ワールド行列の合成
	matWorld = XMMatrixIdentity(); // 変形をリセット
	matWorld *= matScale; // ワールド行列にスケーリングを反映
	matWorld *= matRot; // ワールド行列に回転を反映
	matWorld *= matTrans; // ワールド行列に平行移動を反映

	if (isBillboard) {
		const XMMATRIX& matBillboard = Camera::BillboardMatrix();

		matWorld = XMMatrixIdentity();
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

	const XMMATRIX& matViewProjection = Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	const XMFLOAT3& cameraPos = Camera::GetEye();

	XMMATRIX& lightMatViewProjection = Camera::ViewMatrix();
	if (isShadowCamera)
	{
		//影用
		XMFLOAT3 eye = { 0,150,0 };
		XMFLOAT3 target = { 0,0,0 };
		XMFLOAT3 up = { 0,0,-1 };

		XMMATRIX matView = XMMatrixLookAtLH(
			XMLoadFloat3(&eye),
			XMLoadFloat3(&target),
			XMLoadFloat3(&up));

		float fov = ImguiControl::Imgui_fov;
		XMMATRIX lightMatPerspective = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(fov),
			(float)WINDOW_WIDTH / WINDOW_HEIGHT,
			0.1f, ImguiControl::Imgui_far_z); //前端、奥端

		lightMatViewProjection = matView * lightMatPerspective;
	}

	// 定数バッファへデータ転送
	ConstBufferDataB0* constMap = nullptr;
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);
	if (isShadowCamera) { constMap->viewproj = lightMatViewProjection; }
	else { constMap->viewproj = matViewProjection; }
	constMap->world = matWorld;
	constMap->cameraPos = cameraPos;
	constBuffB0->Unmap(0, nullptr);

	//当たり判定更新
	if (collider) {
		collider->Update();
	}
}

void Object::Draw(PipelineSet pipelineSet)
{
	//nullptrチェック
	assert(device);
	assert(Object::cmdList);

	//モデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineSet.pipelinestate);
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature);
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//ライトの描画
	light->Draw(cmdList, 3);

	//モデル描画
	model->Draw(cmdList, modelDescHeap, 4, isAddTexture);
}

void Object::SetCollider(BaseCollider* collider)
{
	collider->SetObject(this);
	this->collider = collider;
	//Managerに登録
	CollisionManager::GetInstance()->AddCollider(collider);
	//更新
	collider->Update();
}

void Object::AddTexture(ID3D12Resource* texbuff)
{
	isAddTexture = true;
	//1枚のみ対応
	HRESULT result;
	modelDescHeap = Model::GetDescHeap();

	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;

	//SRV用デスクリプタヒープを作成
	result = DirectXImportant::dev->CreateDescriptorHeap(
		&srvDescHeapDesc,
		IID_PPV_ARGS(&modelDescHeap));

	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; //設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	/*device->CreateShaderResourceView(
		texbuff,
		&srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart());*/

	device->CreateShaderResourceView(
		texbuff,
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			modelDescHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));
}
