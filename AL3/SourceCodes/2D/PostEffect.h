#pragma once
#include "Sprite.h"
#include "Texture.h"
#include <array>

//AlphaBlendMode
enum AlphaBlendMode {
	ALPHA_BLENDMODE_NONE,	//無し
	ALPHA_BLENDMODE_TRANS,	//半透明合成
	ALPHA_BLENDMODE_ADD,	//加算合成
};

//情報の構造体
struct SpriteInitData {
	std::array<const wchar_t*, 100> m_fileName = { nullptr };				//ファイルパス名
	std::array<Texture*, 100> m_textures = { nullptr };						//使用するテクスチャ
	const char* m_vsEntryPoint = "VSmain";								//頂点シェーダーのエントリーポイント
	const char* m_psEntryPoint = "PSmain";								//ピクセルシェーダーのエントリーポイント
	const wchar_t* m_vsShaderName =
		L"Resources/Shaders/PostEffectTestVS.hlsl";						//読み込むvsの名前
	const wchar_t* m_psShaderName =
		L"Resources/Shaders/PostEffectTestPS.hlsl";						//読み込むpsの名前
	UINT m_width = 0;													//スプライトの幅。
	UINT m_height = 0;													//スプライトの高さ。
	void* m_expandConstantBuffer = nullptr;								//ユーザー拡張の定数バッファ
	int m_expandConstantBufferSize = 0;									//ユーザー拡張の定数バッファのサイズ。
	//IShaderResource* m_expandShaderResoruceView = nullptr;			//ユーザー拡張のシェーダーリソース。
	AlphaBlendMode m_alphaBlendMode = ALPHA_BLENDMODE_NONE;				//AlphaBlendMode
	//D3D12_FILTER m_samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//サンプラのフィルター。
	//std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> m_colorBufferFormat = {
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//	DXGI_FORMAT_UNKNOWN,
	//};	//レンダリングするカラーバッファのフォーマット。
};

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

private:
	//情報取得用
	DXGI_FORMAT texresDescFormat;

public:
	PostEffect();
	void Init(const SpriteInitData& spriteInitData);
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//シーン描画前処理
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
	//シーン描画後処理
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

private:
	//パイプライン生成(中身汎用性高めて細かく関数化した方がいいかも)
	void CreateGraphicsPipelineState(const SpriteInitData& spriteInitData);

	//テクスチャ生成
	//SRV作成
	//RTV作成
	//深度バッファ生成
	//DSV作成

public:
	DXGI_FORMAT GetFormat()
	{
		return texresDescFormat;
	}
};