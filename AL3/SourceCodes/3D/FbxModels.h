#pragma once
#include "FbxMeshes.h"

class FbxModels
{
private:
	static const std::string baseDirectory;

private:
	static ID3D12Device* device;
	static UINT descriptorHandleIncrementSize;

public:
	struct Bone
	{
		std::string name;
		DirectX::XMMATRIX invInitialPose;
		FbxCluster* fbxCluster;
		Bone(const std::string& name) { this->name = name; }
	};

private:
	std::string name;
	std::vector<FbxMeshes*> meshes;
	std::unordered_map<std::string, FbxMaterial*> materials;
	FbxMaterial* defaultMaterial = nullptr;
	ID3D12DescriptorHeap* descHeap = nullptr;
	DirectX::XMFLOAT3 size = { 0,0,0 };
	FbxScene* fbxScene = nullptr;

public:
	static void StaticInit(ID3D12Device* device);
	static FbxModels* CreateFromFbx(const std::string& modelname, bool smoothing = false);

public:
	FbxModels() {}
	~FbxModels();

	void Init(const std::string& modelname, bool smoothing);
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename, FbxSurfaceMaterial* fbx_material);
	void AddMaterial(FbxMaterial* material);
	void CreateDescriptorHeap();
	void LoadTextures();

	void CreateMesh(FbxMesh* fbx_mesh);
	void LoadIndices(FbxMeshes* mesh_data, FbxMesh* mesh);
	void LoadVertices(FbxMeshes* mesh_data, FbxMesh* mesh);
	void LoadNormals(FbxMeshes* mesh_data, FbxMesh* mesh);
	void LoadUV(FbxMeshes* mesh_data, FbxMesh* mesh);
	void LoadColors(FbxMeshes* mesh_data, FbxMesh* mesh);
	void SetMaterialName(FbxMeshes* mesh_data, FbxMesh* mesh);


public:
	inline const std::vector<FbxMeshes*>& GetMeshes() { return meshes; }
	ID3D12DescriptorHeap* GetDescHeap() { return descHeap; }
	DirectX::XMFLOAT3 GetModelSize() { return size; }
	FbxScene* GetFbxScene() { return fbxScene; }
};

