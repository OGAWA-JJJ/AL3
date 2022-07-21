#include "FbxObjects.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"

ID3D12Device* FbxObjects::device = nullptr;
Light* FbxObjects::light = nullptr;

void FbxObjects::StaticInit(ID3D12Device* dev)
{
	assert(!FbxObjects::device);
	assert(device);

	FbxObjects::device = device;
	FbxModels::StaticInit(device);
}

FbxObjects* FbxObjects::Create(FbxModels* model)
{
	FbxObjects* object = new FbxObjects();
	//std::shared_ptr<Object> object = std::make_shared<Object>();
	if (object == nullptr) { return nullptr; }
	if (!object->Init()) { assert(0); }
	if (model) { object->SetModel(model); }

	return object;
}

bool FbxObjects::Init()
{
	HRESULT result;

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferDataB0)
	);

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin)
	);

	//1フレーム分の時間を60FPSで設定
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++) {
		constMapSkin->bones[i] = DirectX::XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	return true;
}

void FbxObjects::Update(bool isShadowCamera)
{
	DirectX::XMMATRIX matScale, matRot, matTrans;

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

	const DirectX::XMMATRIX& matViewProjection =
		Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	const DirectX::XMFLOAT3& cameraPos = Camera::GetEye();

	DirectX::XMMATRIX& lightMatViewProjection = Camera::ViewMatrix();
	if (isShadowCamera)
	{
		//影用
		DirectX::XMMATRIX matView = DirectX::XMMatrixLookAtLH(
			XMLoadFloat3(&light->GetShadowLigitEye()),
			XMLoadFloat3(&light->GetShadowLigitTarget()),
			XMLoadFloat3(&light->GetShadowLigitUp()));

		float fov = ImguiControl::Imgui_fov;
		DirectX::XMMATRIX lightMatPerspective = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(fov),
			(float)WINDOW_WIDTH / WINDOW_HEIGHT,
			0.1f, ImguiControl::Imgui_far_z); //前端、奥端

		lightMatViewProjection = matView * lightMatPerspective;
	}

	//モデルのメッシュトランスフォーム
	const DirectX::XMMATRIX& modelTransform = model->GetModelTransform();

	HRESULT result;

	//定数バッファへデータ転送
	ConstBufferDataB0* constMap = nullptr;
	result = constBufferDataB0->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		if (isShadowCamera) { constMap->viewproj = lightMatViewProjection; }
		else { constMap->viewproj = matViewProjection; }
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBufferDataB0->Unmap(0, nullptr);
	}

	//ボーン配列
	std::vector<FbxModels::Bone>& bones = model->GetBones();
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> fbxData;

	//回転行列配列
	std::vector<DirectX::XMMATRIX> localMatRots;

	//定数バッファへデータ転送(pmx->fbxにしたデータだとここがぶっ飛んだ値になる)←治った
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++) {

		//今の姿勢行列
		DirectX::XMMATRIX matCurrentPose;

		//今の姿勢行列を取得
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);

		//XMMATRIXに変換
		ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);

		//合成してスキニング行列に
		DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, model->GetModelTransform());
		DirectX::XMMATRIX trans = model->GetModelTransform();
		constMapSkin->bones[i] = trans * bones[i].invInitialPose * matCurrentPose * inverse;

		//FbxData取得用
		fbxData.push_back(std::make_pair(bones[i].name, trans * matCurrentPose * matWorld));
		FbxVector4 fbxMatRot =
			bones[i].fbxCluster->GetLink()->EvaluateLocalRotation(currentTime);

		//回転行列取得用
		DirectX::XMMATRIX matRot = DirectX::XMMatrixIdentity();
		matRot *= DirectX::XMMatrixRotationZ(static_cast<float>(fbxMatRot.mData[0]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationX(static_cast<float>(fbxMatRot.mData[1]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationY(static_cast<float>(fbxMatRot.mData[2]) / 180.0f * 3.14f);
		localMatRots.push_back(matRot);

		//手のワールド行列取得
		if (bones[i].name.find("RightHand", 0) != std::string::npos)
		{
			FbxAMatrix fbxMatrix = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
			ConvertMatrixFromFbx(&matrix, fbxMatrix);
			matrix = trans * matrix * matWorld;
		}
	}
	constBuffSkin->Unmap(0, nullptr);

	affineTrans.clear();
	std::copy(fbxData.begin(), fbxData.end(), std::back_inserter(affineTrans));
	matRots.clear();
	std::copy(localMatRots.begin(), localMatRots.end(), std::back_inserter(matRots));
}

void FbxObjects::Draw()
{
	assert(device);
	assert(FbxObjects::cmdList);

	if (model == nullptr) { return; }

	//パイプラインステートの設定
	//cmdList->SetPipelineState(pipelineSet.pipelinestate);
	//ルートシグネチャの設定
	//cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature);
	// 

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//定数バッファビューをセット←rootoaramIndexをちゃんと確認すべし
	cmdList->SetGraphicsRootConstantBufferView(
		0,
		constBufferDataB0->GetGPUVirtualAddress()
	);
	cmdList->SetGraphicsRootConstantBufferView(
		2,
		constBuffSkin->GetGPUVirtualAddress()
	);

	light->Draw(cmdList, 3);
	model->Draw(cmdList);
}

void FbxObjects::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();

	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);

	const char* animstackname = animstack->GetName();

	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	startTime = takeinfo->mLocalTimeSpan.GetStart();

	endTime = takeinfo->mLocalTimeSpan.GetStop();

	currentTime = startTime;

	isPlay = true;
}
