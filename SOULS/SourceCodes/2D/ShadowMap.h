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
	};

private:
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;

private:
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

	UINT incrementSizeSRV = 0;
	UINT incrementSizeRTV = 0;
	UINT incrementSizeDSV = 0;
	UINT srvIndex = 0;
	UINT rtvIndex = 0;
	UINT dsvIndex = 0;
	UINT inDsvIndex = 0;

public:
	static void StaticInit(
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		ShadowMap::cmdList = cmdList;
	}

public:
	ShadowMap();
	~ShadowMap();
	void Init();
	void Draw();
	void PreDraw();
	void PostDraw();

private:
	void CreateGraphicsPipelineState();
	void CreateVBV();
	void CreateConstBuff();
	void CreateTexBuff();
	void CreateSRV();
	void CreateRTV();
	void CreateDSV();
};