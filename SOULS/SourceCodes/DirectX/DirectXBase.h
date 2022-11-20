#pragma once
#include "../DirectX/DirectXImportant.h"
#include <vector>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DirectXBase
{
private:
	static HRESULT result;
	static Microsoft::WRL::ComPtr<IDXGIAdapter> tmpAdapter;
	static D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	static Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	static Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	static UINT64 fenceVal;
	static std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>backBuffers;
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	static D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
	static UINT bbIndex;

public:
	//������
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
	//�`��O
	static void SetDrawMode();
	static void RenderTarget();
	static void ClearScreen(float* ClearColor);
	static void Viewports();
	static void ScissorRects();

	//�`���
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

