#include "FbxObject3D.h"
#include "../3D/FbxLoader.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

ID3D12Device* FbxObject3D::device = nullptr;
//ComPtr<ID3D12RootSignature> FbxObject3D::rootsignature;
//ComPtr<ID3D12PipelineState> FbxObject3D::pipelinestate;
FbxPipelineSet FbxObject3D::pipelineSet;
Light* FbxObject3D::light = nullptr;

FbxPipelineSet FbxObject3D::CreateGraphicsPipeline(const FbxInitData& fbxInitdata)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob;	//エラーオブジェクト

	assert(device);

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/FBXVS.hlsl",					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		fbxInitdata.m_vsEntryPoint,							//エントリーポイント名、
		"vs_5_0",											//シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
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
		L"Resources/Shaders/FBXPS.hlsl",					//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//インクルード可能にする
		fbxInitdata.m_psEntryPoint,							//エントリーポイント名
		"ps_5_0",											//シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
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
		{ //影響を受けるボーン番号(4つ)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //ボーンのスキンウェイト(4つ)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	//t0 レジスタ

	//ルートパラメータ
	//CD3DX12_ROOT_PARAMETER rootparams[2];
	CD3DX12_ROOT_PARAMETER rootparams[5] = {};

	//CBV（座標変換行列用）
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//SRV（テクスチャ）
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//CBV（スキニング用）
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	//ライト情報転送用
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	//反射光情報転送用
	rootparams[4].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	//スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = pipelineSet.rootsignature;

	//グラフィックスパイプラインの生成
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));
	if (FAILED(result)) { assert(0); }

	return pipelineSet;
}

void FbxObject3D::Init()
{
	HRESULT result;

	//定数バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);

	//定数バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin)
	);

	//定数バッファ生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData1) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffData1)
	);

	//1フレーム分の時間を60FPSで設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	//定数バッファへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++) {
		constMapSkin->bones[i] = DirectX::XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);
}

void FbxObject3D::Update(bool isShadowCamera)
{
	XMMATRIX matScale, matRot, matTrans;

	//アニメーション
	if (isPlay) {
		//1フレーム進める
		currentTime += frameTime;

		//最後まで再生したら先頭に戻す
		if (currentTime > endTime) { currentTime = startTime; }
	}

	//スケール、回転、平行移動行列の計算
	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//ワールド行列の合成
	matWorld = DirectX::XMMatrixIdentity(); //変形をリセット
	matWorld *= matScale;					//ワールド行列にスケーリングを反映
	matWorld *= matRot;						//ワールド行列に回転を反映
	matWorld *= matTrans;					//ワールド行列に平行移動を反映

	const XMMATRIX& matViewProjection =
		Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	const XMFLOAT3& cameraPos = Camera::GetEye();

	XMMATRIX& lightMatViewProjection = Camera::ViewMatrix();
	if (isShadowCamera)
	{
		//影用
		XMMATRIX matView = DirectX::XMMatrixLookAtLH(
			XMLoadFloat3(&light->GetShadowLigitEye()),
			XMLoadFloat3(&light->GetShadowLigitTarget()),
			XMLoadFloat3(&light->GetShadowLigitUp()));

		float fov = ImguiControl::Imgui_fov;
		XMMATRIX lightMatPerspective = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(fov),
			(float)WINDOW_WIDTH / WINDOW_HEIGHT,
			0.1f, ImguiControl::Imgui_far_z); //前端、奥端

		lightMatViewProjection = matView * lightMatPerspective;
	}

	//モデルのメッシュトランスフォーム
	const XMMATRIX& modelTransform = model->GetModelTransform();

	HRESULT result;

	//定数バッファへデータ転送
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		if (isShadowCamera) { constMap->viewproj = lightMatViewProjection; }
		else { constMap->viewproj = matViewProjection; }
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBuffTransform->Unmap(0, nullptr);
	}

	//定数バッファへデータ転送
	ConstBufferData1* constMap1 = nullptr;
	result = constBuffData1->Map(0, nullptr, (void**)&constMap1);
	if (SUCCEEDED(result)) {
		constMap1->ambient = ambient;
		constMap1->diffuse = diffuse;
		constMap1->specular = specular;
		constMap1->alpha = alpha;
		constBuffData1->Unmap(0, nullptr);
	}

	//ボーン配列
	std::vector<FbxModel::Bone>& bones = model->GetBones();
	std::vector<std::pair<std::string, XMMATRIX>> fbxData;

	//回転行列配列
	std::vector<DirectX::XMMATRIX> localMatRots;

	//定数バッファへデータ転送(pmx->fbxにしたデータだとここがぶっ飛んだ値になる)←治った
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++) {

		//今の姿勢行列
		XMMATRIX matCurrentPose;

		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);

		//XMMATRIXに変換
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);

		//合成してスキニング行列に
		XMMATRIX inverse = XMMatrixInverse(nullptr, model->GetModelTransform());
		XMMATRIX trans = model->GetModelTransform();
		constMapSkin->bones[i] = trans * bones[i].invInitialPose * matCurrentPose * inverse;

		//FbxData取得用
		fbxData.push_back(std::make_pair(bones[i].name, trans * matCurrentPose * matWorld));
		FbxVector4 fbxMatRot =
			bones[i].fbxCluster->GetLink()->EvaluateLocalRotation(currentTime);

		//回転行列取得用
		DirectX::XMMATRIX matRot = DirectX::XMMatrixIdentity();
		//matRot.r[0].m128_f32[0] = fbxMatRot.mData[0] / 180.0f * 3.14f;
		//matRot.r[1].m128_f32[1] = fbxMatRot.mData[1] / 180.0f * 3.14f;
		//matRot.r[2].m128_f32[2] = fbxMatRot.mData[2] / 180.0f * 3.14f;
		matRot *= DirectX::XMMatrixRotationZ(static_cast<float>(fbxMatRot.mData[0]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationX(static_cast<float>(fbxMatRot.mData[1]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationY(static_cast<float>(fbxMatRot.mData[2]) / 180.0f * 3.14f);
		localMatRots.push_back(matRot);

		//手のワールド行列取得
		if (bones[i].name.find("RightHand", 0) != std::string::npos)
		{
			FbxAMatrix fbxMatrix = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
			FbxLoader::ConvertMatrixFromFbx(&matrix, fbxMatrix);
			matrix = trans * matrix * matWorld;
		}
	}
	constBuffSkin->Unmap(0, nullptr);

	affineTrans.clear();
	std::copy(fbxData.begin(), fbxData.end(), std::back_inserter(affineTrans));
	matRots.clear();
	std::copy(localMatRots.begin(), localMatRots.end(), std::back_inserter(matRots));
}

void FbxObject3D::Draw(ID3D12GraphicsCommandList* cmdList, const FbxPipelineSet& pipelineSet)
{
	//モデルの割り当てがなければ描画しない
	if (model == nullptr) { return; }

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineSet.pipelinestate);
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature);
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(
		0,
		constBuffTransform->GetGPUVirtualAddress()
	);
	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(
		4,
		constBuffData1->GetGPUVirtualAddress()
	);
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(
		2,
		constBuffSkin->GetGPUVirtualAddress()
	);


	//ライトの描画
	light->Draw(cmdList, 3);
	//描画
	model->Draw(cmdList);
}

void FbxObject3D::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();

	//0番のアニメーション取得
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);

	//アニメーションの名前取得
	const char* animstackname = animstack->GetName();

	//アニメーションの時間情報
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//開始時間取得
	startTime = takeinfo->mLocalTimeSpan.GetStart();

	//終了時間取得
	endTime = takeinfo->mLocalTimeSpan.GetStop();

	//開始時間に合わせる
	currentTime = startTime;

	//再生中にする
	isPlay = true;
}

void FbxObject3D::AddTexture(ID3D12Resource* texbuff, ID3D12DescriptorHeap* srv)
{
	//1枚のみ対応
	//HRESULT result;
	fbxDescHeap = srv;
	if (srv == nullptr) { assert(0); }

	//D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	//srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//srvDescHeapDesc.NumDescriptors = 1;

	//SRV用デスクリプタヒープを作成
	//result = DirectXImportant::dev->CreateDescriptorHeap(
		//&srvDescHeapDesc,
		//IID_PPV_ARGS(&modelDescHeap));

	//assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; //設定構造体
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	/*device->CreateShaderResourceView(
		texbuff,
		&srvDesc,
		modelDescHeap->GetCPUDescriptorHandleForHeapStart());*/

	device->CreateShaderResourceView(
		texbuff,
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			fbxDescHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));
}

void FbxObject3D::ShadowDraw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* srv)
{
	UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	cmdList->SetGraphicsRootDescriptorTable(
		4,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			srv->GetGPUDescriptorHandleForHeapStart(),
			0,
			size));
}
