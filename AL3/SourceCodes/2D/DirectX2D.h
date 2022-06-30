#pragma once
#include "../DirectX/DirectXImportant.h"
#include <DirectXTex.h>
#include <wrl.h>

const int spriteSRVCount = 512;

class DirectX2D
{
private:
	//スプライト
	struct VertexPosUv {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	//定数バッファ用データ構造体
	struct ConstBufferData {
		DirectX::XMFLOAT4 color; //色（RGBA
		DirectX::XMMATRIX mat;   //3D変換行列
	};

	Microsoft::WRL::ComPtr<ID3D12RootSignature> spriteRootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> spritePipelineState;
	Microsoft::WRL::ComPtr<ID3D12Resource> spriteVertBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> spriteConstBuff;
	D3D12_VERTEX_BUFFER_VIEW spriteVBView{};

	DirectX::XMFLOAT2 anchorpoint = { 0.5f,0.5f };
	bool isFlipX = false;
	bool isFlipY = false;

public:
	struct SpriteData
	{
		float spriteRotation;           //Z軸回りの回転角
		DirectX::XMVECTOR spritePosition{};      //座標
		DirectX::XMMATRIX spriteMatWorld{};      //ワールド行列
		DirectX::XMMATRIX spriteMatProjection{}; //射影行列
		DirectX::XMFLOAT4 spriteColor;           //色
		UINT texNumber;                 //テクスチャ番号
	} sprData;


private:
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{}; //グラフィックスパイプライン設定構造体
	DirectX::TexMetadata metadata{};
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};      //設定構造体

	//ComPtr<ID3D12RootSignature> rootsignature;
	//ComPtr<ID3D12PipelineState> pipelinestate;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap;
	D3D12_INPUT_ELEMENT_DESC inputLayout[2];
	//ComPtr<ID3DBlob> rootSigBlob;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> spriteDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> spriteTexBuff[spriteSRVCount];

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
	//場所の設定
	void SetPos(DirectX::XMFLOAT2 pos) {
		sprData.spritePosition = { pos.x,pos.y,0 };
	}
};

