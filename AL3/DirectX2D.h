#pragma once
#include "DirectXImportant.h"

const int spriteSRVCount = 512;

class DirectX2D
{
private:
	//スプライト
	struct VertexPosUv {
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	//定数バッファ用データ構造体
	struct ConstBufferData {
		XMFLOAT4 color; //色（RGBA
		XMMATRIX mat;   //3D変換行列
	};

	ComPtr<ID3D12RootSignature> spriteRootSignature;
	ComPtr<ID3D12PipelineState> spritePipelineState;
	ComPtr<ID3D12Resource> spriteVertBuff;
	ComPtr<ID3D12Resource> spriteConstBuff;
	D3D12_VERTEX_BUFFER_VIEW spriteVBView{};

	XMFLOAT2 anchorpoint = { 0.5f,0.5f };
	bool isFlipX = false;
	bool isFlipY = false;

public:
	struct SpriteData
	{
		float spriteRotation;           //Z軸回りの回転角
		XMVECTOR spritePosition{};      //座標
		XMMATRIX spriteMatWorld{};      //ワールド行列
		XMMATRIX spriteMatProjection{}; //射影行列
		XMFLOAT4 spriteColor;           //色
		UINT texNumber;                 //テクスチャ番号
	} sprData;


private:
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{}; //グラフィックスパイプライン設定構造体
	TexMetadata metadata{};
	ComPtr<ID3D12Resource> texbuff;
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};      //設定構造体

	//ComPtr<ID3D12RootSignature> rootsignature;
	//ComPtr<ID3D12PipelineState> pipelinestate;
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;
	D3D12_INPUT_ELEMENT_DESC inputLayout[2];
	//ComPtr<ID3DBlob> rootSigBlob;

	ComPtr<ID3D12DescriptorHeap> spriteDescHeap;
	ComPtr<ID3D12Resource> spriteTexBuff[spriteSRVCount];

private:
	void VSShaderCompile();
	void PSShaderCompile();
	void InputLayout();
	void SettingPipelineState();
	void RootSignature();
	void CreatePipelineState();

public:
	DirectX2D();
	//画像読み込み
	HRESULT SpriteLoadTexture(UINT texnumber, const wchar_t* filename);
	//パイプラインステートオブジェクト生成（初期化）
	void PipelineStateObjInit();
	//スプライト生成（初期化）
	HRESULT CreateSpriteInit();
	//パイプライン設定コマンド（毎フレーム処理）
	void SpriteUpdate();
	//描画コマンド（毎フレーム処理）
	void SpriteDraw(bool isDraw = true);
};

