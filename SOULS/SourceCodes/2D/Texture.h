#pragma once
#include "IShaderResource.h"

class Texture :public  IShaderResource
{
private:
	ID3D12Resource* m_texture = nullptr;	//�e�N�X�`���B
	D3D12_RESOURCE_DESC m_textureDesc = {};	//�e�N�X�`�����

public:
	Texture() {}
	~Texture();

	//�t�@�C������e�N�X�`�������[�h����R���X�g���N�^��Load�������s���ꍇ
	//explicit Texture(const wchar_t* filePath);
	//����������e�N�X�`����������
	//void InitFromMemory(const char* memory, unsigned int size);
	//D3D���\�[�X����e�N�X�`��������������
	void InitFromD3DResource(ID3D12Resource* texture);
	//SRV�ɓo�^
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

public:
	//�e�N�X�`�����L��������
	bool IsValid() const { return m_texture != nullptr; }
	//�|�C���^�̎擾
	ID3D12Resource* Get() { return m_texture; }
	//���T�C�Y�擾
	int GetWidth() const { return static_cast<int>(m_textureDesc.Width); }
	//�c�T�C�Y�擾
	int GetHeight() const { return static_cast<int>(m_textureDesc.Height); }
	//�e�N�X�`���̃t�H�[�}�b�g���擾
	DXGI_FORMAT GetFormat() const { return m_textureDesc.Format; }

private:
	//����������e�N�X�`�������[�h
	//void LoadTextureFromMemory(const char* memory, unsigned int size);
};

