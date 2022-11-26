#pragma once
#include <d3d12.h>
#include "../DirectX/d3dx12.h"
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

class Material
{
public:
	struct ConstBufferDataB1
	{
		DirectX::XMFLOAT3 ambient; // アンビエント係数
		float pad1; // パディング
		DirectX::XMFLOAT3 diffuse; // ディフューズ係数
		float pad2; // パディング
		DirectX::XMFLOAT3 specular; // スペキュラー係数
		float alpha;	// アルファ
	};

public:
	//マテリアル名
	std::string name;
	//アンビエント影響度
	DirectX::XMFLOAT3 ambient;
	//ディフューズ影響度
	DirectX::XMFLOAT3 diffuse;
	//スペキュラー影響度
	DirectX::XMFLOAT3 specular;
	//アルファ
	float alpha;
	//テクスチャファイル名
	std::string textureFilename;

private:
	//デバイス
	static ID3D12Device* device;
	//テクスチャバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	//シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	//シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

public:
	//静的初期化
	static void StaticInit(ID3D12Device* device);
	//マテリアル生成
	static Material* Create();

public:
	//テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	//更新
	void Update();

public:
	//定数バッファの取得
	ID3D12Resource* GetConstantBuffer() { return constBuff.Get(); }

	const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV; }
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV; }

private:
	Material() {
		ambient = { 0.4f, 0.4f, 0.4f };
		diffuse = { 0.8f, 0.8f, 0.8f };
		specular = { 0.4f, 0.4f, 0.4f };
		alpha = 1.0f;
	}

	//初期化
	void Init();
	//定数バッファの生成
	void CreateConstantBuffer();
};

