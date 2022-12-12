#include "TexManager.h"
#include "../DirectX/DirectXImportant.h"

UINT TexManager::MAX_NUM_SRV = 512;
UINT TexManager::MAX_NUM_RTV = 32;
UINT TexManager::MAX_NUM_DSV = 32;
UINT TexManager::SPRITE_START_INDEX = 256;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TexManager::m_descHeapSRV;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TexManager::m_descHeapRTV;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TexManager::m_descHeapDSV;
std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 512> TexManager::m_buffer;
UINT TexManager::m_offsetSRV = 0;
UINT TexManager::m_offsetRTV = 0;
UINT TexManager::m_offsetDSV = 0;
UINT TexManager::m_spriteOffset = TexManager::SPRITE_START_INDEX;

void TexManager::StaticInit()
{
	//SRV
	D3D12_DESCRIPTOR_HEAP_DESC descHeapSettingsSRV = {};
	descHeapSettingsSRV.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapSettingsSRV.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapSettingsSRV.NumDescriptors = MAX_NUM_SRV;
	DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapSettingsSRV,
		IID_PPV_ARGS(&m_descHeapSRV));

	//RTV
	D3D12_DESCRIPTOR_HEAP_DESC descHeapSettingsRTV = {};
	descHeapSettingsRTV.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descHeapSettingsRTV.NumDescriptors = MAX_NUM_RTV;
	DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapSettingsRTV,
		IID_PPV_ARGS(&m_descHeapRTV));

	//DSV
	D3D12_DESCRIPTOR_HEAP_DESC descHeapSettingsDSV = {};
	descHeapSettingsDSV.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descHeapSettingsDSV.NumDescriptors = MAX_NUM_DSV;
	DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapSettingsDSV,
		IID_PPV_ARGS(&m_descHeapDSV));
}
