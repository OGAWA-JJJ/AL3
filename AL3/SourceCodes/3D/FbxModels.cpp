#include "FbxModels.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <xlocbuf>
#include <codecvt> 
#include <DirectXTex.h>

const std::string FbxModels::baseDirectory = "Resources/";
ID3D12Device* FbxModels::device = nullptr;
UINT FbxModels::descriptorHandleIncrementSize = 0;

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

FbxModels* FbxModels::CreateFromFbx(const std::string& modelname, bool smoothing)
{
	FbxModels* instance = new FbxModels();
	//std::shared_ptr<Model> instance = std::make_shared<Model>();
	instance->Init(modelname, smoothing);

	return instance;
}

//Load
void FbxModels::Init(const std::string& modelname, bool smoothing)
{
	//Test-----
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
	converter.SplitMeshesPerMaterial(fbxScene, true);
	converter.Triangulate(fbxScene, true);



	//debug
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbxScene->GetSrcObject<FbxMesh>(0)->GetDeformer(0, FbxDeformer::eSkin));
	int clusterCount = fbxSkin->GetClusterCount();



	int mesh_num = fbxScene->GetSrcObjectCount<FbxMesh>();
	for (int i = 0; i < mesh_num; i++)
	{
		CreateMesh(fbxScene->GetSrcObject<FbxMesh>(i));
	}

	int material_num = fbxScene->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int i = 0; i < material_num; i++)
	{
		LoadMaterial(directoryPath, filename, fbxScene->GetSrcObject<FbxSurfaceMaterial>(i));
	}

	FbxNode* fbxNode = fbxScene->GetRootNode();

	//LoadNode(meshes[0], fbxNode);
	for (auto& m : meshes)
	{
		//fbxNodeが悪いのかGetMesh()が悪いのか...
		LoadNode(m, fbxNode);
	}
	/*for (int i = 0; i < mesh_num; i++)
	{
		LoadNode(fbxScene->GetSrcObject<FbxMesh>(i), meshes[i], fbxNode);
	}*/

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

	//for (auto& m : meshes)
	//{
	//	//1つのマテリアルしか入ってない
	//	if (m->GetMaterial() == nullptr)
	//	{
	//		if (defaultMaterial == nullptr)
	//		{
	//			defaultMaterial = FbxMaterial::Create();
	//			defaultMaterial->SetName("no material");
	//			materials.emplace(defaultMaterial->GetName(), defaultMaterial);
	//		}
	//		m->SetMaterial(defaultMaterial);
	//	}
	//}

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

void FbxModels::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (descHeap) {
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}

	//設計が不安、アルファが今は固定
	int count = 0;
	for (auto& mesh : meshes) {
		mesh->Draw(cmdList, materials[names[count]]);
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

	FbxDouble3 color = colors[(int)MaterialOrder::Ambient];
	FbxDouble factor = factors[(int)MaterialOrder::Ambient];
	entry_material.ambient = DirectX::XMFLOAT3((float)color[0], (float)color[1], (float)color[2]);

	color = colors[(int)MaterialOrder::Diffuse];
	factor = factors[(int)MaterialOrder::Diffuse];
	entry_material.diffuse = DirectX::XMFLOAT3((float)color[0], (float)color[1], (float)color[2]);

	color = colors[(int)MaterialOrder::Specular];
	factor = factors[(int)MaterialOrder::Specular];
	entry_material.specular = DirectX::XMFLOAT3((float)color[0], (float)color[1], (float)color[2]);

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

	//LoadIndices(mesh, fbx_mesh);
	LoadVertices(mesh, fbx_mesh);
	//LoadNormals(mesh, fbx_mesh);

	//IndicesとUVも一括で行う(仮)
	LoadUV(mesh, fbx_mesh);

	LoadColors(mesh, fbx_mesh);
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
	/*FbxVector4* vertices = mesh->GetControlPoints();
	int* indices = mesh->GetPolygonVertices();
	int polygon_vertex_count = mesh->GetPolygonVertexCount();

	for (int i = 0; i < polygon_vertex_count; i++)
	{
		FbxMeshes::VertexPosNormalUv vertex;
		int index = indices[i];

		vertex.pos.x = (float)-vertices[index][0];
		vertex.pos.y = (float)vertices[index][1];
		vertex.pos.z = (float)vertices[index][2];

		mesh_data->AddVertex(vertex);
	}*/

	//以前のLoader
	const int controlPointCount =
		mesh->GetControlPointsCount();
	FbxVector4* pCoord = mesh->GetControlPoints();
	for (int i = 0; i < controlPointCount; i++)
	{
		FbxMeshes::VertexPosNormalUv vertex;

		vertex.pos.x = (float)pCoord[i][0];
		vertex.pos.y = (float)pCoord[i][1];
		vertex.pos.z = (float)pCoord[i][2];

		mesh_data->AddVertex(vertex);
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
	/*FbxStringList uvset_names;
	mesh->GetUVSetNames(uvset_names);

	FbxArray<FbxVector2> uv_buffer;
	mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);

	for (int i = 0; i < uv_buffer.Size(); i++)
	{
		FbxVector2& uv = uv_buffer[i];

		mesh_data->GetVertices()[i].uv.x = (float)uv[0];
		mesh_data->GetVertices()[i].uv.y = (float)(1.0 - uv[1]);
	}*/

	//以前のLoader(インデックスとUV)
	const int polygonCount = mesh->GetPolygonCount();
	const int textureUVCount = mesh->GetTextureUVCount();

	FbxStringList uvNames;
	mesh->GetUVSetNames(uvNames);

	for (int i = 0; i < polygonCount; i++)
	{
		const int polygonSize = mesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		for (int j = 0; j < polygonSize; j++)
		{
			int index = mesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			FbxVector4 normal;
			if (mesh->GetPolygonVertexNormal(i, j, normal))
			{
				mesh_data->GetVertices()[index].normal.x = (float)-normal[0];
				mesh_data->GetVertices()[index].normal.y = (float)normal[1];
				mesh_data->GetVertices()[index].normal.z = (float)normal[2];
			}

			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;

				if (mesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					mesh_data->GetVertices()[index].uv.x = (float)uvs[0];
					mesh_data->GetVertices()[index].uv.y = (float)(1.0 - uvs[1]);
				}
			}

			if (j < 3)
			{
				mesh_data->AddIndex(index);
			}

			else
			{
				int index2 = mesh_data->GetIndices()[mesh_data->GetIndices().size() - 1];
				int index3 = index;
				int index0 = mesh_data->GetIndices()[mesh_data->GetIndices().size() - 3];
				mesh_data->AddIndex(index2);
				mesh_data->AddIndex(index3);
				mesh_data->AddIndex(index0);
			}
		}
	}
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
				mesh_data->GetVertices()[i].color.x = (float)color.mRed;
				mesh_data->GetVertices()[i].color.y = (float)color.mGreen;
				mesh_data->GetVertices()[i].color.z = (float)color.mBlue;
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

void FbxModels::LoadNode(FbxMeshes* mesh_data, FbxNode* fbxNode, FbxMeshes::Node* parent)
{
	std::string name = fbxNode->GetName();

	mesh_data->GetNodes().emplace_back();
	FbxMeshes::Node& node = mesh_data->GetNodes().back();

	node.name = fbxNode->GetName();

	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
	node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

	node.rotation.m128_f32[0] = DirectX::XMConvertToRadians(node.rotation.m128_f32[0]);
	node.rotation.m128_f32[1] = DirectX::XMConvertToRadians(node.rotation.m128_f32[1]);
	node.rotation.m128_f32[2] = DirectX::XMConvertToRadians(node.rotation.m128_f32[2]);

	DirectX::XMMATRIX matScaling, matRotation, matTranslaton;
	matScaling = DirectX::XMMatrixScalingFromVector(node.scaling);
	matRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTranslaton = DirectX::XMMatrixTranslationFromVector(node.translation);

	node.transform = DirectX::XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotation;
	node.transform *= matTranslaton;

	node.globalTransform = node.transform;
	if (parent)
	{
		node.parent = parent;
		node.globalTransform *= parent->globalTransform;
	}

	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			mesh_data->SetMeshNode(&node);
			transform = node.globalTransform;
			FbxMesh* fbx_mesh = fbxNode->GetMesh();
			//FbxMesh* fbx_mesh = fbxScene->GetSrcObject<FbxMesh>(i);
			LoadSkin(mesh_data, fbx_mesh);
		}
	}

	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		LoadNode(mesh_data, fbxNode->GetChild(i), &node);
	}
}

void FbxModels::LoadSkin(FbxMeshes* mesh_data, FbxMesh* mesh)
{
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));

	std::vector<FbxMeshes::Skin> skinVert = mesh_data->GetSkins();

	if (fbxSkin == nullptr)
	{
		mesh_data->GetSkins().resize(mesh_data->GetVertexCount());
		for (int i = 0; i < mesh_data->GetVertexCount(); i++)
		{
			skinVert[i].boneIndex[0] = 0;
			skinVert[i].boneWeight[0] = 1.0f;
		}
		return;
	}

	std::vector<FbxMeshes::Bone>& bones = mesh_data->GetBones();

	//受け取ったメッシュのClusterCountが欲しい、28になるのが変
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		const char* boneName = fbxCluster->GetLink()->GetName();

		bones.emplace_back(FbxMeshes::Bone(boneName));
		FbxMeshes::Bone& bone = bones.back();

		bone.fbxCluster = fbxCluster;

		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		DirectX::XMMATRIX initialPose;
		ConvertMatrixFromFbx(&initialPose, fbxMat);

		bone.invInitialPose = DirectX::XMMatrixInverse(nullptr, initialPose);
	}

	struct  WeightSet
	{
		UINT index;
		float weight;
	};

	std::vector<std::list<WeightSet>>
		weightLists(mesh_data->GetVertexCount());

	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			int vertIndex = controlPointIndices[j];

			float weight = (float)controlPointWeights[j];

			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}

		auto& vertices = mesh_data->GetSkins();
		vertices.resize(mesh_data->GetVertexCount());

		for (int i = 0; i < vertices.size(); i++)
		{
			auto& weightList = weightLists[i];

			weightList.sort(
				[](auto const& lhs, auto const& rhs)
				{
					return lhs.weight > rhs.weight;
				});

			int weightArrayIndex = 0;

			for (auto& weightSet : weightList)
			{
				vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
				vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;

				if (++weightArrayIndex >= FbxMeshes::MAX_BONE_INDICES)
				{
					float weight = 0.0f;

					for (int j = 1; j < FbxMeshes::MAX_BONE_INDICES; j++)
					{
						weight += vertices[i].boneWeight[j];
					}

					vertices[i].boneWeight[0] = 1.0f - weight;
					break;
				}
			}
		}
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
