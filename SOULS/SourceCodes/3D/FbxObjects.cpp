#include "FbxObjects.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/ConstantBuffer.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

Microsoft::WRL::ComPtr<ID3D12Device> FbxObjects::device = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> FbxObjects::cmdList = nullptr;
FbxObjects::FbxPipelineSet FbxObjects::pipelineSet;
std::shared_ptr<Light> FbxObjects::light = nullptr;

void FbxObjects::StaticInit(
	Microsoft::WRL::ComPtr<ID3D12Device> dev,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
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
		fbxInitdata.m_vsEntryPoint.c_str(),
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
		fbxInitdata.m_psEntryPoint.c_str(),
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
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
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

	CD3DX12_DESCRIPTOR_RANGE descRange[2] = {};
	descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

	CD3DX12_ROOT_PARAMETER rootparams[6] = {};

	//どこで何使ってるか書け
	//ワールド行列
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//マテリアルのテクスチャ
	rootparams[1].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);
	//スキニング行列
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	//ライト情報
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	//マテリアル情報
	rootparams[4].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	//影
	rootparams[5].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(
		0, D3D12_FILTER_MIN_MAG_MIP_POINT);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;

	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));
	if (FAILED(result)) { assert(0); }

	return pipelineSet;
}

std::shared_ptr<FbxObjects> FbxObjects::Create(std::shared_ptr<FbxModels> model)
{
	std::shared_ptr<FbxObjects> object = std::make_shared<FbxObjects>();
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

void FbxObjects::Update()
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

	DirectX::XMMATRIX& matViewProjection =
		Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	DirectX::XMFLOAT3& cameraPos = Camera::GetEye();

	HRESULT result;

	DirectX::XMMATRIX& modelTransform = model->GetModelTransform();
	modelTransform = DirectX::XMMatrixIdentity();

	//他
	ConstBufferDataB0* constMap = nullptr;
	result = constBufferDataB0->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->viewproj = matViewProjection;
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBufferDataB0->Unmap(0, nullptr);
	}

	UpdateAnimation();
	UpdateTransform();
}

void FbxObjects::Draw(const FbxPipelineSet& pipelineSet, bool isShadow)
{
	assert(device);
	assert(FbxObjects::cmdList);

	if (model == nullptr) { return; }

	cmdList->SetPipelineState(pipelineSet.pipelinestate.Get());

	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cmdList->SetGraphicsRootConstantBufferView(
		0,
		constBufferDataB0->GetGPUVirtualAddress()
	);
	cmdList->SetGraphicsRootConstantBufferView(
		2,
		constBuffSkin->GetGPUVirtualAddress()
	);

	if (isShadow)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			TexManager::GetGpuHeapStartSRV(),
			TexManager::GetShadowInDsvIndex(),
			TexManager::GetIncrementSizeSRV());
		cmdList->SetGraphicsRootDescriptorTable(5, gpuHandle);
	}

	light->Draw(cmdList, 3);
	model->Draw(cmdList, drawSkip);
}

void FbxObjects::UpdateAnimation()
{
	animationDatas[m_currentAnimationIndex].m_isAnimationEndTrigger = false;
	if (!animationDatas[m_currentAnimationIndex].m_isPlay) { return; }

	const std::vector<FbxModels::Animation>& animations = model->GetAnimations();
	const FbxModels::Animation& animation = animations.at(m_currentAnimationIndex);

	if (!m_isBlend)
	{
		const std::vector<FbxModels::Keyframe>& keyframes = animation.keyframes;
		int keyCount = static_cast<int>(keyframes.size());
		for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
		{
			const FbxModels::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const FbxModels::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds >= keyframe0.seconds &&
				animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds < keyframe1.seconds)
			{
				float rate = (animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds -
					keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

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
	}

	if (!m_isAddTimerEase)
	{
		animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds +=
			animation.add_time;
	}
	else
	{
		animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds =
			animation.seconds_length * animationDatas[m_currentAnimationIndex].m_addSpeed;
	}

	if (animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds >= animation.seconds_length)
	{
		animationDatas[m_currentAnimationIndex].m_isAnimationEndTrigger = true;
		if (animationDatas[m_currentAnimationIndex].m_animationLoopFlag == true)
		{
			animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds -=
				animation.seconds_length;
		}
		else
		{
			animationDatas[m_currentAnimationIndex].m_isPlay = false;
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

void FbxObjects::BlendAnimation(int startIndex, int endIndex, float rate, bool isBlend)
{
	m_isBlend = isBlend;
	if (!m_isBlend)
	{
		return;
	}

	//補間前のアニメーション
	const std::vector<FbxModels::Animation>& animations =
		model->GetAnimations();
	const FbxModels::Animation& startAnimation =
		animations.at(startIndex);

	//補間後のアニメーション
	const FbxModels::Animation& endAnimation =
		animations.at(endIndex);

	//補間前のキーフレーム
	const std::vector<FbxModels::Keyframe>& startKeyframes = startAnimation.keyframes;

	//補間後のキーフレーム
	const std::vector<FbxModels::Keyframe>& endKeyframes = endAnimation.keyframes;

	//変数
	std::vector<DirectX::XMVECTOR> startScales;
	std::vector<DirectX::XMVECTOR> startRotates;
	std::vector<DirectX::XMVECTOR> startTranslates;
	bool isStartCalc = false;

	//補間前
	int startKeyCount = static_cast<int>(startKeyframes.size());
	for (int keyIndex = 0; keyIndex < startKeyCount - 1; ++keyIndex)
	{
		//補間前
		const FbxModels::Keyframe& startKeyframe0 = startKeyframes.at(keyIndex);
		const FbxModels::Keyframe& startKeyframe1 = startKeyframes.at(keyIndex + 1);

		//補間前計算
		if (animationDatas[startIndex].m_currentAnimationSeconds >= startKeyframe0.seconds &&
			animationDatas[startIndex].m_currentAnimationSeconds < startKeyframe1.seconds)
		{
			isStartCalc = true;
			float rate = (animationDatas[startIndex].m_currentAnimationSeconds - startKeyframe0.seconds) /
				(startKeyframe1.seconds - startKeyframe0.seconds);

			int nodeCount = static_cast<int>(nodes.size());

			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				const FbxModels::NodeKeyData& key0 = startKeyframe0.nodeKeys.at(nodeIndex);
				const FbxModels::NodeKeyData& key1 = startKeyframe1.nodeKeys.at(nodeIndex);

				DirectX::XMVECTOR Scale1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR Rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR Translate1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR Scale0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR Rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR Translate0 = DirectX::XMLoadFloat3(&key0.translate);

				startScales.push_back(DirectX::XMVectorLerp(Scale0, Scale1, rate));
				startRotates.push_back(DirectX::XMQuaternionSlerp(Rotate0, Rotate1, rate));
				startTranslates.push_back(DirectX::XMVectorLerp(Translate0, Translate1, rate));
			}
			break;
		}
	}
	if (!isStartCalc)
	{
		int nodeCount = static_cast<int>(nodes.size());
		for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			startScales.push_back(DirectX::XMLoadFloat3(&nodes[nodeIndex].scale));
			startRotates.push_back(DirectX::XMLoadFloat4(&nodes[nodeIndex].rotate));
			startTranslates.push_back(DirectX::XMLoadFloat3(&nodes[nodeIndex].translate));
		}
	}

	//変数
	std::vector<DirectX::XMVECTOR> endScales;
	std::vector<DirectX::XMVECTOR> endRotates;
	std::vector<DirectX::XMVECTOR> endTranslates;
	bool isEndCalc = false;

	//補間後
	int endKeyCount = static_cast<int>(endKeyframes.size());
	for (int keyIndex = 0; keyIndex < endKeyCount - 1; ++keyIndex)
	{
		//補間後
		const FbxModels::Keyframe& endKeyframe0 = endKeyframes.at(keyIndex);
		const FbxModels::Keyframe& endKeyframe1 = endKeyframes.at(keyIndex + 1);

		//補間後計算
		if (animationDatas[endIndex].m_currentAnimationSeconds >= endKeyframe0.seconds &&
			animationDatas[endIndex].m_currentAnimationSeconds < endKeyframe1.seconds)
		{
			isEndCalc = true;
			float rate = (animationDatas[endIndex].m_currentAnimationSeconds - endKeyframe0.seconds) /
				(endKeyframe1.seconds - endKeyframe0.seconds);

			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				const FbxModels::NodeKeyData& key0 = endKeyframe0.nodeKeys.at(nodeIndex);
				const FbxModels::NodeKeyData& key1 = endKeyframe1.nodeKeys.at(nodeIndex);

				DirectX::XMVECTOR Scale1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR Rotate1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR Translate1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR Scale0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR Rotate0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR Translate0 = DirectX::XMLoadFloat3(&key0.translate);

				endScales.push_back(DirectX::XMVectorLerp(Scale0, Scale1, rate));
				endRotates.push_back(DirectX::XMQuaternionSlerp(Rotate0, Rotate1, rate));
				endTranslates.push_back(DirectX::XMVectorLerp(Translate0, Translate1, rate));
			}
			break;
		}
	}
	if (!isEndCalc)
	{
		int nodeCount = static_cast<int>(nodes.size());
		for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
		{
			endScales.push_back(DirectX::XMLoadFloat3(&nodes[nodeIndex].scale));
			endRotates.push_back(DirectX::XMLoadFloat4(&nodes[nodeIndex].rotate));
			endTranslates.push_back(DirectX::XMLoadFloat3(&nodes[nodeIndex].translate));
		}
	}

	//補間
	int nodeCount = static_cast<int>(startScales.size());
	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		DirectX::XMVECTOR Scale =
			DirectX::XMVectorLerp(startScales[nodeIndex], endScales[nodeIndex], rate);
		DirectX::XMVECTOR Rotate =
			DirectX::XMQuaternionSlerp(startRotates[nodeIndex], endRotates[nodeIndex], rate);
		DirectX::XMVECTOR Translate =
			DirectX::XMVectorLerp(startTranslates[nodeIndex], endTranslates[nodeIndex], rate);

		Node& node = nodes[nodeIndex];
		DirectX::XMStoreFloat3(&node.scale, Scale);
		DirectX::XMStoreFloat4(&node.rotate, Rotate);
		DirectX::XMStoreFloat3(&node.translate, Translate);
	}
}

void FbxObjects::SetCurrentAnimationSpeed(float addSpeed, bool isSet)
{
	m_isAddTimerEase = isSet;
	if (!m_isAddTimerEase)
	{
		return;
	}
	animationDatas[m_currentAnimationIndex].m_addSpeed = addSpeed;
}

void FbxObjects::SetAnimationSpeed(int animationIndex, float addSpeed, bool isSet)
{
	m_isAddTimerEase = isSet;
	if (!m_isAddTimerEase)
	{
		return;
	}
	animationDatas[animationIndex].m_addSpeed = addSpeed;
}

DirectX::XMMATRIX& FbxObjects::GetMatWorld()
{
	DirectX::XMFLOAT3 l_scale = {};
	l_scale.x = 1 / scale.x;
	l_scale.y = 1 / scale.y;
	l_scale.z = 1 / scale.z;

	m_matTransRotWorld = matWorld;
	m_matTransRotWorld.r[0].m128_f32[0] *= l_scale.x;
	m_matTransRotWorld.r[0].m128_f32[1] *= l_scale.x;
	m_matTransRotWorld.r[0].m128_f32[2] *= l_scale.x;
	m_matTransRotWorld.r[1].m128_f32[0] *= l_scale.y;
	m_matTransRotWorld.r[1].m128_f32[1] *= l_scale.y;
	m_matTransRotWorld.r[1].m128_f32[2] *= l_scale.y;
	m_matTransRotWorld.r[2].m128_f32[0] *= l_scale.z;
	m_matTransRotWorld.r[2].m128_f32[1] *= l_scale.z;
	m_matTransRotWorld.r[2].m128_f32[2] *= l_scale.z;
	return m_matTransRotWorld;
}
