#include "ConstantBuffer.h"
#include <cassert>
#include "d3dx12.h"

ID3D12Resource* ConstantBuffer::CreateConstantBuffer(size_t size)
{
	ID3D12Resource* constBuff;
	HRESULT result;

	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((size + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(SUCCEEDED(result));

	return constBuff;
}

void ConstantBuffer::CopyToVRAM(Microsoft::WRL::ComPtr<ID3D12Resource> constBuff, void* data, size_t size)
{
	void* dst = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&dst);
	memcpy(dst, data, size);
	constBuff->Unmap(0, nullptr);
	assert(SUCCEEDED(result));
}
