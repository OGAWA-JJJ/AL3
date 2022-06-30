#include "Object3D.h"
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <DirectXTex.h>

#pragma comment(lib,"d3dcompiler.lib")

std::vector<Model3D::Vertex>  Model3D::vertices;
std::vector<unsigned short>  Model3D::indices;
Model3D::Material Model3D::material;

Model3D::Model3D()
{
	scale = { 3.0f,3.0f,3.0f };
	rotation = { 0.0f,-90.0f,0.0f };
	position = { 0.0f,0.0f,0.0f };
}

void Model3D::CreateDescriptorHeap()
{
	if (basicDescHeap != nullptr) return;

	HRESULT result = S_FALSE;

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = 1;
	result = DirectXImportant::dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
}

void Model3D::CreateGraphicsPipeline()
{
	if (pipelinestate != nullptr) return;

	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ObjVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSmain", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/ObjPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSmain", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	/*頂点シェーダ、ピクセルシェーダをパイプラインに設定*/
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

	/*サンプルマスクとラスタライザステートの設定*/
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//gpipeline.RasterizerState.DepthClipEnable = true;
	gpipeline.DepthStencilState.DepthEnable = true;                          //深度テストを行う
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; //書き込み許可
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;      //小さければ合格
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                             //深度値フォーマット

	/*共通設定*/
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.BlendEnable = true;
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	/*半透明合成*/
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpipeline.SampleDesc.Count = 1;

	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootparams[3] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	result = DirectXImportant::dev->CreateRootSignature(0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));

	gpipeline.pRootSignature = rootsignature.Get();

	result = DirectXImportant::dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
}

void Model3D::LoadTexture(const std::string& directoryPath, const std::string& filename)
{
	HRESULT result = S_FALSE;

	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	std::string filepath = directoryPath + filename;

	wchar_t wfilepath[128];
	int iBufferSize = MultiByteToWideChar(
		CP_ACP,
		0,
		filepath.c_str(),
		-1,
		wfilepath,
		_countof(wfilepath));

	result = LoadFromWICFile(
		wfilepath, DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) {
		assert(0);
	}

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));
	if (FAILED(result)) {
		assert(0);
	}

	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch);
	if (FAILED(result)) {
		assert(0);
	}

	//ComPtr<ID3D12DescriptorHeap> descHeap;

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	/*basicHeapHandle.ptr +=
		DirectXImportant::dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);*/

	DirectXImportant::dev->CreateShaderResourceView(texbuff.Get(),
		&srvDesc,
		basicHeapHandle);
}

void Model3D::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	std::ifstream file;
	file.open(directoryPath + filename);

	if (file.fail()) {
		assert(0);
	}

	std::string line;
	while (getline(file, line)) {
		std::istringstream line_stream(line);
		std::string key;
		getline(line_stream, key, ' ');

		if (key[0] == '\t') {
			key.erase(key.begin());
		}

		if (key == "newmtl") {
			line_stream >> material.name;
		}

		if (key == "Ka") {
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}

		if (key == "Kd") {
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.x;
		}

		if (key == "Ks") {
			line_stream >> material.specular.x;
			line_stream >> material.specular.x;
			line_stream >> material.specular.x;
		}

		if (key == "map_Kd") {
			line_stream >> material.textureFilename;
			LoadTexture(directoryPath, material.textureFilename);
		}
	}
	file.close();
}

void Model3D::CreateModel(const std::string& modelName)
{
	HRESULT result;

	std::ifstream file;
	const std::string modelname = modelName;
	const std::string filename = modelname + ".obj";
	const std::string directoryPath = "Resources/";
	file.open(directoryPath + filename);

	if (file.fail()) {
		OutputDebugStringA("-----読み込みに失敗-----\n");
		assert(0);
	}

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoords;

	std::string line;
	while (getline(file, line)) {
		std::istringstream line_stream(line);

		std::string key;
		getline(line_stream, key, ' ');

		if (key == "v") {
			DirectX::XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			positions.emplace_back(position);
		}

		if (key == "vt") {
			DirectX::XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}

		if (key == "vn") {
			DirectX::XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			normals.emplace_back(normal);
		}

		if (key == "f") {
			std::string index_string;
			while (getline(line_stream, index_string, ' ')) {
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexNormal;

				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				if (normals.empty())
					vertex.normal = normals[indexNormal - 1];
				if (!texcoords.empty())
					vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);
				indices.emplace_back((unsigned short)indices.size());
			}
		}

		if (key == "mtllib") {
			std::string filename;
			line_stream >> filename;
			LoadMaterial(directoryPath, filename);
		}
	}
	file.close();

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	// 頂点バッファ生成
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// インデックスバッファ生成
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// 頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	// インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void Model3D::Init()
{
	HRESULT result;

	/*定数バッファの作成*/
	D3D12_HEAP_PROPERTIES cbheapprop{};
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferDataB0) + 0xff) & ~0xff;
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = DirectXImportant::dev->CreateCommittedResource(
		&cbheapprop,
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));

	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(constBuffB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1));

	result = constBuffB0->Map(0, nullptr, (void**)&constMapB0);
	constMapB0->mat = matWorld * Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	constBuffB0->Unmap(0, nullptr);

	result = constBuffB1->Map(0, nullptr, (void**)&constMapB1);
	constMapB1->ambient = material.ambient;
	constMapB1->diffuse = material.diffuse;
	constMapB1->specular = material.specular;
	constMapB1->alpha = material.alpha;
	constBuffB1->Unmap(0, nullptr);

	/*定数バッファビューの作成*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuffB1->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuffB1->GetDesc().Width;

	basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	DirectXImportant::dev->CreateConstantBufferView(
		&cbvDesc, basicHeapHandle);
}

void Model3D::Update()
{
	HRESULT result;
	DirectX::XMMATRIX matScale, matTrans;

	matWorld = DirectX::XMMatrixIdentity();

	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));

	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	DirectX::XMFLOAT3 Rotation = { rotation.x,rotation.y,rotation.z };
	constMapB0->mat = matWorld * Camera::ViewMatrix() * Camera::PerspectiveMatrix();
}

void Model3D::Draw()
{
	DirectXImportant::cmdList->SetPipelineState(pipelinestate.Get());
	DirectXImportant::cmdList->SetGraphicsRootSignature(rootsignature.Get());

	DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectXImportant::cmdList->IASetVertexBuffers(0, 1, &vbView);
	DirectXImportant::cmdList->IASetIndexBuffer(&ibView);

	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	DirectXImportant::cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	DirectXImportant::cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	DirectXImportant::cmdList->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandle;
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	/*gpuDescHandle.ptr += DirectXImportant::dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);*/

	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(2, gpuDescHandle);

	DirectXImportant::cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

DirectX::XMMATRIX Model3D::GetmatRot()
{
	return matRot;
}

DirectX::XMFLOAT3 Model3D::GetPosition()
{
	return position;
}
