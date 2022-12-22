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
	//�ď������`�F�b�N
	assert(!Object::device);

	//nullptr�`�F�b�N
	assert(device);

	Object::device = device;
	Object::cmdList = cmdList;

	//���f���̐ÓI������
	Model::StaticInit(device);
}

ObjPipelineSet Object::CreateGraphicsPipeline(const ObjectInitData& objectInitData)
{
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;	//���_�V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;	//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;	//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/Shaders/ObjVS.hlsl",					//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�\�ɂ���
		objectInitData.m_vsEntryPoint.c_str(),
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		//errorBlob����G���[���e��string�^�ɃR�s�[
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
		L"Resources/Shaders/ObjPS.hlsl",					//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�\�ɂ���
		objectInitData.m_psEntryPoint.c_str(),
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
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
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //t0 ���W�X�^

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //t1 ���W�X�^

	//���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[5] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	//�ʏ�Ɖe�psampler
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
	//�o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//���[�g�V�O�l�`���̐���
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));
	if (FAILED(result)) {
		assert(0);
	}

	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	//�O���t�B�b�N�X�p�C�v���C���̐���
	result = device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	if (FAILED(result)) {
		assert(0);
	}

	return pipelineSet;
}

std::shared_ptr<Object> Object::Create(std::shared_ptr<Model> model)
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	std::shared_ptr<Object> object = std::make_shared<Object>();
	if (object == nullptr) {
		return nullptr;
	}

	// ������
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
	//nullptr�`�F�b�N
	assert(device);

	HRESULT result;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// �A�b�v���[�h�\
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

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	this->matRot = matRot;

	//���[���h�s��̍���
	matWorld = DirectX::XMMatrixIdentity();	//�ό`�����Z�b�g
	matWorld *= matScale;			//���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot;				//���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans;			//���[���h�s��ɕ��s�ړ��𔽉f

	if (isAffine)
	{
		matWorld *= matrix;
	}

	if (isBillboard) {
		const DirectX::XMMATRIX& matBillboard = Camera::BillboardMatrix();

		matWorld = DirectX::XMMatrixIdentity();
		matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
		matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
		matWorld *= matBillboard;
		matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f
	}

	// �e�I�u�W�F�N�g�������
	if (parent != nullptr) {
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}

	DirectX::XMMATRIX matViewProjection = Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	DirectX::XMFLOAT3 cameraPos = Camera::GetEye();

	// �萔�o�b�t�@�փf�[�^�]��
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
	//nullptr�`�F�b�N
	assert(device);
	assert(Object::cmdList);

	//���f���̊��蓖�Ă��Ȃ���Ε`�悵�Ȃ�
	if (model == nullptr) {
		return;
	}

	//�`��
	Object::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineSet.pipelinestate.Get());
	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature.Get());
	//�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	if (isShadow)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			TexManager::GetGpuHeapStartSRV(),
			TexManager::GetShadowInDsvIndex(),
			TexManager::GetIncrementSizeSRV());
		cmdList->SetGraphicsRootDescriptorTable(4, gpuHandle);
	}

	//���C�g�̕`��
	light->Draw(cmdList, 3);

	//���f���`��
	model->Draw(cmdList);
}
