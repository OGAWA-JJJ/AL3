#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include "../DirectX/d3dx12.h"

class Light
{
public:
	struct ConstBufferData {
		DirectX::XMVECTOR lightv;		//ライトへの方向を表すベクトル
		DirectX::XMFLOAT3 lightcolor;	//ライトの色
		DirectX::XMMATRIX lightViewproj;
	};

private:
	//デバイス
	static Microsoft::WRL::ComPtr<ID3D12Device> device;

private:
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	//ライト光線方向（単位ベクトル）
	DirectX::XMVECTOR lightdir = { 1,0,0,0 };
	//ライト色
	DirectX::XMFLOAT3 lightcolor = { 1,1,1 };
	//ダーティフラグ
	bool dirty = false;

	//影用ライト座標
	DirectX::XMFLOAT3 shadowEye = { 300,300,-300 };
	DirectX::XMFLOAT3 shadowTarget = { 0,0,0 };
	DirectX::XMFLOAT3 shadowUp = { 0,1,0 };

public:
	//静的初期化
	static void StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device);
	//インスタンス生成
	static std::shared_ptr<Light> Create();

public:
	//ライト
	void Init();
	//定数バッファ転送
	void TransferConstBuffer();
	//ライト方向をセット
	void SetLightDir(const DirectX::XMVECTOR& lightdir);
	//ライト色をセット
	void SetLightColor(const DirectX::XMFLOAT3& lightcolor);
	//更新
	void Update();
	//描画
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, UINT rootParameterIndex);

	//影用ライト座標をセット
	void SetShadowLigitEye(DirectX::XMFLOAT3 eye) { shadowEye = eye;  dirty = true; }
	void SetShadowLigitTarget(DirectX::XMFLOAT3 target) { shadowTarget = target; dirty = true; }
	void SetShadowLigitUp(DirectX::XMFLOAT3 up) { shadowUp = up; dirty = true; }

	DirectX::XMFLOAT3 GetShadowLigitEye() { return shadowEye; }
	DirectX::XMFLOAT3 GetShadowLigitTarget() { return shadowTarget; }
	DirectX::XMFLOAT3 GetShadowLigitUp() { return shadowUp; }
};

