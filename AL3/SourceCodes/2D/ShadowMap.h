#pragma once
#include "../DirectX/DirectXImportant.h"
#include "../3D/FbxObject3D.h"

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
		//DirectX::XMMATRIX viewproj;
		//DirectX::XMMATRIX lightViewproj;
	};

private:
	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObj1 = nullptr;

private:
	//テクスチャバッファ
	ID3D12Resource* texbuff;
	//SRv用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource> depthbuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
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
	//定数バッファ（重み転送用）
	//ComPtr<ID3D12Resource> constBuff_b0;
	//色
	//DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

public:
	ShadowMap();
	~ShadowMap();
	void Init();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void PreDraw(ID3D12GraphicsCommandList* cmdList);
	void PostDraw(ID3D12GraphicsCommandList* cmdList);
	ID3D12Resource* GetTexbuffer() { return texbuff; }

private:
	void CreateGraphicsPipelineState();
};