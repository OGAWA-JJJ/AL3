#include "FbxObjects.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

ID3D12Device* FbxObjects::device = nullptr;
ID3D12GraphicsCommandList* FbxObjects::cmdList = nullptr;
FbxObjects::FbxPipelineSet FbxObjects::pipelineSet;
Light* FbxObjects::light = nullptr;

void FbxObjects::StaticInit(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList)
{
	assert(!FbxObjects::device);
	assert(dev);

	FbxObjects::device = dev;
	FbxObjects::cmdList = cmdList;
	FbxModels::StaticInit(device);
}

FbxObjects::FbxPipelineSet FbxObjects::CreateGraphicsPipeline(const FbxInitData& fbxInitdata)
{
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	assert(device);

	result = D3DCompileFromFile(
		L"Resources/Shaders/FBXVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		fbxInitdata.m_vsEntryPoint,
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
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	result = D3DCompileFromFile(
		L"Resources/Shaders/FBXPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		fbxInitdata.m_psEntryPoint,
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
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootparams[5] = {};

	//どこで何使ってるか書け

	//ワールド行列
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//マテリアルのテクスチャ
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//スキニング行列
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	//ライト情報
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	//マテリアル情報
	rootparams[4].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;

	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = pipelineSet.rootsignature;

	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));
	if (FAILED(result)) { assert(0); }

	return pipelineSet;
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

	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMapSkin->bones[i] = DirectX::XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);

	return true;
}

void FbxObjects::Update(bool isShadowCamera)
{
	DirectX::XMMATRIX matScale, matRot, matTrans;

	if (isPlay)
	{
		currentTime += frameTime;
		if (currentTime > endTime) { currentTime = startTime; }
	}

	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

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

	DirectX::XMMATRIX& modelTransform = model->GetModelTransform();
	modelTransform = DirectX::XMMatrixIdentity();

	HRESULT result;

	ConstBufferDataB0* constMap = nullptr;
	result = constBufferDataB0->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		if (isShadowCamera) { constMap->viewproj = lightMatViewProjection; }
		else { constMap->viewproj = matViewProjection; }
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBufferDataB0->Unmap(0, nullptr);
	}

	std::vector<FbxMeshes::Bone>& bones = model->GetBones();
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> fbxData;

	std::vector<DirectX::XMMATRIX> localMatRots;

	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);

	//一旦コメントアウト←FbxMeshesでUpdateすべき
	/*for (int i = 0; i < bones.size(); i++) {

		DirectX::XMMATRIX matCurrentPose;

		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);

		FbxModels::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);

		DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, model->GetModelTransform());
		DirectX::XMMATRIX trans = model->GetModelTransform();
		constMapSkin->bones[i] = trans * bones[i].invInitialPose * matCurrentPose * inverse;

		fbxData.push_back(std::make_pair(bones[i].name, trans * matCurrentPose * matWorld));
		FbxVector4 fbxMatRot =
			bones[i].fbxCluster->GetLink()->EvaluateLocalRotation(currentTime);

		DirectX::XMMATRIX matRot = DirectX::XMMatrixIdentity();
		matRot *= DirectX::XMMatrixRotationZ(static_cast<float>(fbxMatRot.mData[0]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationX(static_cast<float>(fbxMatRot.mData[1]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationY(static_cast<float>(fbxMatRot.mData[2]) / 180.0f * 3.14f);
		localMatRots.push_back(matRot);

		if (bones[i].name.find("RightHand", 0) != std::string::npos)
		{
			FbxAMatrix fbxMatrix = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
			FbxModels::ConvertMatrixFromFbx(&matrix, fbxMatrix);
			matrix = trans * matrix * matWorld;
		}
	}
	constBuffSkin->Unmap(0, nullptr);

	affineTrans.clear();
	std::copy(fbxData.begin(), fbxData.end(), std::back_inserter(affineTrans));
	matRots.clear();
	std::copy(localMatRots.begin(), localMatRots.end(), std::back_inserter(matRots));*/
}

void FbxObjects::Draw(const FbxPipelineSet& pipelineSet)
{
	assert(device);
	assert(FbxObjects::cmdList);

	if (model == nullptr) { return; }

	cmdList->SetPipelineState(pipelineSet.pipelinestate);

	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
