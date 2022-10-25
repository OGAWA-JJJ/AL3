#include "FbxObjects.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/ConstantBuffer.h"
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
	object->CalcNode();

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
	DirectX::XMMATRIX matRot, matScale, matTrans;

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

	DirectX::XMMATRIX lightMatViewProjection = Camera::ViewMatrix();
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

	UpdateAnimation();
	UpdateTransform();
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
	isPlay = true;
}

void FbxObjects::UpdateAnimation()
{
	if (!isPlay) { return; }

	const std::vector<FbxModels::Animation>& animations = model->GetAnimations();
	const FbxModels::Animation& animation = animations.at(current_animation_index);

	const std::vector<FbxModels::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		const FbxModels::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const FbxModels::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (current_animation_seconds >= keyframe0.seconds &&
			current_animation_seconds < keyframe1.seconds)
		{
			float rate = (current_animation_seconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				const FbxModels::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const FbxModels::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR Scale1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR Rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR Translate1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR Scale0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR Rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR Translate0 = DirectX::XMLoadFloat3(&key0.translate);

				DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(Scale0, Scale1, rate);
				DirectX::XMVECTOR Rotate = DirectX::XMQuaternionSlerp(Rotate0, Rotate1, rate);
				DirectX::XMVECTOR Translate = DirectX::XMVectorLerp(Translate0, Translate1, rate);

				DirectX::XMStoreFloat3(&node.scale, Scale);
				DirectX::XMStoreFloat4(&node.rotate, Rotate);
				DirectX::XMStoreFloat3(&node.translate, Translate);
			}
			break;
		}
	}

	current_animation_seconds += animation.add_time;
	m_isAnimationEndTrigger = false;

	if (current_animation_seconds >= animation.seconds_length)
	{
		m_isAnimationEndTrigger = true;
		if (animation_loop_flag == true)
		{
			current_animation_seconds = 0;

		}
		else
		{
			isPlay = false;
		}
	}
}

void FbxObjects::UpdateTransform()
{
	DirectX::XMMATRIX node_transform = DirectX::XMMatrixIdentity();
	for (Node& node : nodes)
	{
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(
			node.scale.x,
			node.scale.y,
			node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
			node.translate.x,
			node.translate.y,
			node.translate.z);
		DirectX::XMMATRIX local_transform = S * R * T;

		DirectX::XMMATRIX parent_transform;
		if (node.parent != nullptr)
		{
			parent_transform = node.parent->world_transform;
		}
		else
		{
			parent_transform = node_transform;
		}

		DirectX::XMMATRIX world_transform = local_transform * parent_transform;

		node.local_transform = local_transform;
		node.world_transform = world_transform;
	}

	std::vector<std::pair<std::string, DirectX::XMMATRIX>> fbxData;
	std::vector<DirectX::XMMATRIX> localMatRots;

	HRESULT result;
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < model->GetNodeIndices().size(); i++)
	{
		DirectX::XMMATRIX worldTransform =
			nodes.at(model->GetNodeIndices().at(i)).world_transform;
		DirectX::XMMATRIX offsetTransform =
			model->GetOffsetTransforms().at(i);
		DirectX::XMMATRIX boneTransform =
			offsetTransform * worldTransform;
		constMapSkin->bones[i] = boneTransform;

		DirectX::XMFLOAT3 l_scale = {};
		l_scale.x = 1 / scale.x;
		l_scale.y = 1 / scale.y;
		l_scale.z = 1 / scale.z;
		worldTransform.r[0].m128_f32[0] *= l_scale.x;
		worldTransform.r[0].m128_f32[1] *= l_scale.x;
		worldTransform.r[0].m128_f32[2] *= l_scale.x;
		worldTransform.r[1].m128_f32[0] *= l_scale.y;
		worldTransform.r[1].m128_f32[1] *= l_scale.y;
		worldTransform.r[1].m128_f32[2] *= l_scale.y;
		worldTransform.r[2].m128_f32[0] *= l_scale.z;
		worldTransform.r[2].m128_f32[1] *= l_scale.z;
		worldTransform.r[2].m128_f32[2] *= l_scale.z;

		fbxData.push_back(std::make_pair(
			nodes.at(model->GetNodeIndices().at(i)).name,
			worldTransform * matWorld));
		DirectX::XMFLOAT4 fbxMatRot = {};
		fbxMatRot = nodes.at(model->GetNodeIndices().at(i)).rotate;

		DirectX::XMMATRIX matRot = DirectX::XMMatrixIdentity();
		matRot *= DirectX::XMMatrixRotationZ(fbxMatRot.z);
		matRot *= DirectX::XMMatrixRotationX(fbxMatRot.x);
		matRot *= DirectX::XMMatrixRotationY(fbxMatRot.y);
		localMatRots.push_back(matRot);

		std::string l_name = nodes.at(model->GetNodeIndices().at(i)).name;
		if (l_name.find("RightHand", 0) != std::string::npos)
		{
			matrix = worldTransform * matWorld;
		}
	}
	constBuffSkin->Unmap(0, nullptr);

	affineTrans.clear();
	std::copy(fbxData.begin(), fbxData.end(), std::back_inserter(affineTrans));
	matRots.clear();
	std::copy(localMatRots.begin(), localMatRots.end(), std::back_inserter(matRots));
}
