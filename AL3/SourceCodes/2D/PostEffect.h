#pragma once
#include "Sprite.h"

class PostEffect :public Sprite
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	static const float clearColor[4];

private:
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
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

public:
	PostEffect();
	void Init();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//シーン描画前処理
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
	//シーン描画後処理
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
	//パイプライン生成(中身汎用性高めて細かく関数化した方がいいかも)
	void CreateGraphicsPipelineState();

	//テクスチャ生成
	//SRV作成
	//RTV作成
	//深度バッファ生成
	//DSV作成
};