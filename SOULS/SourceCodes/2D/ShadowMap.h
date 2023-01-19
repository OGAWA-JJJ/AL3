#pragma once
#include "../DirectX/DirectXImportant.h"

#include <wrl.h>
#include <DirectXMath.h>

class ShadowMap
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	struct ConstantBuffer_b0
	{
		DirectX::XMMATRIX mat;			//変換行列
	};

private:
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;

private:
	const int SHADOW_WINDOW_WIDTH = 4096;
	const int SHADOW_WINDOW_HEIGHT = 4096;

private:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;

	UINT incrementSizeSRV = 0;
	UINT incrementSizeDSV = 0;
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
	void PreDraw();
	void PostDraw();

private:
	void CreateConstBuff();
	void CreateSRV();
	void CreateDSV();
};