#include "FbxModels.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <xlocbuf>
#include <codecvt> 
#include "include/DirectXTex.h"
#include <filesystem>

#include "../Math/OgaJHelper.h"

const std::string FbxModels::baseDirectory = "Resources/";
ID3D12Device* FbxModels::device = nullptr;
UINT FbxModels::descriptorHandleIncrementSize = 0;

std::unordered_map<std::string, FbxModels*> FbxModels::models;

inline DirectX::XMFLOAT3 ToXMFLOAT3(const FbxDouble3& fbxdouble3)
{
	DirectX::XMFLOAT3 xmfloat3{};
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline DirectX::XMFLOAT4 ToXMFLOAT4(const FbxDouble4& fbxdouble4)
{
	DirectX::XMFLOAT4 xmfloat4{};
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}

struct BoneInfluence
{
	uint32_t bone_index{ 0 };
	float bone_weight{ 1.0f };
};
using BoneInfluencesPerControlPoint = std::vector<BoneInfluence>;
void FetchBoneInfluences(const FbxMesh* fbx_mesh, std::vector<BoneInfluencesPerControlPoint>& bone_influences)
{
	const int control_points_count = fbx_mesh->GetControlPointsCount();
	bone_influences.resize(control_points_count);

	const int skin_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin));
		const int cluster_count = fbx_skin->GetClusterCount();

		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster = fbx_skin->GetCluster(cluster_index);

			const int control_point_indices_count = fbx_cluster->GetControlPointIndicesCount();

			for (int control_point_indices_index = 0;
				control_point_indices_index < control_point_indices_count;
				++control_point_indices_index)
			{
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight = fbx_cluster->GetControlPointWeights()[control_point_indices_index];
				bone_influences.at(control_point_index).emplace_back();
				BoneInfluence& bone_influence = bone_influences.at(control_point_index).back();
				bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
				bone_influence.bone_weight = static_cast<float>(control_point_weight);
			}
		}
	}
}

FbxModels::FbxModels()
{
}

FbxModels::~FbxModels()
{
	for (auto m : meshes) {
		delete m;
	}
	meshes.clear();

	for (auto m : materials) {
		delete m.second;
	}
	materials.clear();

	fbx_manager->Destroy();
	fbx_importer->Destroy();
	fbxScene->Destroy();
}

void FbxModels::StaticInit(ID3D12Device* dev)
{
	assert(!FbxModels::device);

	FbxModels::device = dev;
	FbxMeshes::StaticInit(dev);
}

FbxModels* FbxModels::CreateFromFbx(const std::string& modelname, const std::string& key, bool smoothing)
{
	FbxModels* instance = new FbxModels();
	//std::shared_ptr<Model> instance = std::make_shared<Model>();
	instance->Init(modelname, key, smoothing);
	if (key.size() > 0)
	{
		models.emplace(key, instance);
	}

	return instance;
}

//Load
void FbxModels::Init(const std::string& modelname, const std::string& key, bool smoothing)
{
	const std::string filename = modelname + ".fbx";
	const std::string directoryPath = baseDirectory + modelname + "/";
	const std::string path = directoryPath + filename;

	name = modelname;

	fbx_manager = fbxsdk::FbxManager::Create();
	assert(fbx_manager != nullptr);

	fbx_importer = FbxImporter::Create(fbx_manager, "");
	if (fbx_importer == nullptr)
	{
		fbx_manager->Destroy();
		assert(0);
	}

	fbxScene = FbxScene::Create(fbx_manager, "");
	if (fbxScene == nullptr)
	{
		fbx_importer->Destroy();
		fbx_manager->Destroy();
		assert(0);
	}

	fbx_importer->Initialize(path.c_str());

	fbx_importer->Import(fbxScene);

	FbxGeometryConverter converter(fbx_manager);
	converter.Triangulate(fbxScene, true);
	converter.RemoveBadPolygonsFromMeshes(fbxScene);

#pragma region Test
	//ノードの取得
	std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node)
	{
		nodes.emplace_back();
		Node& node = nodes.back();

		node.attribute = fbx_node->GetNodeAttribute() ?
			fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
		node.name = fbx_node->GetName();
		node.id = fbx_node->GetUniqueID();

		node.parent_index = FindNodeIndex(fbx_node->GetParent() ?
			fbx_node->GetParent()->GetUniqueID() : 0);

		node.scaling = ToXMFLOAT3(fbx_node->EvaluateLocalTransform().GetS());
		node.rotation = ToXMFLOAT4(fbx_node->EvaluateLocalTransform().GetQ());
		node.translation = ToXMFLOAT3(fbx_node->EvaluateLocalTransform().GetT());

		for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
		{
			traverse(fbx_node->GetChild(child_index));
		}
	} };
	traverse(fbxScene->GetRootNode());

	//ボーンとか
	CheckBone();
#pragma endregion

	converter.SplitMeshesPerMaterial(fbxScene, true);

	for (const Node& node : nodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		FbxNode* fbx_node = fbxScene->FindNodeByName(node.name.c_str());
		FbxMesh* fbx_mesh = fbx_node->GetMesh();
		FetchSkeleton(fbx_mesh);
	}

	//見つかった場合
	bool isFind = false;
	if (models.find(key) != models.end())
	{
		this->key = key;
		isFind = true;
	}

	if (!isFind)
	{
		int mesh_num = fbxScene->GetSrcObjectCount<FbxMesh>();
		for (int i = 0; i < mesh_num; i++)
		{
			CreateMesh(fbxScene->GetSrcObject<FbxMesh>(i));
		}
	}
	else
	{
		int mesh_num = fbxScene->GetSrcObjectCount<FbxMesh>();
		for (int i = 0; i < mesh_num; i++)
		{
			FindKeyCreateMesh(fbxScene->GetSrcObject<FbxMesh>(i), i);
		}
	}

	//ボーン代入処理
	if (!isFind)
	{
		int l_num = 0;
		int l_counts = 0;
		for (int size = 1; size < subsets.size(); size++)
		{
			int max_Index = subsets[size].start_index - l_counts;
			for (int index = 0; index < max_Index; index++)
			{
				for (int bone = 0; bone < MAX_BONE_INDICES; bone++)
				{
					meshes[l_num]->GetVertices()[index].boneIndex[bone] =
						skinVert[l_counts].boneIndex[bone];
					meshes[l_num]->GetVertices()[index].boneWeight[bone] =
						skinVert[l_counts].boneWeight[bone];
				}
				l_counts++;
			}
			l_num++;
		}

		int last_Index = static_cast<int>(skinVert.size()) - l_counts;
		l_num = static_cast<int>(meshes.size()) - 1;
		for (int index = 0; index < last_Index; index++)
		{
			for (int bone = 0; bone < MAX_BONE_INDICES; bone++)
			{
				meshes[l_num]->GetVertices()[index].boneIndex[bone] =
					skinVert[l_counts].boneIndex[bone];
				meshes[l_num]->GetVertices()[index].boneWeight[bone] =
					skinVert[l_counts].boneWeight[bone];
			}
			l_counts++;
		}
	}

	int material_num = fbxScene->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int i = 0; i < material_num; i++)
	{
		LoadMaterial(directoryPath, filename, fbxScene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	int texture_num = fbxScene->GetSrcObjectCount<FbxFileTexture>();
	for (int i = 0; i < texture_num; i++)
	{
		FbxFileTexture* texture = fbxScene->GetSrcObject<FbxFileTexture>(i);
		if (texture)
		{
			const char* file_name01 = texture->GetFileName();
			const char* file_name02 = texture->GetRelativeFileName();
			int tex = texture->GetSrcObjectCount< FbxSurfaceMaterial>();
		}
	}

	for (auto& m : meshes)
	{
		m->CreateBuffers();
	}

	for (auto& m : materials) {
		m.second->Update();
	}

	CreateDescriptorHeap();

	LoadTextures();
}

void FbxModels::Draw(ID3D12GraphicsCommandList* cmdList, std::vector<int> drawSkips)
{
	if (descHeap) {
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}

	//設計が不安、アルファが今は固定
	int count = 0;
	for (auto& mesh : meshes)
	{
		if (!OgaJHelper::VectorFinder(drawSkips, count))
		{
			mesh->Draw(cmdList, materials[names[count]]);
		}
		count++;
	}
}

//Load
void FbxModels::LoadMaterial(const std::string& directoryPath, const std::string& filename, FbxSurfaceMaterial* fbx_material)
{
	//Test-----
	FbxMaterial* l_material = FbxMaterial::Create();
	l_material->SetName(fbx_material->GetName());
	FbxMaterial::ConstBufferDataB1 entry_material;

	enum class MaterialOrder
	{
		Ambient,
		Diffuse,
		Specular,
		MaxOrder,
	};

	FbxDouble3 colors[(int)MaterialOrder::MaxOrder];
	FbxDouble factors[(int)MaterialOrder::MaxOrder];
	FbxProperty prop = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);

	if (fbx_material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		const char* element_check_list[] =
		{
			FbxSurfaceMaterial::sAmbient,
			FbxSurfaceMaterial::sDiffuse,
		};

		const char* factor_check_list[] =
		{
			FbxSurfaceMaterial::sAmbientFactor,
			FbxSurfaceMaterial::sDiffuseFactor,
		};

		for (int i = 0; i < 2; i++)
		{
			prop = fbx_material->FindProperty(element_check_list[i]);
			if (prop.IsValid()) { colors[i] = prop.Get<FbxDouble3>(); }
			else { colors[i] = FbxDouble3(1.0, 1.0, 1.0); }

			prop = fbx_material->FindProperty(factor_check_list[i]);
			if (prop.IsValid()) { factors[i] = prop.Get<FbxDouble>(); }
			else { factors[i] = 1.0; }
		}
	}

	entry_material.ambient = l_material->ambient;
	entry_material.diffuse = l_material->diffuse;
	entry_material.specular = l_material->specular;
	entry_material.color = l_material->color;

	l_material->SetMaterial(entry_material);

	//名前とかテクスチャ取得の処理がない↓
	prop = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxFileTexture* texture = nullptr;
	int texture_num = prop.GetSrcObjectCount<FbxFileTexture>();
	if (texture_num > 0)
	{
		texture = prop.GetSrcObject<FbxFileTexture>(0);
	}
	else
	{
		int layer_num = prop.GetSrcObjectCount<FbxLayeredTexture>();
		if (layer_num > 0)
		{
			texture = prop.GetSrcObject<FbxFileTexture>(0);
		}
	}
	std::string name = LoadName(texture);
	if (name.size() > 0)
	{
		l_material->SetTextureFileName(name);
	}

	if (l_material)
	{
		names.push_back(l_material->GetName());
		AddMaterial(l_material);
	}
}

void FbxModels::AddMaterial(FbxMaterial* material)
{
	materials.emplace(material->GetName(), material);
}

void FbxModels::CreateDescriptorHeap()
{
	HRESULT result = S_FALSE;

	//マテリアルの数
	size_t count = materials.size();

	//デスクリプタヒープ生成(1つにまとめた方が楽！0~100が定数バッファ,101~200がSRV...)←cmdListを呼ぶ回数が減る
	if (count > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descHeapDesc.NumDescriptors = (UINT)count;
		result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
		if (FAILED(result)) {
			assert(0);
		}
	}

	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void FbxModels::LoadTextures()
{
	int textureIndex = 0;
	std::string directoryPath = baseDirectory + name + "/";

	for (auto& m : materials) {
		FbxMaterial* material = m.second;

		//テクスチャあり
		if (material->GetTextureFilename().size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeap->GetCPUDescriptorHandleForHeapStart(),
				textureIndex,
				descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeap->GetGPUDescriptorHandleForHeapStart(),
				textureIndex,
				descriptorHandleIncrementSize);
			material->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
			textureIndex++;
		}
		//テクスチャなし
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeap->GetCPUDescriptorHandleForHeapStart(),
				textureIndex,
				descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeap->GetGPUDescriptorHandleForHeapStart(),
				textureIndex,
				descriptorHandleIncrementSize);
			material->LoadTexture(baseDirectory, cpuDescHandleSRV, gpuDescHandleSRV);
			textureIndex++;
		}
	}
}

void FbxModels::CreateMesh(FbxMesh* fbx_mesh)
{
	FbxMeshes* mesh = new FbxMeshes;
	LoadVertices(mesh, fbx_mesh);
	LoadNormals(mesh, fbx_mesh);
	LoadUV(mesh, fbx_mesh);			//Indices込み

	SetMaterialName(mesh, fbx_mesh);

	meshes.push_back(mesh);
}

void FbxModels::FindKeyCreateMesh(FbxMesh* fbx_mesh, int mesh_num)
{
	FbxMeshes* mesh = new FbxMeshes;

	mesh->SetVertices(models.at(key)->GetMeshes()[mesh_num]->GetVertices());
	mesh->SetIndices(models.at(key)->GetMeshes()[mesh_num]->GetIndices());

	SetMaterialName(mesh, fbx_mesh);

	meshes.push_back(mesh);
}

void FbxModels::LoadIndices(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	int polygon_count = mesh->GetPolygonCount();

	for (int i = 0; i < polygon_count; i++)
	{
		mesh_data->AddIndex(i * 3 + 2);
		mesh_data->AddIndex(i * 3 + 1);
		mesh_data->AddIndex(i * 3);
	}
}

void FbxModels::LoadVertices(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	//New
	const int polygonCount = mesh->GetPolygonCount();
	const FbxVector4* controlPoints = mesh->GetControlPoints();
	const int materialCount = mesh->GetNode()->GetMaterialCount();

	mesh_data->GetIndices().resize(polygonCount * 3LL);

	for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		const int material_index =
			materialCount > 0 ?
			mesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;

		for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
		{
			FbxMeshes::VertexPosNormalUv vertex;
			const int polygonVertex = mesh->GetPolygonVertex(polygonIndex, positionInPolygon);
			vertex.pos.x = static_cast<float>(controlPoints[polygonVertex][0]);
			vertex.pos.y = static_cast<float>(controlPoints[polygonVertex][1]);
			vertex.pos.z = static_cast<float>(controlPoints[polygonVertex][2]);
			mesh_data->AddVertex(vertex);

			const int vertex_index = polygonIndex * 3 + positionInPolygon;
			mesh_data->GetIndices()[vertex_index] = vertex_index;
		}

		subsets[material_index].index_count++;
	}
}

void FbxModels::LoadNormals(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	FbxArray<FbxVector4> normals;
	mesh->GetPolygonVertexNormals(normals);

	for (int i = 0; i < normals.Size(); i++)
	{
		mesh_data->GetVertices()[i].normal.x = (float)-normals[i][0];
		mesh_data->GetVertices()[i].normal.y = (float)normals[i][1];
		mesh_data->GetVertices()[i].normal.z = (float)normals[i][2];
	}
}

void FbxModels::LoadUV(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	FbxStringList uvset_names;
	mesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;
	mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		FbxVector2& uv = uv_buffer[i];

		mesh_data->GetVertices()[i].uv.x = (float)uv[0];
		mesh_data->GetVertices()[i].uv.y = (float)(1.0 - uv[1]);
	}

	////以前のLoader(インデックスとUV)
	//const int polygonCount = mesh->GetPolygonCount();
	//const int textureUVCount = mesh->GetTextureUVCount();

	//FbxStringList uvNames;
	//mesh->GetUVSetNames(uvNames);

	//for (int i = 0; i < polygonCount; i++)
	//{
	//	const int polygonSize = mesh->GetPolygonSize(i);
	//	assert(polygonSize <= 4);

	//	for (int j = 0; j < polygonSize; j++)
	//	{
	//		int index = mesh->GetPolygonVertex(i, j);
	//		assert(index >= 0);

	//		FbxVector4 normal;
	//		if (mesh->GetPolygonVertexNormal(i, j, normal))
	//		{
	//			mesh_data->GetVertices()[index].normal.x = (float)-normal[0];
	//			mesh_data->GetVertices()[index].normal.y = (float)normal[1];
	//			mesh_data->GetVertices()[index].normal.z = (float)normal[2];
	//		}

	//		if (textureUVCount > 0)
	//		{
	//			FbxVector2 uvs;
	//			bool lUnmappedUV;

	//			if (mesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
	//			{
	//				mesh_data->GetVertices()[index].uv.x = (float)uvs[0];
	//				mesh_data->GetVertices()[index].uv.y = (float)(1.0 - uvs[1]);
	//			}
	//		}

	//		if (j < 3)
	//		{
	//			mesh_data->AddIndex(index);
	//		}

	//		else
	//		{
	//			int index2 = mesh_data->GetIndices()[mesh_data->GetIndices().size() - 1];
	//			int index3 = index;
	//			int index0 = mesh_data->GetIndices()[mesh_data->GetIndices().size() - 3];
	//			mesh_data->AddIndex(index2);
	//			mesh_data->AddIndex(index3);
	//			mesh_data->AddIndex(index0);
	//		}
	//	}
	//}

	//New
	/*const int polygonCount = mesh->GetPolygonCount();
	FbxStringList uv_names;
	mesh->GetUVSetNames(uv_names);
	for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
		{
			if (mesh->GetElementNormalCount() > 0)
			{
				FbxVector4 normal;
				mesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
				mesh_data->GetVertices()[polygonIndex].normal.x = static_cast<float>(normal[0]);
				mesh_data->GetVertices()[polygonIndex].normal.y = static_cast<float>(normal[1]);
				mesh_data->GetVertices()[polygonIndex].normal.z = static_cast<float>(normal[2]);
			}

			if (mesh->GetElementUVCount() > 0)
			{
				FbxVector2 uv;
				bool unmapped_uv;
				mesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uv_names[0], uv, unmapped_uv);
				mesh_data->GetVertices()[polygonIndex].uv.x = static_cast<float>(uv[0]);
				mesh_data->GetVertices()[polygonIndex].uv.y = 1.0f - static_cast<float>(uv[1]);
			}
		}
	}*/
}

//読み込んでるけど今送ってない
void FbxModels::LoadColors(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	int color_count = mesh->GetElementVertexColorCount();
	if (color_count == 0) { return; }

	FbxGeometryElementVertexColor* vertex_colors = mesh->GetElementVertexColor(0);
	if (vertex_colors == nullptr) { return; }

	FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
	FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();
	if (mapping_mode == FbxLayerElement::eByPolygonVertex)
	{
		if (reference_mode == FbxLayerElement::eIndexToDirect)
		{
			FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
			FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
			for (int i = 0; i < indeces.GetCount(); i++)
			{
				int id = indeces.GetAt(i);
				FbxColor color = colors.GetAt(id);
				//mesh_data->GetVertices()[i].color.x = (float)color.mRed;
				//mesh_data->GetVertices()[i].color.y = (float)color.mGreen;
				//mesh_data->GetVertices()[i].color.z = (float)color.mBlue;
				//mesh_data->GetVertices()[i].color.w = (float)color.mAlpha;
			}
		}
	}
}

void FbxModels::SetMaterialName(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	if (mesh->GetElementMaterialCount() == 0)
	{
		mesh_data->SetName("");
		return;
	}

	FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
	int index = material->GetIndexArray().GetAt(0);
	FbxSurfaceMaterial* surface_material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);
	if (surface_material != nullptr)
	{
		mesh_data->SetName(surface_material->GetName());
	}
	else
	{
		mesh_data->SetName("");
	}
}

//コメントアウト
void FbxModels::LoadNode(FbxMeshes* mesh_data, FbxNode* fbxNode, Node* parent)
{
	//std::string name = fbxNode->GetName();

	//nodes.emplace_back();
	//Node& node = nodes.back();

	//node.name = fbxNode->GetName();

	//FbxDouble3 rotation = fbxNode->LclRotation.Get();
	//FbxDouble3 scaling = fbxNode->LclScaling.Get();
	//FbxDouble3 translation = fbxNode->LclTranslation.Get();

	//node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	//node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
	//node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

	//node.rotation.m128_f32[0] = DirectX::XMConvertToRadians(node.rotation.m128_f32[0]);
	//node.rotation.m128_f32[1] = DirectX::XMConvertToRadians(node.rotation.m128_f32[1]);
	//node.rotation.m128_f32[2] = DirectX::XMConvertToRadians(node.rotation.m128_f32[2]);

	//DirectX::XMMATRIX matScaling, matRotation, matTranslaton;
	//matScaling = DirectX::XMMatrixScalingFromVector(node.scaling);
	//matRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(node.rotation);
	//matTranslaton = DirectX::XMMatrixTranslationFromVector(node.translation);

	//node.transform = DirectX::XMMatrixIdentity();
	//node.transform *= matScaling;
	//node.transform *= matRotation;
	//node.transform *= matTranslaton;

	//node.globalTransform = node.transform;
	//if (parent)
	//{
	//	node.parent = parent;
	//	node.globalTransform *= parent->globalTransform;
	//}

	//FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	//if (fbxNodeAttribute)
	//{
	//	if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	//	{
	//		mesh_data->SetMeshNode(&node);
	//		transform = node.globalTransform;
	//		FbxMesh* fbx_mesh = fbxNode->GetMesh();
	//		//FbxMesh* fbx_mesh = fbxScene->GetSrcObject<FbxMesh>(i);
	//		LoadSkin(mesh_data, fbx_mesh);
	//	}
	//}

	//for (int i = 0; i < fbxNode->GetChildCount(); i++)
	//{
	//	LoadNode(mesh_data, fbxNode->GetChild(i), &node);
	//}
}
void FbxModels::LoadSkin(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	//FbxSkin* fbxSkin =
	//	static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));

	//std::vector<FbxMeshes::Skin> skinVert = mesh_data->GetSkins();

	//if (fbxSkin == nullptr)
	//{
	//	mesh_data->GetSkins().resize(mesh_data->GetVertexCount());
	//	for (int i = 0; i < mesh_data->GetVertexCount(); i++)
	//	{
	//		skinVert[i].boneIndex[0] = 0;
	//		skinVert[i].boneWeight[0] = 1.0f;
	//	}
	//	return;
	//}

	//std::vector<FbxMeshes::Bone>& bones = mesh_data->GetBones();

	////受け取ったメッシュのClusterCountが欲しい、28になるのが変
	//int clusterCount = fbxSkin->GetClusterCount();
	//bones.reserve(clusterCount);

	//for (int i = 0; i < clusterCount; i++)
	//{
	//	FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

	//	const char* boneName = fbxCluster->GetLink()->GetName();

	//	bones.emplace_back(FbxMeshes::Bone(boneName));
	//	FbxMeshes::Bone& bone = bones.back();

	//	bone.fbxCluster = fbxCluster;

	//	FbxAMatrix fbxMat;
	//	fbxCluster->GetTransformLinkMatrix(fbxMat);

	//	DirectX::XMMATRIX initialPose;
	//	ConvertMatrixFromFbx(&initialPose, fbxMat);

	//	bone.invInitialPose = DirectX::XMMatrixInverse(nullptr, initialPose);
	//}

	//struct  WeightSet
	//{
	//	UINT index;
	//	float weight;
	//};

	//std::vector<std::list<WeightSet>>
	//	weightLists(mesh_data->GetVertexCount());

	//for (int i = 0; i < clusterCount; i++)
	//{
	//	FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

	//	int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

	//	int* controlPointIndices = fbxCluster->GetControlPointIndices();
	//	double* controlPointWeights = fbxCluster->GetControlPointWeights();

	//	for (int j = 0; j < controlPointIndicesCount; j++)
	//	{
	//		int vertIndex = controlPointIndices[j];

	//		float weight = (float)controlPointWeights[j];

	//		weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
	//	}

	//	auto& vertices = mesh_data->GetSkins();
	//	vertices.resize(mesh_data->GetVertexCount());

	//	for (int i = 0; i < vertices.size(); i++)
	//	{
	//		auto& weightList = weightLists[i];

	//		weightList.sort(
	//			[](auto const& lhs, auto const& rhs)
	//			{
	//				return lhs.weight > rhs.weight;
	//			});

	//		int weightArrayIndex = 0;

	//		for (auto& weightSet : weightList)
	//		{
	//			vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
	//			vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;

	//			if (++weightArrayIndex >= FbxMeshes::MAX_BONE_INDICES)
	//			{
	//				float weight = 0.0f;

	//				for (int j = 1; j < FbxMeshes::MAX_BONE_INDICES; j++)
	//				{
	//					weight += vertices[i].boneWeight[j];
	//				}

	//				vertices[i].boneWeight[0] = 1.0f - weight;
	//				break;
	//			}
	//		}
	//	}
	//}
}

std::string FbxModels::LoadName(FbxFileTexture* texture)
{
	//テクスチャを読み込んで各meshのtextureFileNameに持たせる
	std::string str;
	if (texture == nullptr) { return str; }

	std::string file_path = texture->GetRelativeFileName();

	char buffer[256];
	ZeroMemory(buffer, sizeof(char) * 256);
	memcpy(buffer, file_path.c_str(), sizeof(char) * 256);

	Replace('\\', '/', buffer);
	std::vector<std::string> split_list;
	std::string replace_file_name = buffer;
	Split('/', buffer, split_list);

	str = buffer;
	std::filesystem::path l_path(str);
	l_path.replace_extension(".dds");
	str = l_path.string();

	return str;
}

void FbxModels::Replace(char search_char, char replace_char, char* buffer)
{
	int len = (int)strlen(buffer);

	for (int i = 0; i < len; i++)
	{
		if (buffer[i] == search_char)
		{
			buffer[i] = replace_char;
		}
	}
}

void FbxModels::Split(char split_char, char* buffer, std::vector<std::string>& out)
{
	int count = 0;
	if (buffer == nullptr)
	{
		return;
	}

	int start_point = 0;

	while (buffer[count] != '\0')
	{
		if (buffer[count] == split_char)
		{
			if (start_point != count)
			{
				char split_str[256] = { 0 };
				strncpy_s(split_str, 256, &buffer[start_point], count - start_point);
				out.emplace_back(split_str);
			}
			else
			{
				out.emplace_back("");
			}
			start_point = count + 1;
		}
		count++;
	}

	if (start_point != count)
	{
		char split_str[256] = { 0 };
		strncpy_s(split_str, 256, &buffer[start_point], count - start_point);
		out.emplace_back(split_str);
	}
}

void FbxModels::CheckBone()
{
	for (const Node& node : nodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		FbxNode* fbx_node = fbxScene->FindNodeByName(node.name.c_str());
		FbxMesh* fbx_mesh = fbx_node->GetMesh();

		std::vector<BoneInfluencesPerControlPoint> bone_influences;
		FetchBoneInfluences(fbx_mesh, bone_influences);
		//FetchSkeleton(fbx_mesh);
		FetchAnimaton();

		const int material_count = fbx_mesh->GetNode()->GetMaterialCount();
		subsets.resize(material_count > 0 ? material_count : 1);

		const int polygon_count = fbx_mesh->GetPolygonCount();

		if (material_count > 0)
		{
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				const int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index);
				subsets.at(material_index).index_count += 3;
			}
			uint32_t offset = 0;
			for (int i = 0; i < subsets.size(); i++)
			{
				subsets[i].start_index = offset;
				offset += subsets[i].index_count;
				subsets[i].index_count = 0;
			}
		}

		skinVert.resize(polygon_count * 3LL);
		int l_count = 0;

		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
		{
			const int material_index =
				material_count > 0 ?
				fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0;
			//subsets[material_index].index_count++;

			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
			{
				const int polygon_vertex = fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon);
				const BoneInfluencesPerControlPoint& influences_per_control_point =
					bone_influences.at(polygon_vertex);

				for (size_t influence_index = 0;
					influence_index < influences_per_control_point.size();
					++influence_index)
				{
					if (influence_index < 4)
					{
						const int index = l_count;
						skinVert[index].boneWeight[influence_index] =
							influences_per_control_point.at(influence_index).bone_weight;
						skinVert[index].boneIndex[influence_index] =
							influences_per_control_point.at(influence_index).bone_index;
					}
				}

				l_count++;
			}
		}
	}
}

void FbxModels::FetchSkeleton(FbxMesh* fbx_mesh)
{
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbx_mesh->GetDeformer(0, FbxDeformer::eSkin));
	int clusterCount = fbxSkin->GetClusterCount();
	node_indices.resize(clusterCount);
	offset_transforms.resize(clusterCount);

	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		uint64_t nodeId = static_cast<uint64_t>(fbxCluster->GetLink()->GetUniqueID());
		node_indices.at(i) = FindNodeIndex(nodeId);

		const char* boneName = fbxCluster->GetLink()->GetName();

		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		DirectX::XMMATRIX initialPose;
		ConvertMatrixFromFbx(&initialPose, fbxMat);

		FbxAMatrix reference_gloval_init_position;
		fbxCluster->GetTransformMatrix(reference_gloval_init_position);

		DirectX::XMMATRIX l_mat = DirectX::XMMatrixIdentity();
		ConvertMatrixFromFbx(&l_mat, fbxMat.Inverse() * reference_gloval_init_position);
		offset_transforms.at(i) = l_mat;
	}
}

void FbxModels::FetchAnimaton()
{
	FbxArray<FbxString*> animation_stack_names;

	fbxScene->FillAnimStackNameArray(animation_stack_names);
	const int animation_stack_count = animation_stack_names.GetCount();

	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		animation_clips.emplace_back();
		Animation& animation_clip = animation_clips.back();
		animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

		FbxAnimStack* animation_stack = fbxScene->FindMember<FbxAnimStack>(animation_clip.name.c_str());
		fbxScene->SetCurrentAnimationStack(animation_stack);

		const FbxTime::EMode time_mode{ fbxScene->GetGlobalSettings().GetTimeMode() };
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);
		animation_clip.sampling_rate = static_cast<float>(one_second.GetFrameRate(time_mode));

		const FbxTime sampling_interval = static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate);
		float fsamplng_interval = one_second.Get() / animation_clip.sampling_rate;
		const FbxTakeInfo* take_info = fbxScene->GetTakeInfo(animation_clip.name.c_str());
		const FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
		const FbxTime stop_time = take_info->mLocalTimeSpan.GetStop();

		float stop = stop_time.GetFrameCount() / animation_clip.sampling_rate;
		float start = start_time.GetFrameCount() / animation_clip.sampling_rate;
		animation_clip.seconds_length = stop - start;

		animation_clip.add_time = stop / stop_time.GetFrameCount(FbxTime::EMode::eFrames60);

		for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
		{
			animation_clip.keyframes.emplace_back();
			Keyframe& keyframe = animation_clip.keyframes.back();

			const size_t node_count = nodes.size();
			keyframe.nodeKeys.resize(node_count);

			float f_time = time.GetFrameCount() / animation_clip.sampling_rate;
			keyframe.seconds = f_time - start;

			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				FbxNode* fbx_node = fbxScene->FindNodeByName(nodes.at(node_index).name.c_str());
				if (fbx_node)
				{
					NodeKeyData& node = keyframe.nodeKeys.at(node_index);

					const FbxAMatrix& local_transform = fbx_node->EvaluateLocalTransform(time);
					node.scale = ToXMFLOAT3(local_transform.GetS());
					node.rotate = ToXMFLOAT4(local_transform.GetQ());
					node.translate = ToXMFLOAT3(local_transform.GetT());
				}
			}
		}
	}
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		delete animation_stack_names[animation_stack_index];
	}
}
