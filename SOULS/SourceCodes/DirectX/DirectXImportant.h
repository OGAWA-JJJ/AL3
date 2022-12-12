#pragma once
#include "../WinAPI/WindowsAPI.h"
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXImportant
{
public:
	static Microsoft::WRL::ComPtr<ID3D12Device> dev;
	static Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	static Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;
	static Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	//«ƒƒ“ƒ`ƒƒƒ“•K—v‚È‚¢‚©‚à
	static Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;

	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
};
