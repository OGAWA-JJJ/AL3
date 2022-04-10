#include "Texture.h"
#include "../DirectX/DirectXImportant.h"

Texture::~Texture()
{
	if (m_texture) { m_texture->Release(); }
}

void Texture::InitFromD3DResource(ID3D12Resource* texture)
{
	if (m_texture) { m_texture->Release(); }
	m_texture = texture;
	m_texture->AddRef();
	m_textureDesc = m_texture->GetDesc();
}

void Texture::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
{
	if (m_texture) {
		auto device = DirectXImportant::dev;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;
		device->CreateShaderResourceView(m_texture, &srvDesc, descriptorHandle);
	}
}
