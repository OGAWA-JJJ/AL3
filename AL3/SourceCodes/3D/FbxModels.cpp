//#include "FbxModels.h"
//#include <fstream>
//#include <sstream>
//#include <algorithm>
//#include <cassert>
//
//FbxModels::~FbxModels()
//{
//	for (auto m : meshes) {
//		delete m;
//	}
//	meshes.clear();
//
//	for (auto m : materials) {
//		delete m.second;
//	}
//	materials.clear();
//
//	fbxScene->Destroy();
//}
//
//void FbxModels::StaticInit(ID3D12Device* dev)
//{
//	assert(!FbxModels::device);
//
//	FbxModels::device = dev;
//	FbxMeshes::StaticInit(dev);
//}
//
//FbxModels* FbxModels::CreateFromFbx(const std::string& modelname, bool smoothing)
//{
//	FbxModels* instance = new FbxModels();
//	//std::shared_ptr<Model> instance = std::make_shared<Model>();
//	instance->Init(modelname, smoothing);
//
//	return instance;
//}
//
////Load
//void FbxModels::Init(const std::string& modelname, bool smoothing)
//{
//	//Test-----
//	const std::string filename = modelname + ".fbx";
//	const std::string directoryPath = baseDirectory + modelname + "/";
//	const std::string path = directoryPath + filename;
//
//	FbxManager* fbx_manager = fbxsdk::FbxManager::Create();
//	assert(fbx_manager == nullptr);
//
//	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
//	if (fbx_importer == nullptr)
//	{
//		fbx_manager->Destroy();
//		assert(0);
//	}
//
//	fbxScene = FbxScene::Create(fbx_manager, "");
//	if (fbxScene == nullptr)
//	{
//		fbx_importer->Destroy();
//		fbx_manager->Destroy();
//		assert(0);
//	}
//
//	fbx_importer->Initialize(path.c_str());
//
//	fbx_importer->Import(fbxScene);
//
//	FbxGeometryConverter converter(fbx_manager);
//	converter.SplitMeshesPerMaterial(fbxScene, true);
//	converter.Triangulate(fbxScene, true);
//
//	int material_num = fbxScene->GetSrcObjectCount<FbxSurfaceMaterial>();
//	for (int i = 0; i < material_num; i++)
//	{
//		LoadMaterial(directoryPath, filename, fbxScene->GetSrcObject<FbxSurfaceMaterial>(i));
//	}
//
//	int mesh_num = fbxScene->GetSrcObjectCount<FbxMesh>();
//	for (int i = 0; i < mesh_num; i++)
//	{
//		CreateMesh(fbxScene->GetSrcObject<FbxMesh>(i));
//	}
//
//	int texture_num = fbxScene->GetSrcObjectCount<FbxFileTexture>();
//	for (int i = 0; i < texture_num; i++)
//	{
//		FbxFileTexture* texture = fbxScene->GetSrcObject<FbxFileTexture>(i);
//		if (texture)
//		{
//			const char* file_name01 = texture->GetFileName();
//			const char* file_name02 = texture->GetRelativeFileName();
//			int tex = texture->GetSrcObjectCount< FbxSurfaceMaterial>();
//		}
//	}
//
//	fbx_importer->Destroy();
//	//fbx_scene->Destroy();
//	fbx_manager->Destroy();
//
//	for (auto& m : meshes)
//	{
//		if (m->GetMaterial() == nullptr)
//		{
//			if (defaultMaterial == nullptr)
//			{
//				defaultMaterial = FbxMaterial::Create();
//				defaultMaterial->SetName("no material");
//				materials.emplace(defaultMaterial->GetName(), defaultMaterial);
//			}
//			m->SetMaterial(defaultMaterial);
//		}
//	}
//
//	for (auto& m : meshes)
//	{
//		m->CreateBuffers();
//	}
//
//	for (auto& m : materials) {
//		m.second->Update();
//	}
//
//	CreateDescriptorHeap();
//
//	LoadTextures();
//
//	//Test-----
//	const std::string filename = modelname + ".obj";
//	const std::string directoryPath = baseDirectory + modelname + "/";
//
//	// ファイルストリーム
//	std::ifstream file;
//	// .objファイルを開く
//	file.open(directoryPath + filename);
//	// ファイルオープン失敗をチェック
//	if (file.fail()) {
//		assert(0);
//	}
//
//	name = modelname;
//
//	// メッシュ生成
//	Mesh* mesh = new Mesh;
//	int indexCountTex = 0;
//	int indexCountNoTex = 0;
//
//	std::vector<DirectX::XMFLOAT3> positions;	// 頂点座標
//	std::vector<DirectX::XMFLOAT3> normals;	// 法線ベクトル
//	std::vector<DirectX::XMFLOAT2> texcoords;	// テクスチャUV
//	// 1行ずつ読み込む
//	std::string line;
//	while (getline(file, line)) {
//
//		// 1行分の文字列をストリームに変換して解析しやすくする
//		std::istringstream line_stream(line);
//
//		// 半角スペース区切りで行の先頭文字列を取得
//		std::string key;
//		getline(line_stream, key, ' ');
//
//		//マテリアル
//		if (key == "mtllib")
//		{
//			// マテリアルのファイル名読み込み
//			std::string filename;
//			line_stream >> filename;
//			// マテリアル読み込み
//			LoadMaterial(directoryPath, filename);
//		}
//		// 先頭文字列がgならグループの開始
//		if (key == "g") {
//			// カレントメッシュの情報が揃っているなら
//			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
//				//頂点法線の平均によるエッジの平滑化
//				if (smoothing) {
//					mesh->CalculateSmoothedVertexNormals();
//				}
//				// コンテナに登録
//				meshes.emplace_back(mesh);
//				// 次のメッシュ生成
//				mesh = new Mesh;
//				indexCountTex = 0;
//			}
//
//			// グループ名読み込み
//			std::string groupName;
//			line_stream >> groupName;
//
//			// メッシュに名前をセット
//			mesh->SetName(groupName);
//		}
//		// 先頭文字列がvなら頂点座標
//		if (key == "v") {
//			// X,Y,Z座標読み込み
//			DirectX::XMFLOAT3 position{};
//			line_stream >> position.x;
//			line_stream >> position.y;
//			line_stream >> position.z;
//			positions.emplace_back(position);
//
//			//サイズの比較
//			if (position.x > size.x) { size.x = position.x; }
//			if (position.y > size.y) { size.y = position.y; }
//			if (position.z > size.z) { size.z = position.z; }
//		}
//		// 先頭文字列がvtならテクスチャ
//		if (key == "vt")
//		{
//			// U,V成分読み込み
//			DirectX::XMFLOAT2 texcoord{};
//			line_stream >> texcoord.x;
//			line_stream >> texcoord.y;
//			// V方向反転
//			texcoord.y = 1.0f - texcoord.y;
//			// テクスチャ座標データに追加
//			texcoords.emplace_back(texcoord);
//		}
//		// 先頭文字列がvnなら法線ベクトル
//		if (key == "vn") {
//			// X,Y,Z成分読み込み
//			DirectX::XMFLOAT3 normal{};
//			line_stream >> normal.x;
//			line_stream >> normal.y;
//			line_stream >> normal.z;
//			// 法線ベクトルデータに追加
//			normals.emplace_back(normal);
//		}
//		// 先頭文字列がusemtlならマテリアルを割り当てる
//		if (key == "usemtl")
//		{
//			if (mesh->GetMaterial() == nullptr) {
//				// マテリアルの名読み込み
//				std::string materialName;
//				line_stream >> materialName;
//
//				// マテリアル名で検索し、マテリアルを割り当てる
//				auto itr = materials.find(materialName);
//				if (itr != materials.end()) {
//					mesh->SetMaterial(itr->second);
//				}
//			}
//		}
//		// 先頭文字列がfならポリゴン（三角形）
//		if (key == "f")
//		{
//			int faceIndexCount = 0;
//			// 半角スペース区切りで行の続きを読み込む
//			std::string index_string;
//			while (getline(line_stream, index_string, ' ')) {
//				// 頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
//				std::istringstream index_stream(index_string);
//				unsigned short indexPosition, indexNormal, indexTexcoord;
//				// 頂点番号
//				index_stream >> indexPosition;
//
//				Material* material = mesh->GetMaterial();
//				index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
//				// マテリアル、テクスチャがある場合
//				if (material && material->textureFilename.size() > 0) {
//					index_stream >> indexTexcoord;
//					index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
//					index_stream >> indexNormal;
//					// 頂点データの追加
//					Mesh::VertexPosNormalUv vertex{};
//					vertex.pos = positions[indexPosition - 1];
//					vertex.normal = normals[indexNormal - 1];
//					vertex.uv = texcoords[indexTexcoord - 1];
//					mesh->AddVertex(vertex);
//					//エッジ平滑化用のデータを追加
//					if (smoothing) {
//						//vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
//						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
//					}
//				}
//				else {
//					char c;
//					index_stream >> c;
//					// スラッシュ2連続の場合、頂点番号のみ
//					if (c == '/') {
//						// 頂点データの追加
//						Mesh::VertexPosNormalUv vertex{};
//						vertex.pos = positions[indexPosition - 1];
//						vertex.normal = { 0, 0, 1 };
//						vertex.uv = { 0, 0 };
//						mesh->AddVertex(vertex);
//						//エッジ平滑化用のデータを追加
//						if (smoothing) {
//							//vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
//							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
//						}
//					}
//					else {
//						index_stream.seekg(-1, std::ios_base::cur); // 1文字戻る
//						index_stream >> indexTexcoord;
//						index_stream.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
//						index_stream >> indexNormal;
//						// 頂点データの追加
//						Mesh::VertexPosNormalUv vertex{};
//						vertex.pos = positions[indexPosition - 1];
//						vertex.normal = normals[indexNormal - 1];
//						vertex.uv = { 0, 0 };
//						mesh->AddVertex(vertex);
//						//エッジ平滑化用のデータを追加
//						if (smoothing) {
//							//vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
//							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
//						}
//					}
//				}
//				// インデックスデータの追加
//				if (faceIndexCount >= 3) {
//					// 四角形ポリゴンの4点目なので、
//					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
//					mesh->AddIndex(indexCountTex - 1);
//					mesh->AddIndex(indexCountTex);
//					mesh->AddIndex(indexCountTex - 3);
//				}
//				else
//				{
//					mesh->AddIndex(indexCountTex);
//				}
//				indexCountTex++;
//				faceIndexCount++;
//			}
//		}
//	}
//	file.close();
//
//	//頂点法線の平均によるエッジの平滑化
//	if (smoothing) {
//		mesh->CalculateSmoothedVertexNormals();
//	}
//
//	// コンテナに登録
//	meshes.emplace_back(mesh);
//
//	// メッシュのマテリアルチェック
//	for (auto& m : meshes) {
//		// マテリアルの割り当てがない
//		if (m->GetMaterial() == nullptr) {
//			if (defaultMaterial == nullptr) {
//				// デフォルトマテリアルを生成
//				defaultMaterial = Material::Create();
//				defaultMaterial->name = "no material";
//				materials.emplace(defaultMaterial->name, defaultMaterial);
//			}
//			// デフォルトマテリアルをセット
//			m->SetMaterial(defaultMaterial);
//		}
//	}
//
//	// メッシュのバッファ生成
//	for (auto& m : meshes) {
//		m->CreateBuffers();
//	}
//
//	// マテリアルの数値を定数バッファに反映
//	for (auto& m : materials) {
//		m.second->Update();
//	}
//
//	// デスクリプタヒープ生成
//	CreateDescriptorHeap();
//
//	// テクスチャの読み込み
//	LoadTextures();
//}
//
//void FbxModels::Draw(ID3D12GraphicsCommandList* cmdList)
//{
//	if (descHeap) {
//		ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
//		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
//	}
//
//	for (auto& mesh : meshes) {
//		mesh->Draw(cmdList);
//	}
//}
//
////Load
//void FbxModels::LoadMaterial(const std::string& directoryPath, const std::string& filename, FbxSurfaceMaterial* fbx_material)
//{
//	//Test-----
//	FbxMaterial* l_material = FbxMaterial::Create();
//	FbxMaterial::ConstBufferDataB1 entry_material;
//	enum class MaterialOrder
//	{
//		Ambient,
//		Diffuse,
//		Specular,
//		MaxOrder,
//	};
//
//	FbxDouble3 colors[(int)MaterialOrder::MaxOrder];
//	FbxDouble factors[(int)MaterialOrder::MaxOrder];
//	FbxProperty prop = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
//
//	if (fbx_material->GetClassId().Is(FbxSurfaceLambert::ClassId))
//	{
//		const char* element_check_list[] =
//		{
//			FbxSurfaceMaterial::sAmbient,
//			FbxSurfaceMaterial::sDiffuse,
//		};
//
//		const char* factor_check_list[] =
//		{
//			FbxSurfaceMaterial::sAmbientFactor,
//			FbxSurfaceMaterial::sDiffuseFactor,
//		};
//
//		for (int i = 0; i < 2; i++)
//		{
//			prop = fbx_material->FindProperty(element_check_list[i]);
//			if (prop.IsValid()) { colors[i] = prop.Get<FbxDouble3>(); }
//			else { colors[i] = FbxDouble3(1.0, 1.0, 1.0); }
//
//			prop = fbx_material->FindProperty(factor_check_list[i]);
//			if (prop.IsValid()) { factors[i] = prop.Get<FbxDouble>(); }
//			else { factors[i] = 1.0; }
//		}
//	}
//
//	FbxDouble3 color = colors[(int)MaterialOrder::Ambient];
//	FbxDouble factor = factors[(int)MaterialOrder::Ambient];
//	entry_material.ambient = DirectX::XMFLOAT3((float)color[0], (float)color[1], (float)color[2]);
//
//	color = colors[(int)MaterialOrder::Diffuse];
//	factor = factors[(int)MaterialOrder::Diffuse];
//	entry_material.diffuse = DirectX::XMFLOAT3((float)color[0], (float)color[1], (float)color[2]);
//
//	color = colors[(int)MaterialOrder::Specular];
//	factor = factors[(int)MaterialOrder::Specular];
//	entry_material.specular = DirectX::XMFLOAT3((float)color[0], (float)color[1], (float)color[2]);
//
//	l_material->SetMaterial(entry_material);
//
//	if (l_material) { AddMaterial(l_material); }
//
//	//Test-----
//
//	//ファイルストリーム
//	std::ifstream file;
//	//マテリアルファイルを開く
//	file.open(directoryPath + filename);
//	//ファイルオープン失敗をチェック
//	if (file.fail()) {
//		assert(0);
//	}
//
//	FbxMaterial* material = nullptr;
//
//	//1行ずつ読み込む
//	std::string line;
//	while (getline(file, line)) {
//
//		//1行分の文字列をストリームに変換して解析しやすくする
//		std::istringstream line_stream(line);
//
//		//半角スペース区切りで行の先頭文字列を取得
//		std::string key;
//		getline(line_stream, key, ' ');
//
//		//先頭のタブ文字は無視する
//		if (key[0] == '\t') {
//			key.erase(key.begin()); // 先頭の文字を削除
//		}
//
//		//先頭文字列がnewmtlならマテリアル名
//		if (key == "newmtl") {
//
//			//既にマテリアルがあれば
//			if (material) {
//				//マテリアルをコンテナに登録
//				AddMaterial(material);
//			}
//
//			//新しいマテリアルを生成
//			material = FbxMaterial::Create();
//			//マテリアル名読み込み
//			line_stream >> material->GetName();
//		}
//		//先頭文字列がKaならアンビエント色
//		if (key == "Ka") {
//			line_stream >> material->ambient.x;
//			line_stream >> material->ambient.y;
//			line_stream >> material->ambient.z;
//		}
//		//先頭文字列がKdならディフューズ色
//		if (key == "Kd") {
//			line_stream >> material->diffuse.x;
//			line_stream >> material->diffuse.y;
//			line_stream >> material->diffuse.z;
//		}
//		//先頭文字列がKsならスペキュラー色
//		if (key == "Ks") {
//			line_stream >> material->specular.x;
//			line_stream >> material->specular.y;
//			line_stream >> material->specular.z;
//		}
//		//先頭文字列がmap_Kdならテクスチャファイル名
//		if (key == "map_Kd") {
//			//テクスチャのファイル名読み込み
//			line_stream >> material->textureFilename;
//
//			//フルパスからファイル名を取り出す
//			size_t pos1;
//			pos1 = material->textureFilename.rfind('\\');
//			if (pos1 != std::string::npos) {
//				material->textureFilename = material->textureFilename.substr(pos1 + 1, material->textureFilename.size() - pos1 - 1);
//			}
//
//			pos1 = material->textureFilename.rfind('/');
//			if (pos1 != std::string::npos) {
//				material->textureFilename = material->textureFilename.substr(pos1 + 1, material->textureFilename.size() - pos1 - 1);
//			}
//		}
//	}
//	//ファイルを閉じる
//	file.close();
//
//	if (material) {
//		//マテリアルを登録
//		AddMaterial(material);
//	}
//}
//
//void FbxModels::AddMaterial(FbxMaterial* material)
//{
//	materials.emplace(material->GetName(), material);
//}
//
//void FbxModels::CreateDescriptorHeap()
//{
//	HRESULT result = S_FALSE;
//
//	//マテリアルの数
//	size_t count = materials.size();
//
//	//デスクリプタヒープ生成(1つにまとめた方が楽！0~100が定数バッファ,101~200がSRV...)←cmdListを呼ぶ回数が減る
//	if (count > 0) {
//		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
//		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//		descHeapDesc.NumDescriptors = (UINT)count;
//		result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));
//		if (FAILED(result)) {
//			assert(0);
//		}
//	}
//
//	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//}
//
//void FbxModels::LoadTextures()
//{
//	int textureIndex = 0;
//	std::string directoryPath = baseDirectory + name + "/";
//
//	for (auto& m : materials) {
//		FbxMaterial* material = m.second;
//
//		//テクスチャあり
//		if (material->GetTextureFilename().size() > 0) {
//			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
//			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
//			//マテリアルにテクスチャ読み込み
//			material->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
//			textureIndex++;
//		}
//		//テクスチャなし
//		else {
//			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
//			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
//			//マテリアルにテクスチャ読み込み
//			material->LoadTexture(baseDirectory, cpuDescHandleSRV, gpuDescHandleSRV);
//			textureIndex++;
//		}
//	}
//}
//
//void FbxModels::CreateMesh(FbxMesh* fbx_mesh)
//{
//	FbxMeshes* mesh = new FbxMeshes;
//
//	LoadIndices(mesh, fbx_mesh);
//	LoadVertices(mesh, fbx_mesh);
//	LoadNormals(mesh, fbx_mesh);
//	LoadUV(mesh, fbx_mesh);
//	LoadColors(mesh, fbx_mesh);
//	SetMaterialName(mesh, fbx_mesh);
//
//	meshes.push_back(mesh);
//}
//
//void FbxModels::LoadIndices(FbxMeshes* mesh_data, FbxMesh* mesh)
//{
//	int polygon_count = mesh->GetPolygonCount();
//
//	for (int i = 0; i < polygon_count; i++)
//	{
//		mesh_data->AddIndex(i * 3 + 2);
//		mesh_data->AddIndex(i * 3 + 1);
//		mesh_data->AddIndex(i * 3);
//	}
//}
//
//void FbxModels::LoadVertices(FbxMeshes* mesh_data, FbxMesh* mesh)
//{
//	FbxVector4* vertices = mesh->GetControlPoints();
//	int* indices = mesh->GetPolygonVertices();
//	int polygon_vertex_count = mesh->GetPolygonVertexCount();
//
//	for (int i = 0; i < polygon_vertex_count; i++)
//	{
//		FbxMeshes::VertexPosNormalUv vertex;
//		int index = indices[i];
//
//		vertex.pos.x = (float)-vertices[index][0];
//		vertex.pos.y = (float)vertices[index][1];
//		vertex.pos.z = (float)vertices[index][2];
//
//		mesh_data->AddVertex(vertex);
//	}
//}
//
//void FbxModels::LoadNormals(FbxMeshes* mesh_data, FbxMesh* mesh)
//{
//	FbxArray<FbxVector4> normals;
//	mesh->GetPolygonVertexNormals(normals);
//
//	for (int i = 0; i < normals.Size(); i++)
//	{
//		mesh_data->GetVertices()[i].normal.x = (float)-normals[i][0];
//		mesh_data->GetVertices()[i].normal.y = (float)normals[i][1];
//		mesh_data->GetVertices()[i].normal.z = (float)normals[i][2];
//	}
//}
//
//void FbxModels::LoadUV(FbxMeshes* mesh_data, FbxMesh* mesh)
//{
//	FbxStringList uvset_names;
//	mesh->GetUVSetNames(uvset_names);
//
//	FbxArray<FbxVector2> uv_buffer;
//	mesh->GetPolygonVertexUVs(uvset_names.GetStringAt(0), uv_buffer);
//
//	for (int i = 0; i < uv_buffer.Size(); i++)
//	{
//		FbxVector2& uv = uv_buffer[i];
//
//		mesh_data->GetVertices()[i].uv.x = (float)uv[0];
//		mesh_data->GetVertices()[i].uv.y = (float)(1.0 - uv[1]);
//	}
//}
//
//void FbxModels::LoadColors(FbxMeshes* mesh_data, FbxMesh* mesh)
//{
//	int color_count = mesh->GetElementVertexColorCount();
//	if (color_count == 0) { return; }
//
//	FbxGeometryElementVertexColor* vertex_colors = mesh->GetElementVertexColor(0);
//	if (vertex_colors == nullptr) { return; }
//
//	FbxLayerElement::EMappingMode mapping_mode = vertex_colors->GetMappingMode();
//	FbxLayerElement::EReferenceMode reference_mode = vertex_colors->GetReferenceMode();
//	if (mapping_mode == FbxLayerElement::eByPolygonVertex)
//	{
//		if (reference_mode == FbxLayerElement::eIndexToDirect)
//		{
//			FbxLayerElementArrayTemplate<FbxColor>& colors = vertex_colors->GetDirectArray();
//			FbxLayerElementArrayTemplate<int>& indeces = vertex_colors->GetIndexArray();
//			for (int i = 0; i < indeces.GetCount(); i++)
//			{
//				int id = indeces.GetAt(i);
//				FbxColor color = colors.GetAt(id);
//				mesh_data->GetVertices()[i].color.x = (float)color.mRed;
//				mesh_data->GetVertices()[i].color.y = (float)color.mGreen;
//				mesh_data->GetVertices()[i].color.z = (float)color.mBlue;
//				mesh_data->GetVertices()[i].color.w = (float)color.mAlpha;
//			}
//		}
//	}
//}
//
//void FbxModels::SetMaterialName(FbxMeshes* mesh_data, FbxMesh* mesh)
//{
//	if (mesh->GetElementMaterialCount() == 0)
//	{
//		mesh_data->SetName("");
//		return;
//	}
//
//	FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
//	int index = material->GetIndexArray().GetAt(0);
//	FbxSurfaceMaterial* surface_material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);
//	if (surface_material != nullptr)
//	{
//		mesh_data->SetName(surface_material->GetName());
//	}
//	else
//	{
//		mesh_data->SetName("");
//	}
//}
