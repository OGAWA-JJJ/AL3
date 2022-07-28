#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include "FbxMaterial.h"

class FbxMeshes
{
public:
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos;		//xyz座標
		DirectX::XMFLOAT3 normal;	//法線ベクトル
		DirectX::XMFLOAT2 uv;		//uv座標
		DirectX::XMFLOAT4 color;
	};

	struct Node
	{
		std::string name;
		DirectX::XMVECTOR scaling = { 1,1,1,0 };
		DirectX::XMVECTOR rotation = { 0,0,0,0 };
		DirectX::XMVECTOR translation = { 0,0,0,1 };
		DirectX::XMMATRIX transform;
		DirectX::XMMATRIX globalTransform;
		Node* parent = nullptr;
	};

	struct Bone
	{
		std::string name;
		DirectX::XMMATRIX invInitialPose;
		FbxCluster* fbxCluster;
		Bone(const std::string& name) { this->name = name; }
	};

	static const int MAX_BONE_INDICES = 4;
	struct Skin
	{
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};

private:
	static ID3D12Device* device;

private:
	std::string name;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	std::vector<VertexPosNormalUv> vertices;
	std::vector<unsigned short> indices;
	FbxMaterial* material = nullptr;
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;

	std::vector<Bone> bones;
	std::vector<Node> nodes;
	Node* meshNode = nullptr;
	std::vector<Skin> skinVert;

public:
	static void StaticInit(ID3D12Device* dev);

public:
	FbxMeshes() {}
	~FbxMeshes() {}

	void SetName(const std::string& name);
	void AddVertex(const VertexPosNormalUv& vertex);
	void AddIndex(unsigned short index);
	void SetMaterial(FbxMaterial* material);
	void CreateBuffers();
	void Draw(ID3D12GraphicsCommandList* cmdList, FbxMaterial* material);
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);
	void CalculateSmoothedVertexNormals();

public:
	const std::string& GetName() { return name; }
	inline size_t GetVertexCount() { return vertices.size(); }
	FbxMaterial* GetMaterial() { return material; }
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
	inline std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }
	inline const std::vector<unsigned short>& GetIndices() { return indices; }

	std::vector<Bone>& GetBones() { return bones; }
	std::vector<Node>& GetNodes() { return nodes; }
	std::vector<Skin>& GetSkins() { return skinVert; }
	void SetMeshNode(Node* meshNode) { this->meshNode = meshNode; }
};

