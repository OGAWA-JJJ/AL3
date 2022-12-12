#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <array>

class TexManager
{
private:
	static UINT MAX_NUM_SRV;
	static UINT MAX_NUM_RTV;
	static UINT MAX_NUM_DSV;
	static UINT SPRITE_START_INDEX;

private:
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descHeapSRV;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descHeapRTV;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descHeapDSV;
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 512> m_buffer;
	static UINT m_offsetSRV;
	static UINT m_offsetRTV;
	static UINT m_offsetDSV;
	static UINT m_spriteOffset;

public:
	static void StaticInit();

public:
	static UINT GetSpriteStartIndex() { return SPRITE_START_INDEX; }

	static UINT GetOffsetSRV() { return m_offsetSRV; }
	static void AddOffsetSRV() { m_offsetSRV++; }

	static UINT GetOffsetRTV() { return m_offsetRTV; }
	static void AddOffsetRTV() { m_offsetRTV++; }

	static UINT GetOffsetDSV() { return m_offsetDSV; }
	static void AddOffsetDSV() { m_offsetDSV++; }

	static UINT GetSpriteOffset() { return m_spriteOffset; }
	static void AddSpriteOffset() { m_spriteOffset++; }

	static Microsoft::WRL::ComPtr<ID3D12Resource>& GetBuffer() { return m_buffer[m_offsetSRV]; }
	static Microsoft::WRL::ComPtr<ID3D12Resource>& GetSpriteBuffer() { return m_buffer[m_spriteOffset]; }
	static Microsoft::WRL::ComPtr<ID3D12Resource>& GetBuffer(UINT texNum) { return m_buffer[texNum]; }
	static Microsoft::WRL::ComPtr<ID3D12Resource>& GetSpriteBuffer(UINT texNum) { return m_buffer[texNum + SPRITE_START_INDEX]; }

	static ID3D12DescriptorHeap* GetHeapAdressSRV() { return m_descHeapSRV.Get(); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapStartSRV() {
		return m_descHeapSRV->GetGPUDescriptorHandleForHeapStart();
	}
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHeapStartSRV() {
		return m_descHeapSRV->GetCPUDescriptorHandleForHeapStart();
	}

	static ID3D12DescriptorHeap* GetHeapAdressRTV() { return m_descHeapRTV.Get(); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapStartRTV() {
		return m_descHeapRTV->GetGPUDescriptorHandleForHeapStart();
	}
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHeapStartRTV() {
		return m_descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	}

	static ID3D12DescriptorHeap* GetHeapAdressDSV() { return m_descHeapDSV.Get(); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHeapStartDSV() {
		return m_descHeapDSV->GetGPUDescriptorHandleForHeapStart();
	}
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHeapStartDSV() {
		return m_descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	}
};

