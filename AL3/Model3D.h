#pragma once
#include "fstream"
#include "sstream"
#include "string"
#include "vector"
#include "assert.h"

#include "DirectXImportant.h"
#include "Camera.h"

using namespace std;

class Model3D
{
private:
	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
	};

	struct ConstBufferDataB0 {
		XMMATRIX mat;
	};
	ConstBufferDataB0* constMapB0 = nullptr;

	struct ConstBufferDataB1 {
		XMFLOAT3 ambient;
		float pad1;
		XMFLOAT3 diffuse;
		float pad2;
		XMFLOAT3 specular;
		float alpha;
	};
	ConstBufferDataB1* constMapB1 = nullptr;

	struct Material
	{
		string name;
		XMFLOAT3 ambient;
		XMFLOAT3 diffuse;
		XMFLOAT3 specular;
		float alpha;
		string textureFilename;

		Material() {
			ambient = { 1.0f,1.0f,1.0f };
			diffuse = { 1.0f,1.0f,1.0f };
			specular = { 1.0f,1.0f,1.0f };
			alpha = 1.0f;
		}
	};

public:
	static vector<Vertex> vertices;
	static vector<unsigned short> indices;
	static Material material;

private:
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	ComPtr<ID3D12Resource> constBuffB0;
	ComPtr<ID3D12Resource> constBuffB1;
	ComPtr<ID3D12Resource> texbuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle;
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;
	ComPtr<ID3D12PipelineState> pipelinestate;
	ComPtr<ID3D12RootSignature> rootsignature;
	XMMATRIX matWorld;

public:
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMFLOAT3 position;
	XMMATRIX matRot;

public:
	Model3D();
	void CreateDescriptorHeap();
	void CreateGraphicsPipeline();
	void LoadTexture(const string& directoryPath, const string& filename);
	void LoadMaterial(const string& directoryPath, const string& filename);
	void CreateModel(const string& modelName);
	void Init();
	void Update();
	void Draw();
	XMMATRIX GetmatRot();
	XMFLOAT3 GetPosition();
};

