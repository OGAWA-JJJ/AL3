#include "FbxMeshes.h"
#include <d3dcompiler.h>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")

ID3D12Device* FbxMeshes::device = nullptr;

void FbxMeshes::StaticInit(ID3D12Device* dev)
{
	assert(!FbxMeshes::device);

	device = dev;
	FbxMaterial::StaticInit(dev);
}

void FbxMeshes::SetName(const std::string& name)
{
	this->name = name;
}

void FbxMeshes::AddVertex(const VertexPosNormalUv& vertex)
{
	vertices.emplace_back(vertex);
}

void FbxMeshes::AddIndex(unsigned short index)
{
	indices.emplace_back(index);
}

void FbxMeshes::SetMaterial(FbxMaterial* material)
{
	this->material = material;
}

void FbxMeshes::CreateBuffers()
{
	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices.size());

	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices.begin(), vertices.end(), vertMap);
		vertBuff->Unmap(0, nullptr);
	}

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	result = device->CreateCommittedResource(
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

	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices.begin(), indices.end(), indexMap);
		indexBuff->Unmap(0, nullptr);
	}

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
}

void FbxMeshes::Draw(ID3D12GraphicsCommandList* cmdList, FbxMaterial* fbxMaterial)
{
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);
	cmdList->SetGraphicsRootDescriptorTable(1, fbxMaterial->GetGpuHandle());

	//FbxMaterial‚ÌConstBuff
	ID3D12Resource* constBuff = fbxMaterial->GetConstantBuffer();
	cmdList->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());

	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}

void FbxMeshes::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData[indexPosition].emplace_back(indexVertex);
}

void FbxMeshes::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData.begin();
	for (; itr != smoothData.end(); ++itr) {
		std::vector<unsigned short>& v = itr->second;
		DirectX::XMVECTOR normal = {};
		for (unsigned short index : v) {
			DirectX::XMVECTOR xmvector = DirectX::XMVectorSet(vertices[index].normal.x, vertices[index].normal.y, vertices[index].normal.z, 0);
			normal = DirectX::XMVectorAdd(normal, xmvector);
		}
		normal.m128_f32[0] = normal.m128_f32[0] / (float)v.size();
		normal.m128_f32[1] = normal.m128_f32[1] / (float)v.size();
		normal.m128_f32[2] = normal.m128_f32[2] / (float)v.size();
		normal.m128_f32[3] = normal.m128_f32[3] / (float)v.size();
		normal = DirectX::XMVector3Normalize(normal);

		for (unsigned short index : v) {
			vertices[index].normal = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2] };
		}
	}
}
