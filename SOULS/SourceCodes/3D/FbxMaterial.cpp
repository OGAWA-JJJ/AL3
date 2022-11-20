#include "../3D/FbxMaterial.h"
#include <DirectXTex.h>
#include <cassert>

ID3D12Device* FbxMaterial::device = nullptr;

void FbxMaterial::StaticInit(ID3D12Device* dev)
{
	assert(!FbxMaterial::device);

	device = dev;
}

FbxMaterial* FbxMaterial::Create()
{
	FbxMaterial* instance = new FbxMaterial;

	instance->Init();

	return instance;
}

void FbxMaterial::LoadTexture(const std::string& directoryPath, CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	if (textureFilename.size() == 0) {
		textureFilename = "white1x1.png";
	}

	cpuDescHandleSRV = cpuHandle;
	gpuDescHandleSRV = gpuHandle;

	HRESULT result = S_FALSE;

	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	std::string filepath = directoryPath + textureFilename;
	wchar_t wfilepath[128];

	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	result = LoadFromDDSFile(
		wfilepath, DirectX::DDS_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result))
	{
		result = LoadFromWICFile(
			wfilepath, DirectX::WIC_FLAGS_NONE,
			&metadata, scratchImg);
		if (FAILED(result)) { assert(0); }
	}

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	result = device->CreateCommittedResource(
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
		(UINT)img->slicePitch
	);
	if (FAILED(result)) {
		assert(0);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texbuff.Get(),
		&srvDesc,
		cpuDescHandleSRV
	);
}

void FbxMaterial::Update()
{
	HRESULT result;

	ConstBufferDataB1* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->ambient = ambient;
		constMap->diffuse = diffuse;
		constMap->specular = specular;
		constMap->color = color;
		constBuff->Unmap(0, nullptr);
	}
}

void FbxMaterial::Init()
{
	CreateConstantBuffer();
}

void FbxMaterial::CreateConstantBuffer()
{
	HRESULT result;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) {
		assert(0);
	}
}
