#pragma once
#include "../DirectX/DirectXImportant.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DirectXBase
{
private:
	static HRESULT result;
	static ComPtr<IDXGIAdapter> tmpAdapter;
	static D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	static ComPtr<ID3D12Resource> depthBuffer;
	static ComPtr<ID3D12DescriptorHeap> dsvHeap;
	static ComPtr<ID3D12Fence> fence;
	static UINT64 fenceVal;
	static std::vector<ComPtr<ID3D12Resource>>backBuffers;
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	static D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
	static UINT bbIndex;

public:
	//èâä˙âª
	static void DXGIFactory();
	static void GraphicsAdapter();
	static void Device();
	static void CommandAllocator();
	static void CommandList();
	static void CommandQueue();
	static void SwapChain(HWND hwnd);
	static void RenderTargetView();
	static void DepthBuffer();
	static void DepthStencilView();
	static void Fence();
	//ï`âÊëO
	static void SetDrawMode();
	static void RenderTarget();
	static void ClearScreen(float* ClearColor);
	static void Viewports();
	static void ScissorRects();

	//ï`âÊå„
	static void SetIndicateMode();
	static void CloseCommandList();
	static void ExecuteCommandList();
	static void WaitForSingle();
	static void ResetCmdAllocator();
	static void ResetCommandList();
	static void Swap();

public:
	//DirectXBase();
	static void Init(HWND hwnd);
	static void BeforeDraw(float* ClearColor);
	static void AfterDraw();
};

