#pragma once
#include "FbxMeshes.h"

class FbxModels
{
public:
	//Test
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
	std::vector<Node> nodes;
	Node* meshNode = nullptr;
	std::vector<Node>& GetNodes() { return nodes; }
	void SetMeshNode(Node* meshNode) { this->meshNode = meshNode; }

	static const int MAX_BONE_INDICES = 4;
	struct Skin
	{
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};
	std::vector<Skin> skinVert;
	std::vector<Skin>& GetSkins() { return skinVert; }

	void CheckBone();
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
	void FetchSkeleton(FbxMesh* fbx_mesh);

	struct SubSet
	{
		uint32_t start_index = 0;
		uint32_t index_count = 0;
	};
	std::vector<SubSet> subsets;

	struct Bone
	{
		std::string name;
		DirectX::XMMATRIX invInitialPose;
		FbxCluster* fbxCluster;
		Bone(const std::string& name) { this->name = name; }
	};
	//std::vector<Bone> bones;
	//std::vector<Bone>& GetBones() { return bones; }
	std::vector<DirectX::XMMATRIX> offset_transforms;
	std::vector<uint32_t> node_indices;
	std::vector<DirectX::XMMATRIX>& GetOffsetTransforms() { return offset_transforms; }
	std::vector<uint32_t>& GetNodeIndices() { return node_indices; }

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
	std::vector<Animation>	animation_clips;
	void FetchAnimaton();
	std::vector<Animation>& GetAnimations() { return animation_clips; }

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

	FbxManager* fbx_manager = nullptr;
	FbxImporter* fbx_importer = nullptr;
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

	void LoadNode(FbxMeshes* mesh_data, FbxNode* fbxNode, Node* parent = nullptr);
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
};

