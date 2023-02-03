#pragma once
#include "FbxMeshes.h"

class FbxModels
{
private:
	static const int MAX_BONE_INDICES = 4;

public:
	struct Node
	{
		uint64_t id = 0;
		std::string name;
		FbxNodeAttribute::EType	attribute = FbxNodeAttribute::EType::eUnknown;
		int64_t parent_index = -1;
		DirectX::XMFLOAT3 scaling = { 1,1,1 };
		DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
		DirectX::XMFLOAT3 translation = { 0,0,0 };
		template < class T>
		void serialize(T& archive)
		{
			archive(id, name, attribute, parent_index, scaling, rotation, translation);
		}
	};

	struct NodeKeyData
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 rotate;
		DirectX::XMFLOAT3 translate;
		template<class T>
		void serialize(T& archive)
		{
			archive(scale, rotate, translate);
		}
	};

	struct Keyframe
	{
		float seconds;
		std::vector<NodeKeyData> nodeKeys;

		template<class T>
		void serialize(T& archive)
		{
			archive(seconds, nodeKeys);
		}
	};

	struct Animation
	{
		std::string name;
		float seconds_length;
		float sampling_rate;
		float add_time;
		std::vector<Keyframe> keyframes;

		template<class T>
		void serialize(T& archive)
		{
			archive(name, seconds_length, sampling_rate, keyframes);
		}
	};

	struct SubSet
	{
		uint32_t start_index = 0;
		uint32_t index_count = 0;
	};

	struct Skin
	{
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};

private:
	std::shared_ptr<Node> meshNode = nullptr;
	std::vector<Node> nodes;
	std::vector<Skin> skinVert;		//1‚Â‚µ‚©‘Î‰ž‚µ‚Ä‚È‚¢
	std::vector<SubSet> subsets;	//1‚Â‚µ‚©‘Î‰ž‚µ‚Ä‚È‚¢
	std::vector<DirectX::XMMATRIX> offset_transforms;
	std::vector<uint64_t> node_indices;
	std::vector<Animation>	animation_clips;

private:
	void CheckBone();
	void FetchSkeleton(FbxMesh* fbx_mesh);
	void FetchAnimaton();
	uint64_t FindNodeIndex(uint64_t nodeId) const
	{
		int nodeCount = static_cast<int>(nodes.size());
		for (int i = 0; i < nodeCount; ++i)
		{
			if (nodes[i].id == nodeId)
			{
				return i;
			}
		}
		return -1;
	}

public:
	std::vector<Node>& GetNodes() { return nodes; }
	std::vector<Skin>& GetSkins() { return skinVert; }
	std::vector<DirectX::XMMATRIX>& GetOffsetTransforms() { return offset_transforms; }
	std::vector<uint64_t>& GetNodeIndices() { return node_indices; }
	std::vector<Animation>& GetAnimations() { return animation_clips; }
	void SetMeshNode(std::shared_ptr<Node> meshNode) { this->meshNode = meshNode; }
	void AddAnimation(const std::string& modelname);

private:
	static const std::string baseDirectory;

private:
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	static UINT descriptorHandleIncrementSize;

private:
	std::string name;
	std::vector<std::shared_ptr<FbxMeshes>> meshes;
	std::unordered_map<std::string, std::shared_ptr<FbxMaterial>> materials;
	std::shared_ptr<FbxMaterial> defaultMaterial = nullptr;
	DirectX::XMFLOAT3 size = { 0,0,0 };

	FbxManager* fbx_manager = nullptr;
	FbxImporter* fbx_importer = nullptr;
	FbxScene* fbxScene = nullptr;
	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();

	std::vector<std::string> names;
	std::string key;

public:
	static void StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device);
	static std::shared_ptr<FbxModels> CreateFromFbx(const std::string& modelname, bool smoothing = false);

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

	void Init(const std::string& modelname, bool smoothing = false);
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, std::vector<int> drawSkips);

private:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename, FbxSurfaceMaterial* fbx_material);
	void AddMaterial(std::shared_ptr<FbxMaterial> material);
	void CreateDescriptorHeap();
	void LoadTextures();

	void CreateMesh(FbxMesh* fbx_mesh);
	void LoadIndices(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh);
	void LoadVertices(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh);
	void LoadNormals(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh);
	void LoadUV(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh);
	void LoadColors(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh);
	void SetMaterialName(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh);

	std::string LoadName(FbxFileTexture* texture);

	void Replace(char search_char, char replace_char, char* buffer);
	void Split(char split_char, char* buffer, std::vector<std::string>& out);

public:
	inline const std::vector<std::shared_ptr<FbxMeshes>>& GetMeshes() { return meshes; }
	DirectX::XMFLOAT3 GetModelSize() { return size; }
	FbxScene* GetFbxScene() { return fbxScene; }
	DirectX::XMMATRIX& GetModelTransform() { return transform; }
};

