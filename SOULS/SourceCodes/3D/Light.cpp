#include "Light.h"
#include "../DirectX/ConstantBuffer.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/Camera.h"

Microsoft::WRL::ComPtr<ID3D12Device> Light::device = nullptr;

void Light::StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	//�ď������`�F�b�N
	assert(!Light::device);
	//nullptr�`�F�b�N
	assert(device);
	//�ÓI�����o�ϐ��̃Z�b�g
	Light::device = device;
}

std::shared_ptr<Light> Light::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	std::shared_ptr<Light> instance = std::make_shared<Light>();
	//������
	instance->Init();
	//���������C���X�^���X��Ԃ�
	return instance;
}

void Light::Init()
{
	HRESULT result;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) { assert(0); }

	//�萔�o�b�t�@�փf�[�^�]��
	TransferConstBuffer();
}

void Light::TransferConstBuffer()
{
	//�e�p
	DirectX::XMMATRIX matView = DirectX::XMMatrixLookAtLH(
		XMLoadFloat3(&shadowEye),
		XMLoadFloat3(&shadowTarget),
		XMLoadFloat3(&shadowUp));

	float l_width = static_cast<float>(WINDOW_WIDTH / 2);
	float l_height = static_cast<float>(WINDOW_HEIGHT / 2);
	float l_far = ImguiControl::Imgui_far_z;

	DirectX::XMMATRIX lightMatPerspective =
		DirectX::XMMatrixOrthographicOffCenterLH(
			-l_width, l_width,
			-l_height, l_height,
			1.0f, l_far);

	const DirectX::XMMATRIX& lightMatViewProjection = matView * lightMatPerspective;

	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData constMap;
	constMap.lightv = lightdir;
	constMap.lightcolor = lightcolor;
	constMap.lightViewproj = lightMatViewProjection;

	ConstantBuffer::CopyToVRAM(constBuff, &constMap, sizeof(ConstBufferData));
}

void Light::SetLightDir(const DirectX::XMVECTOR& lightdir)
{
	//���K�����ăZ�b�g
	this->lightdir = DirectX::XMVector3Normalize(lightdir);
	dirty = true;
}

void Light::SetLightColor(const DirectX::XMFLOAT3& lightcolor)
{
	this->lightcolor = lightcolor;
	dirty = true;
}

void Light::Update()
{
	//�l�̍X�V���������������萔�o�b�t�@�ɓ]������
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList,
	UINT rootParameterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(
		rootParameterIndex,
		constBuff->GetGPUVirtualAddress()
	);
}
