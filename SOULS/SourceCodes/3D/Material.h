#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

//using namespace DirectX;
//using namespace Microsoft::WRL;
//using namespace std;

class Material
{
public:
	struct ConstBufferDataB1
	{
		DirectX::XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		DirectX::XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		DirectX::XMFLOAT3 specular; // �X�y�L�����[�W��
		float alpha;	// �A���t�@
	};

public:
	//�}�e���A����
	std::string name;
	//�A���r�G���g�e���x
	DirectX::XMFLOAT3 ambient;
	//�f�B�t���[�Y�e���x
	DirectX::XMFLOAT3 diffuse;
	//�X�y�L�����[�e���x
	DirectX::XMFLOAT3 specular;
	//�A���t�@
	float alpha;
	//�e�N�X�`���t�@�C����
	std::string textureFilename;

private:
	//�f�o�C�X
	static ID3D12Device* device;
	//�e�N�X�`���o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	//�萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	//�V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	//�V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

public:
	//�ÓI������
	static void StaticInit(ID3D12Device* device);
	//�}�e���A������
	static Material* Create();

public:
	//�e�N�X�`���ǂݍ���
	void LoadTexture(const std::string& directoryPath, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	//�X�V
	void Update();

public:
	//�萔�o�b�t�@�̎擾
	ID3D12Resource* GetConstantBuffer() { return constBuff.Get(); }

	const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV; }
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV; }

private:
	Material() {
		ambient = { 0.4f, 0.4f, 0.4f };
		diffuse = { 0.8f, 0.8f, 0.8f };
		specular = { 0.4f, 0.4f, 0.4f };
		alpha = 1.0f;
	}

	//������
	void Init();
	//�萔�o�b�t�@�̐���
	void CreateConstantBuffer();
};

