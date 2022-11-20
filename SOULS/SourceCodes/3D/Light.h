#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>

//using namespace DirectX;
//using namespace Microsoft::WRL;

class Light
{
public:
	struct ConstBufferData {
		DirectX::XMVECTOR lightv; //���C�g�ւ̕�����\���x�N�g��
		DirectX::XMFLOAT3 lightcolor; //���C�g�̐F
		DirectX::XMMATRIX lightViewproj;
	};

private:
	//�f�o�C�X
	static ID3D12Device* device;

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
	DirectX::XMFLOAT3 eye = { 0,100,0 };
	DirectX::XMFLOAT3 target = { 0,0,0 };
	DirectX::XMFLOAT3 up = { 0,0,-1 };

public:
	//�ÓI������
	static void StaticInit(ID3D12Device* device);
	//�C���X�^���X����
	static Light* Create();

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
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	//�e�p���C�g���W���Z�b�g
	void SetShadowLigitEye(DirectX::XMFLOAT3 eye) { this->eye = eye;  dirty = true; }
	void SetShadowLigitTarget(DirectX::XMFLOAT3 target) { this->target = target; dirty = true; }
	void SetShadowLigitUp(DirectX::XMFLOAT3 up) { this->up = up; dirty = true; }

	DirectX::XMFLOAT3 GetShadowLigitEye() { return eye; }
	DirectX::XMFLOAT3 GetShadowLigitTarget() { return target; }
	DirectX::XMFLOAT3 GetShadowLigitUp() { return up; }
};

