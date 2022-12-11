#pragma once
#include "../DirectX/DirectXImportant.h"

#include <wrl.h>
#include <DirectXMath.h>

class ShadowMap
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos;			//xyz���W
		DirectX::XMFLOAT2 uv;			//uv���W
	};

	struct ConstantBuffer_b0
	{
		DirectX::XMMATRIX mat;			//�ϊ��s��
		//DirectX::XMMATRIX viewproj;
		//DirectX::XMMATRIX lightViewproj;
	};

private:
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthbuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
	//�O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//�萔�o�b�t�@�i�d�ݓ]���p�j
	//ComPtr<ID3D12Resource> constBuff_b0;
	//�F
	//DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

public:
	ShadowMap();
	~ShadowMap();
	void Init();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void PreDraw(ID3D12GraphicsCommandList* cmdList);
	void PostDraw(ID3D12GraphicsCommandList* cmdList);
	ComPtr<ID3D12Resource> GetTexbuffer() { return texbuff; }

private:
	void CreateGraphicsPipelineState();
};