#include <d3d12.h>

class IShaderResource
{
public:
	virtual ~IShaderResource() {}

	//SRV�ɓo�^
	virtual void RegistShaderResourceView(
		D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle,
		int bufferNo
	) = 0;
};
