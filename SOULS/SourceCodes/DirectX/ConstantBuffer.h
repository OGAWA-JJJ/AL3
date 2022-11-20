#pragma once
#include <wrl.h>
#include "DirectXImportant.h"

class ConstantBuffer
{
public:
	//�萔�o�b�t�@����(sizeof(Struct))
	static ID3D12Resource* CreateConstantBuffer(size_t size);

	//�萔�o�b�t�@�]��(ConstantBuffer,Struct,sizeof(Struct))
	static void CopyToVRAM(Microsoft::WRL::ComPtr<ID3D12Resource> constBuff, void* data, size_t size);
};
