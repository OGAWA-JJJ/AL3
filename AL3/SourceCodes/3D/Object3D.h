#pragma once
#include "fstream"
#include "sstream"
#include "string"
#include "vector"
#include "assert.h"

#include "../DirectX/DirectXImportant.h"
#include "../DirectX/Camera.h"

//using namespace std;

class Model3D
{
private:
	struct Vertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};

	struct ConstBufferDataB0 {
		DirectX::XMMATRIX mat;
	};
	ConstBufferDataB0* constMapB0 = nullptr;

	struct ConstBufferDataB1 {
		DirectX::XMFLOAT3 ambient;
		float pad1;
		DirectX::XMFLOAT3 diffuse;
		float pad2;
		DirectX::XMFLOAT3 specular;
		float alpha;
	};
	ConstBufferDataB1* constMapB1 = nullptr;

	struct Material
	{
		std::string name;
		DirectX::XMFLOAT3 ambient;
		DirectX::XMFLOAT3 diffuse;
		DirectX::XMFLOAT3 specular;
		float alpha;
		std::string textureFilename;

		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

public:
	static std::vector<Vertex> vertices;
	static std::vector<unsigned short> indices;
	static Material material;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB1;
	Microsoft::WRL::ComPtr<ID3D12Resource> texbuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> basicDescHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
	DirectX::XMMATRIX matWorld;

public:
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 position;
	DirectX::XMMATRIX matRot;

public:
	Model3D();
	void CreateDescriptorHeap();
	void CreateGraphicsPipeline();
	void LoadTexture(const std::string& directoryPath, const std::string& filename);
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	void CreateModel(const std::string& modelName);
	void Init();
	void Update();
	void Draw();
	DirectX::XMMATRIX GetmatRot();
	DirectX::XMFLOAT3 GetPosition();
};

