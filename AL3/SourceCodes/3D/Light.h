#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>

using namespace DirectX;
using namespace Microsoft::WRL;

class Light
{
public:
	struct ConstBufferData {
		XMVECTOR lightv; //ライトへの方向を表すベクトル
		XMFLOAT3 lightcolor; //ライトの色
		XMMATRIX lightViewproj;
	};

private:
	//デバイス
	static ID3D12Device* device;

private:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ライト光線方向（単位ベクトル）
	XMVECTOR lightdir = { 1,0,0,0 };
	//ライト色
	XMFLOAT3 lightcolor = { 1,1,1 };
	//ダーティフラグ
	bool dirty = false;

public:
	//静的初期化
	static void StaticInit(ID3D12Device* device);
	//インスタンス生成
	static Light* Create();

public:
	//ライト
	void Init();
	//定数バッファ転送
	void TransferConstBuffer();
	//ライト方向をセット
	void SetLightDir(const XMVECTOR& lightdir);
	//ライト色をセット
	void SetLightColor(const XMFLOAT3& lightcolor);
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);
};

