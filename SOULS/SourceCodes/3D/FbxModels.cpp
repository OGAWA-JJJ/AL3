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
#include "../2D/TexManager.h"
#include "../DirectX/DirectXImportant.h"

const std::string FbxModels::baseDirectory = "Resources/";
Microsoft::WRL::ComPtr<ID3D12Device> FbxModels::device = nullptr;
UINT FbxModels::descriptorHandleIncrementSize = 0;

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
	//fbx_manager->Destroy();
	//fbx_importer->Destroy();
	//fbxScene->Destroy();
}

void FbxModels::StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> dev)
{
	assert(!FbxModels::device);

	FbxModels::device = dev;
	FbxMeshes::StaticInit(dev);
}

std::shared_ptr<FbxModels> FbxModels::CreateFromFbx(
	const std::string& modelname, bool smoothing)
{
	std::shared_ptr<FbxModels> instance = std::make_shared<FbxModels>();
	instance->Init(modelname, smoothing);

	return instance;
}

//Load
void FbxModels::Init(const std::string& modelname, bool smoothing)
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
	//converter.RemoveBadPolygonsFromMeshes(fbxScene);

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

	int mesh_num = fbxScene->GetSrcObjectCount<FbxMesh>();
	for (int i = 0; i < mesh_num; i++)
	{
		CreateMesh(fbxScene->GetSrcObject<FbxMesh>(i));
	}

	//ボーン代入処理
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

	descriptorHandleIncrementSize =
		DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	LoadTextures();
}

void FbxModels::Draw(
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, std::vector<int> drawSkips)
{
	if (TexManager::GetHeapAdressSRV())
	{
		ID3D12DescriptorHeap* ppHeaps[] = { TexManager::GetHeapAdressSRV() };
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
	std::shared_ptr<FbxMaterial> l_material = FbxMaterial::Create();
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

void FbxModels::AddMaterial(std::shared_ptr<FbxMaterial> material)
{
	materials.emplace(material->GetName(), material);
}

void FbxModels::LoadTextures()
{
	std::string directoryPath = baseDirectory + name + "/";

	for (auto& m : materials) {
		std::weak_ptr<FbxMaterial> material = m.second;

		//テクスチャあり
		if (material.lock()->GetTextureFilename().size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				TexManager::GetCpuHeapStartSRV(),
				TexManager::GetOffsetSRV(),
				descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
				TexManager::GetGpuHeapStartSRV(),
				TexManager::GetOffsetSRV(),
				descriptorHandleIncrementSize);
			material.lock()->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
		}
		//テクスチャなし
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
				TexManager::GetCpuHeapStartSRV(),
				TexManager::GetOffsetSRV(),
				descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
				TexManager::GetGpuHeapStartSRV(),
				TexManager::GetOffsetSRV(),
				descriptorHandleIncrementSize);
			material.lock()->LoadTexture(baseDirectory, cpuDescHandleSRV, gpuDescHandleSRV);
		}
	}
}

void FbxModels::CreateMesh(FbxMesh* fbx_mesh)
{
	std::shared_ptr<FbxMeshes> mesh = std::make_shared<FbxMeshes>();
	LoadVertices(mesh, fbx_mesh);
	LoadNormals(mesh, fbx_mesh);
	LoadUV(mesh, fbx_mesh);			//Indices込み

	SetMaterialName(mesh, fbx_mesh);

	meshes.push_back(mesh);
}

void FbxModels::LoadIndices(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh)
{
	int polygon_count = mesh->GetPolygonCount();

	for (int i = 0; i < polygon_count; i++)
	{
		mesh_data->AddIndex(i * 3 + 2);
		mesh_data->AddIndex(i * 3 + 1);
		mesh_data->AddIndex(i * 3);
	}
}

void FbxModels::LoadVertices(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh)
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

void FbxModels::LoadNormals(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh)
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

void FbxModels::LoadUV(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh)
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
}

void FbxModels::SetMaterialName(std::shared_ptr<FbxMeshes> mesh_data, FbxMesh* mesh)
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
	if (fbxSkin == NULL) { return; }
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

		//アニメーションのフレームレートモード取得
		const FbxTime::EMode time_mode{ fbxScene->GetGlobalSettings().GetTimeMode() };

		//1フレーム辺りの加算量(FbxTimeでの)
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);

		//アニメーションのフレームレート算出
		animation_clip.sampling_rate = static_cast<float>(one_second.GetFrameRate(time_mode));

		//アニメーションの内部的な加算量??多分そんな重要じゃない
		const FbxTime sampling_interval =
			static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate);
		float fsamplng_interval = one_second.Get() / animation_clip.sampling_rate;

		//アニメーションの名前から色んな情報取得(多分)
		const FbxTakeInfo* take_info = fbxScene->GetTakeInfo(animation_clip.name.c_str());

		//それらを元に計算(FbxTimeでの)
		const FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
		const FbxTime stop_time = take_info->mLocalTimeSpan.GetStop();

		//それらを元に計算(floatでの)
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

void FbxModels::AddAnimation(const std::string& modelname)
{
	//読み込み
	const std::string filename = modelname + ".fbx";
	const std::string directoryPath = baseDirectory + modelname + "/";
	const std::string path = directoryPath + filename;

	name = modelname;

	fbx_importer->Initialize(path.c_str());
	fbx_importer->Import(fbxScene);

	//アニメーション抜き取り
	FbxArray<FbxString*> animation_stack_names;

	//ノード情報
	size_t node_count = nodes.size();
	std::vector<FbxNode*> fbxNodes;
	fbxNodes.resize(node_count);
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		fbxNodes[node_index] = fbxScene->FindNodeByName(nodes.at(node_index).name.c_str());
	}

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

		const FbxTime sampling_interval =
			static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate);
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

			keyframe.nodeKeys.resize(node_count);

			float f_time = time.GetFrameCount() / animation_clip.sampling_rate;
			keyframe.seconds = f_time - start;

			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				if (fbxNodes[node_index])
				{
					NodeKeyData& node = keyframe.nodeKeys.at(node_index);

					const FbxAMatrix& local_transform = fbxNodes[node_index]->EvaluateLocalTransform(time);
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
