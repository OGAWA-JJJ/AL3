#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <DirectXMath.h>
#include <string>

class PostEffect
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//AlphaBlendMode
	enum AlphaBlendMode
	{
		ALPHA_BLENDMODE_NONE,	//無し
		ALPHA_BLENDMODE_TRANS,	//半透明合成
		ALPHA_BLENDMODE_ADD,	//加算合成
	};

	//情報の構造体
	struct SpriteInitData
	{
		std::string m_vsEntryPoint = "VSmain";								//頂点シェーダーのエントリーポイント
		std::string m_psEntryPoint = "PSmain";								//ピクセルシェーダーのエントリーポイント
		const wchar_t* m_vsShaderName =
			L"Resources/Shaders/PostEffectTestVS.hlsl";						//読み込むvsの名前
		const wchar_t* m_psShaderName =
			L"Resources/Shaders/PostEffectTestPS.hlsl";						//読み込むpsの名前
		UINT m_width = 1280;												//スプライトの幅。
		UINT m_height = 720;												//スプライトの高さ。
		AlphaBlendMode m_alphaBlendMode = ALPHA_BLENDMODE_TRANS;			//AlphaBlendMode
		float m_size = 1.0f;												//今使ってない
		float m_gaussianSigma = 8.0f;										//ガウシアンブラーの重み
	};

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
	static ComPtr<ID3D12GraphicsCommandList> cmdList;

private:
	float weight[NUM_HEIGHT];

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
	//定数バッファ（重み転送用）
	ComPtr<ID3D12Resource> constBuff_b1;
	//色
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
	//初期化情報
	SpriteInitData spriteInitData;

	UINT incrementSizeSRV = 0;
	UINT incrementSizeRTV = 0;
	UINT incrementSizeDSV = 0;
	UINT srvIndex = 0;
	UINT rtvIndex = 0;
	UINT dsvIndex = 0;

public:
	static void StaticInit(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		PostEffect::cmdList = cmdList;
	}

public:
	PostEffect();
	void Init(const SpriteInitData& spriteInitData);
	void Draw();
	//シーン描画前処理
	void PreDrawScene(const float* clearColor);
	//シーン描画後処理
	void PostDrawScene();

private:
	//パイプライン生成(中身汎用性高めて細かく関数化した方がいいかも)
	void CreateGraphicsPipelineState(const SpriteInitData& spriteInitData);

	void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma);

	void CreateTexBuff();
	void CreateSRV();
	void CreateRTV();
	void CreateDSV();
};