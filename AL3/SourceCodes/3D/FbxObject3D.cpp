#include "FbxObject3D.h"
#include "../3D/FbxLoader.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
//#include "../Math/OgaJHelper.h"
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

//using namespace Microsoft::WRL;
//using namespace DirectX;

ID3D12Device* FbxObject3D::device = nullptr;
//ComPtr<ID3D12RootSignature> FbxObject3D::rootsignature;
//ComPtr<ID3D12PipelineState> FbxObject3D::pipelinestate;
FbxPipelineSet FbxObject3D::pipelineSet;
Light* FbxObject3D::light = nullptr;

FbxPipelineSet FbxObject3D::CreateGraphicsPipeline(const FbxInitData& fbxInitdata)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;	//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;	//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob;	//�G���[�I�u�W�F�N�g

	assert(device);

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/FBXVS.hlsl",					//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�\�ɂ���
		fbxInitdata.m_vsEntryPoint,							//�G���g���[�|�C���g���A
		"vs_5_0",											//�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/FBXPS.hlsl",					//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�\�ɂ���
		fbxInitdata.m_psEntryPoint,							//�G���g���[�|�C���g��
		"ps_5_0",											//�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ //xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //�@���x�N�g��(1�s�ŏ������ق������₷��)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //�e�����󂯂�{�[���ԍ�(4��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ //�{�[���̃X�L���E�F�C�g(4��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	//�T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//�W���ݒ�
	//���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//�f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//�u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//�[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;							//�`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	//0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1;							//1�s�N�Z���ɂ�1��T���v�����O

	//�f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);	//t0 ���W�X�^

	//���[�g�p�����[�^
	//CD3DX12_ROOT_PARAMETER rootparams[2];
	CD3DX12_ROOT_PARAMETER rootparams[5] = {};

	//CBV�i���W�ϊ��s��p�j
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//SRV�i�e�N�X�`���j
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	//CBV�i�X�L�j���O�p�j
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	//���C�g���]���p
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	//���ˌ����]���p
	rootparams[4].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	//�X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	//�o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = pipelineSet.rootsignature;

	//�O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));
	if (FAILED(result)) { assert(0); }

	return pipelineSet;
}

void FbxObject3D::Init()
{
	HRESULT result;

	//�萔�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);

	//�萔�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin)
	);

	//�萔�o�b�t�@����
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData1) + 0xff) & ~0xFF),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffData1)
	);

	//1�t���[�����̎��Ԃ�60FPS�Őݒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++) {
		constMapSkin->bones[i] = DirectX::XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);
}

void FbxObject3D::Update(bool isShadowCamera)
{
	XMMATRIX matScale, matTrans;

	//�A�j���[�V����
	if (isPlay) {
		//1�t���[���i�߂�
		currentTime += frameTime;

		//�Ō�܂ōĐ�������擪�ɖ߂�
		if (currentTime > endTime) { currentTime = startTime; }
	}

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	m_matRot = DirectX::XMMatrixIdentity();
	m_matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));
	m_matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));
	m_matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//���[���h�s��̍���
	matWorld = DirectX::XMMatrixIdentity(); //�ό`�����Z�b�g
	matWorld *= matScale;					//���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= m_matRot;					//���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans;					//���[���h�s��ɕ��s�ړ��𔽉f

	const XMMATRIX& matViewProjection =
		Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	const XMFLOAT3& cameraPos = Camera::GetEye();

	XMMATRIX lightMatViewProjection = Camera::ViewMatrix();
	if (isShadowCamera)
	{
		//�e�p
		XMMATRIX matView = DirectX::XMMatrixLookAtLH(
			XMLoadFloat3(&light->GetShadowLigitEye()),
			XMLoadFloat3(&light->GetShadowLigitTarget()),
			XMLoadFloat3(&light->GetShadowLigitUp()));

		float fov = ImguiControl::Imgui_fov;
		XMMATRIX lightMatPerspective = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(fov),
			(float)WINDOW_WIDTH / WINDOW_HEIGHT,
			0.1f, ImguiControl::Imgui_far_z); //�O�[�A���[

		lightMatViewProjection = matView * lightMatPerspective;
	}

	//���f���̃��b�V���g�����X�t�H�[��
	const XMMATRIX& modelTransform = model->GetModelTransform();

	HRESULT result;

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataTransform* constMap = nullptr;
	result = constBuffTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		if (isShadowCamera) { constMap->viewproj = lightMatViewProjection; }
		else { constMap->viewproj = matViewProjection; }
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBuffTransform->Unmap(0, nullptr);
	}

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData1* constMap1 = nullptr;
	result = constBuffData1->Map(0, nullptr, (void**)&constMap1);
	if (SUCCEEDED(result)) {
		constMap1->ambient = ambient;
		constMap1->diffuse = diffuse;
		constMap1->specular = specular;
		constMap1->alpha = alpha;
		constMap1->color = m_color;
		constBuffData1->Unmap(0, nullptr);
	}

	//�{�[���z��
	std::vector<FbxModel::Bone>& bones = model->GetBones();
	std::vector<std::pair<std::string, XMMATRIX>> fbxData;

	//��]�s��z��
	std::vector<DirectX::XMMATRIX> localMatRots;

	//�萔�o�b�t�@�փf�[�^�]��(pmx->fbx�ɂ����f�[�^���Ƃ������Ԃ���񂾒l�ɂȂ�)��������
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < bones.size(); i++) {

		//���̎p���s��
		XMMATRIX matCurrentPose;

		//���̎p���s����擾
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);

		//XMMATRIX�ɕϊ�
		FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);

		//�������ăX�L�j���O�s���
		XMMATRIX inverse = XMMatrixInverse(nullptr, model->GetModelTransform());
		XMMATRIX trans = model->GetModelTransform();
		constMapSkin->bones[i] = trans * bones[i].invInitialPose * matCurrentPose * inverse;

		//FbxData�擾�p
		fbxData.push_back(std::make_pair(bones[i].name, trans * matCurrentPose * matWorld));
		FbxVector4 fbxMatRot =
			bones[i].fbxCluster->GetLink()->EvaluateLocalRotation(currentTime);

		//��]�s��擾�p
		DirectX::XMMATRIX matRot = DirectX::XMMatrixIdentity();
		//matRot.r[0].m128_f32[0] = fbxMatRot.mData[0] / 180.0f * 3.14f;
		//matRot.r[1].m128_f32[1] = fbxMatRot.mData[1] / 180.0f * 3.14f;
		//matRot.r[2].m128_f32[2] = fbxMatRot.mData[2] / 180.0f * 3.14f;
		matRot *= DirectX::XMMatrixRotationZ(static_cast<float>(fbxMatRot.mData[0]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationX(static_cast<float>(fbxMatRot.mData[1]) / 180.0f * 3.14f);
		matRot *= DirectX::XMMatrixRotationY(static_cast<float>(fbxMatRot.mData[2]) / 180.0f * 3.14f);
		localMatRots.push_back(matRot);

		//��̃��[���h�s��擾
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
	//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) { return; }

	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineSet.pipelinestate);
	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature);
	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(
		0,
		constBuffTransform->GetGPUVirtualAddress()
	);
	//�萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(
		4,
		constBuffData1->GetGPUVirtualAddress()
	);
	//�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(
		2,
		constBuffSkin->GetGPUVirtualAddress()
	);


	//���C�g�̕`��
	light->Draw(cmdList, 3);
	//�`��
	model->Draw(cmdList);
}

void FbxObject3D::PlayAnimation()
{
	FbxScene* fbxScene = model->GetFbxScene();

	//0�Ԃ̃A�j���[�V�����擾
	FbxAnimStack* animstack = fbxScene->GetSrcObject<FbxAnimStack>(0);

	//�A�j���[�V�����̖��O�擾
	const char* animstackname = animstack->GetName();

	//�A�j���[�V�����̎��ԏ��
	FbxTakeInfo* takeinfo = fbxScene->GetTakeInfo(animstackname);

	//�J�n���Ԏ擾
	startTime = takeinfo->mLocalTimeSpan.GetStart();

	//�I�����Ԏ擾
	endTime = takeinfo->mLocalTimeSpan.GetStop();

	//�J�n���Ԃɍ��킹��
	currentTime = startTime;

	//�Đ����ɂ���
	isPlay = true;
}

void FbxObject3D::AddTexture(ID3D12Resource* texbuff, ID3D12DescriptorHeap* srv)
{
	//1���̂ݑΉ�
	//HRESULT result;
	fbxDescHeap = srv;
	if (srv == nullptr) { assert(0); }

	//D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	//srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//srvDescHeapDesc.NumDescriptors = 1;

	//SRV�p�f�X�N���v�^�q�[�v���쐬
	//result = DirectXImportant::dev->CreateDescriptorHeap(
		//&srvDescHeapDesc,
		//IID_PPV_ARGS(&modelDescHeap));

	//assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; //�ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2D�e�N�X�`��
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
