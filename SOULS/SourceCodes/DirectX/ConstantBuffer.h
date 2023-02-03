#pragma once
#include <wrl.h>
#include "DirectXImportant.h"

class ConstantBuffer
{
public:
	//定数バッファ生成(sizeof(Struct))
	static ID3D12Resource* CreateConstantBuffer(size_t size);

	//定数バッファ転送(ConstantBuffer,Struct,sizeof(Struct))
	static void CopyToVRAM(Microsoft::WRL::ComPtr<ID3D12Resource> constBuff, void* data, size_t size);
};
