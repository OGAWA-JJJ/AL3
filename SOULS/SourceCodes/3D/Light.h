#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "../DirectX/d3dx12.h"

class Light
{
public:
	struct ConstBufferData {
		DirectX::XMVECTOR lightv;		//���C�g�ւ̕�����\���x�N�g��
		DirectX::XMFLOAT3 lightcolor;	//���C�g�̐F
		DirectX::XMMATRIX lightViewproj;
	};

private:
	//�f�o�C�X
	static Microsoft::WRL::ComPtr<ID3D12Device> device;

private:
	//�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	//���C�g���������i�P�ʃx�N�g���j
	DirectX::XMVECTOR lightdir = { 1,0,0,0 };
	//���C�g�F
	DirectX::XMFLOAT3 lightcolor = { 1,1,1 };
	//�_�[�e�B�t���O
	bool dirty = false;

	//�e�p���C�g���W
	DirectX::XMFLOAT3 shadowEye = { 300,300,-300 };
	DirectX::XMFLOAT3 shadowTarget = { 0,0,0 };
	DirectX::XMFLOAT3 shadowUp = { 0,1,0 };

public:
	//�ÓI������
	static void StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device);
	//�C���X�^���X����
	static std::shared_ptr<Light> Create();

public:
	//���C�g
	void Init();
	//�萔�o�b�t�@�]��
	void TransferConstBuffer();
	//���C�g�������Z�b�g
	void SetLightDir(const DirectX::XMVECTOR& lightdir);
	//���C�g�F���Z�b�g
	void SetLightColor(const DirectX::XMFLOAT3& lightcolor);
	//�X�V
	void Update();
	//�`��
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, UINT rootParameterIndex);

	//�e�p���C�g���W���Z�b�g
	void SetShadowLigitEye(DirectX::XMFLOAT3 eye) { shadowEye = eye;  dirty = true; }
	void SetShadowLigitTarget(DirectX::XMFLOAT3 target) { shadowTarget = target; dirty = true; }
	void SetShadowLigitUp(DirectX::XMFLOAT3 up) { shadowUp = up; dirty = true; }

	DirectX::XMFLOAT3 GetShadowLigitEye() { return shadowEye; }
	DirectX::XMFLOAT3 GetShadowLigitTarget() { return shadowTarget; }
	DirectX::XMFLOAT3 GetShadowLigitUp() { return shadowUp; }
};

