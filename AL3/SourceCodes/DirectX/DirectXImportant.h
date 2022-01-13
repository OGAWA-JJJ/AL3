#pragma once
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <dinput.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTex.h>

#include <cassert>
#include <fstream>
#include <xaudio2.h>
#include <wrl.h>

#include <vector>
#include <string>

#include "../WinAPI/WindowsAPI.h"
#include "../Input/Input.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

class DirectXImportant
{
public:
	static ComPtr<ID3D12Device> dev;
	static ComPtr<IDXGIFactory6> dxgiFactory;
	static ComPtr<IDXGISwapChain4> swapchain;
	static ComPtr<ID3D12CommandAllocator> cmdAllocator;
	static ComPtr<ID3D12GraphicsCommandList> cmdList;
	//«ƒƒ“ƒ`ƒƒƒ“•K—v‚È‚¢‚©‚à
	static ComPtr<ID3D12CommandQueue> cmdQueue;
	static ComPtr<ID3D12DescriptorHeap> rtvHeaps;
};
