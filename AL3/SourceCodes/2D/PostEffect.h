#pragma once
//#include "Sprite.h"
#include "Texture.h"
#include <wrl.h>
#include <array>
#include <DirectXMath.h>

//AlphaBlendMode
enum AlphaBlendMode {
	ALPHA_BLENDMODE_NONE,	//無し
	ALPHA_BLENDMODE_TRANS,	//半透明合成
	ALPHA_BLENDMODE_ADD,	//加算合成
};

//情報の構造体
struct SpriteInitData {
	std::array<const wchar_t*, 100> m_fileName = { nullptr };			//ファイルパス名
	std::array<Texture*, 100> m_textures = { nullptr };					//使用するテクスチャ
	const char* m_vsEntryPoint = "VSmain";								//頂点シェーダーのエントリーポイント
	const char* m_psEntryPoint = "PSmain";								//ピクセルシェーダーのエントリーポイント
	const wchar_t* m_vsShaderName =
		L"Resources/Shaders/PostEffectTestVS.hlsl";						//読み込むvsの名前
	const wchar_t* m_psShaderName =
		L"Resources/Shaders/PostEffectTestPS.hlsl";						//読み込むpsの名前
	UINT m_width = 1280;												//スプライトの幅。
	UINT m_height = 720;												//スプライトの高さ。
	void* m_expandConstantBuffer = nullptr;								//ユーザー拡張の定数バッファ
	int m_expandConstantBufferSize = 0;									//ユーザー拡張の定数バッファのサイズ。
	//IShaderResource* m_expandShaderResoruceView = nullptr;			//ユーザー拡張のシェーダーリソース。
	AlphaBlendMode m_alphaBlendMode = ALPHA_BLENDMODE_TRANS;			//AlphaBlendMode
	float m_size = 1.0f;												//今使ってない
	float m_gaussianSigma = 8.0f;										//ガウシアンブラーの重み
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

class PostEffect
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
		DirectX::XMFLOAT4 color;		//色
		DirectX::XMMATRIX mat;			//変換行列
		DirectX::XMMATRIX viewproj;
	};

	struct ConstantBuffer_b1//未転送
	{
		float weights[8];
	};

public:
	static float clearColor[4];

private:
	//頂点数
	static const int vertNum = 4;
	static const int NUM_HEIGHT = 8;

private:
	float weight[NUM_HEIGHT];

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

	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//定数バッファ（重み転送用）
	ComPtr<ID3D12Resource> constBuff_b1;
	//色
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };

private:
	//情報取得用
	DXGI_FORMAT texresDescFormat;

public:
	PostEffect();
	void Init(const SpriteInitData& spriteInitData);
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//シーン描画前処理
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList, const SpriteInitData& spriteInitData);
	//シーン描画後処理
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

private:
	//パイプライン生成(中身汎用性高めて細かく関数化した方がいいかも)
	void CreateGraphicsPipelineState(const SpriteInitData& spriteInitData);

	void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma);

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