#include "ShadowMap.h"
#include "../DirectX/ConstantBuffer.h"
#include "../DirectX/Camera.h"
#include "../2D/TexManager.h"

#include "../DirectX/d3dx12.h"
#include <d3dcompiler.h>
#include <cassert>

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> ShadowMap::cmdList = nullptr;

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::Init()
{
	CreateConstBuff();

	incrementSizeSRV = DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	incrementSizeDSV = DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//�����n
	CreateDSV();

	CreateSRV();
}

void ShadowMap::PreDraw()
{
	//���\�[�X�o���A��ύX
	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			TexManager::GetBuffer(inDsvIndex).Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_DEPTH_WRITE)
	);

	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		TexManager::GetHeapAdressDSV()->GetCPUDescriptorHandleForHeapStart();
	dsvH.ptr += dsvIndex * incrementSizeDSV;
	//�Z�b�g
	cmdList->OMSetRenderTargets(0, nullptr, false, &dsvH);

	//�r���[�|�[�g�̐ݒ�
	cmdList->RSSetViewports(
		1,
		&CD3DX12_VIEWPORT(0.0f, 0.0f,
			static_cast<float>(SHADOW_WINDOW_WIDTH), static_cast<float>(SHADOW_WINDOW_HEIGHT))
	);
	//�V�U�����O��`�̐ݒ�
	cmdList->RSSetScissorRects(
		1,
		&CD3DX12_RECT(0, 0, SHADOW_WINDOW_WIDTH, SHADOW_WINDOW_HEIGHT)
	);

	//�[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void ShadowMap::PostDraw()
{
	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			TexManager::GetBuffer(inDsvIndex).Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_GENERIC_READ)
	);
}

void ShadowMap::CreateConstBuff()
{
	//�萔�o�b�t�@�̐���
	constBuff = ConstantBuffer::CreateConstantBuffer(sizeof(ConstantBuffer_b0));

	//�萔�o�b�t�@�Ƀf�[�^�]��
	ConstantBuffer_b0 data;
	data.mat = DirectX::XMMatrixIdentity();

	ConstantBuffer::CopyToVRAM(constBuff, &data, sizeof(ConstantBuffer_b0));
}

void ShadowMap::CreateSRV()
{
	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		TexManager::GetCpuHeapStartSRV(),
		TexManager::GetOffsetSRV(),
		incrementSizeSRV);

	//�f�X�N���v�^�q�[�v��SRV���쐬
	DirectXImportant::dev->CreateShaderResourceView(
		TexManager::GetBuffer(inDsvIndex).Get(),
		&srvDesc,
		cpuHandle
	);

	TexManager::SetIncrementSizeSRV(incrementSizeSRV);

	TexManager::AddOffsetSRV();
}

void ShadowMap::CreateDSV()
{
	HRESULT result;

	//�[�x�o�b�t�@�p���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			SHADOW_WINDOW_WIDTH,
			SHADOW_WINDOW_HEIGHT,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

	//�[�x�o�b�t�@����
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&TexManager::GetBuffer())
	);
	assert(SUCCEEDED(result));

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		TexManager::GetCpuHeapStartDSV(),
		TexManager::GetOffsetDSV(),
		incrementSizeDSV);

	//�f�X�N���v�^�q�[�v��DSV�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		TexManager::GetBuffer().Get(),
		&dsvDesc,
		cpuHandle
	);

	dsvIndex = TexManager::GetOffsetDSV();
	inDsvIndex = TexManager::GetOffsetSRV();
	TexManager::SetShadowInDsvIndex(inDsvIndex);

	TexManager::AddOffsetDSV();
}