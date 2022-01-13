#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

class Material
{
public:
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient; // アンビエント係数
		float pad1; // パディング
		XMFLOAT3 diffuse; // ディフューズ係数
		float pad2; // パディング
		XMFLOAT3 specular; // スペキュラー係数
		float alpha;	// アルファ
	};

public:
	//マテリアル名
	std::string name;
	//アンビエント影響度
	XMFLOAT3 ambient;
	//ディフューズ影響度
	XMFLOAT3 diffuse;
	//スペキュラー影響度
	XMFLOAT3 specular;
	//アルファ
	float alpha;
	//テクスチャファイル名
	std::string textureFilename;

private:
	//デバイス
	static ID3D12Device* device;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
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
		ambient = { 0.3f, 0.3f, 0.3f };
		diffuse = { 0.0f, 0.0f, 0.0f };
		specular = { 0.0f, 0.0f, 0.0f };
		alpha = 1.0f;
	}

	//初期化
	void Init();
	//定数バッファの生成
	void CreateConstantBuffer();
};

