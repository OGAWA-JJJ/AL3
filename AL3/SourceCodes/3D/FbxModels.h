#pragma once
#include "FbxMeshes.h"

class FbxModels
{
private:
	static const std::string baseDirectory;

private:
	static ID3D12Device* device;
	static UINT descriptorHandleIncrementSize;

private:
	std::string name;
	std::vector<FbxMeshes*> meshes;
	std::unordered_map<std::string, FbxMaterial*> materials;
	FbxMaterial* defaultMaterial = nullptr;
	ID3D12DescriptorHeap* descHeap = nullptr;
	DirectX::XMFLOAT3 size = { 0,0,0 };

	FbxScene* fbxScene = nullptr;
	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();

	std::vector<std::string> names;

public:
	static void StaticInit(ID3D12Device* device);
	static FbxModels* CreateFromFbx(const std::string& modelname, bool smoothing = false);

public:
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				dst->r[i].m128_f32[j] = (float)src.Get(i, j);
			}
		}
	}

public:
	FbxModels();
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

	void LoadNode(FbxMeshes* mesh_data, FbxNode* fbxNode, FbxMeshes::Node* parent = nullptr);
	void LoadSkin(FbxMeshes* mesh_data, FbxMesh* mesh);

	std::string LoadName(FbxFileTexture* texture);

	void Replace(char search_char, char replace_char, char* buffer);
	void Split(char split_char, char* buffer, std::vector<std::string>& out);


public:
	inline const std::vector<FbxMeshes*>& GetMeshes() { return meshes; }
	ID3D12DescriptorHeap* GetDescHeap() { return descHeap; }
	DirectX::XMFLOAT3 GetModelSize() { return size; }
	FbxScene* GetFbxScene() { return fbxScene; }
	DirectX::XMMATRIX& GetModelTransform() { return transform; }

	std::vector<FbxMeshes::Bone> GetBones()
	{
		std::vector<FbxMeshes::Bone> l_bones;

		for (auto& m : meshes)
		{
			for (int i = 0; i < m->GetBones().size(); i++)
			{
				l_bones.push_back(m->GetBones()[i]);
			}
		}

		return l_bones;
	}
};

