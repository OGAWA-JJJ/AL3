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
		DirectX::XMFLOAT3 pos;			//xyz座標
		DirectX::XMFLOAT2 uv;			//uv座標
	};

	struct ConstantBuffer_b0
	{
		DirectX::XMMATRIX mat;			//変換行列
	};

private:
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;

private:
	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファ
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