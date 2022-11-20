#pragma once
#include "../DirectX/DirectXImportant.h"
#include "../2D/Texture.h"

class RenderTarget
{
private:
	Texture m_renderTargetTexture;
	ID3D12Resource* m_renderTargetTextureDx12 = nullptr;	//�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���B
	ID3D12Resource* m_depthStencilTexture = nullptr;		//�[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`���B
	ID3D12DescriptorHeap* m_rtvHeap = nullptr;				//RTV�p�̃f�B�X�N���v�^�q�[�v�B
	ID3D12DescriptorHeap* m_dsvHeap = nullptr;				//�[�x�X�e���V���o�b�t�@�r���[�̃f�B�X�N���v�^�q�[�v�B
	UINT m_rtvDescriptorSize = 0;							//�t���[���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y�B
	UINT m_dsvDescriptorSize = 0;							//�[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y�B
	int m_width = 0;										//�����_�����O�^�[�Q�b�g�̕��B
	int m_height = 0;										//�����_�����O�^�[�Q�b�g�̍����B
	float m_rtvClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	//�����_�����O�^�[�Q�b�g�r���[�̃N���A�J���[�B
	float m_dsvClearValue = 1.0f;							//DSV�̃N���A�J���[�B

public:
	RenderTarget() {};
	~RenderTarget();

	bool Create(
		int w,
		int h,
		int mipLevel,
		int arraySize,
		DXGI_FORMAT colorFormat,
		DXGI_FORMAT depthStencilFormat,
		float clearColor[4] = nullptr
	);
	bool CreateDescriptorHeap(ID3D12Device*& d3dDevice);
	bool CreateRenderTargetTexture(
		ID3D12Device*& d3dDevice,
		int w,
		int h,
		int mipLevel,
		int arraySize,
		DXGI_FORMAT format,
		float clearColor[4]
	);
	bool CreateDepthStencilTexture(
		ID3D12Device*& d3dDevice,
		int w,
		int h,
		DXGI_FORMAT format
	);
	void CreateDescriptor(ID3D12Device*& d3dDevice);

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCpuDescriptorHandle() const
	{
		return m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuDescriptorHandle() const
	{
		return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}
	Texture& GetRenderTargetTexture()
	{
		return m_renderTargetTexture;
	}
	bool IsExsitDepthStencilBuffer() const
	{
		return m_depthStencilTexture;
	}
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	DXGI_FORMAT GetColorBufferFormat() const
	{
		return m_renderTargetTexture.GetFormat();
	}
	const float* GetRTVClearColor() const { return m_rtvClearColor; }
	float GetDSVClearValue() const { return m_dsvClearValue; }
};

