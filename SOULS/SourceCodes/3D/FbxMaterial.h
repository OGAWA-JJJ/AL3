#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

#pragma warning(push)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:26812)
#include "fbxsdk.h"
#pragma warning(pop)

class FbxMaterial
{
public:
	struct ConstBufferDataB1
	{
		DirectX::XMFLOAT3 ambient;	//アンビエント係数
		DirectX::XMFLOAT3 diffuse;	//ディフューズ係数
		DirectX::XMFLOAT3 specular;	//スペキュラー係数
		DirectX::XMFLOAT4 color;

		DirectX::XMFLOAT3 pad;
	};

private:
	static ID3D12Device* device;

public:
	DirectX::XMFLOAT3 ambient;
	DirectX::XMFLOAT3 diffuse;
	DirectX::XMFLOAT3 specular;
	DirectX::XMFLOAT4 color;

private:
	std::string name;
	std::string textureFilename;

	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

public:
	static void StaticInit(ID3D12Device* dev);
	static FbxMaterial* Create();

public:
	void LoadTexture(const std::string& directoryPath, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	void Update();

private:
	FbxMaterial() {
		ambient = { 0.8f, 0.8f, 0.8f };
		diffuse = { 0.8f, 0.8f, 0.8f };
		specular = { 0.8f, 0.8f, 0.8f };
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

private:
	void Init();
	void CreateConstantBuffer();

public:
	ID3D12Resource* GetConstantBuffer() { return constBuff.Get(); }

	const CD3DX12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return cpuDescHandleSRV; }
	const CD3DX12_GPU_DESCRIPTOR_HANDLE& GetGpuHandle() { return gpuDescHandleSRV; }

	std::string& GetName() { return name; }
	const std::string& GetTextureFilename() { return textureFilename; }
	void SetTextureFileName(std::string& fileName) { textureFilename = fileName; }

	void SetMaterial(const ConstBufferDataB1& constBuffData01)
	{
		ambient = constBuffData01.ambient;
		diffuse = constBuffData01.diffuse;
		specular = constBuffData01.specular;
		color = constBuffData01.color;
	}
	void SetName(const std::string& name) { this->name = name; }
};

