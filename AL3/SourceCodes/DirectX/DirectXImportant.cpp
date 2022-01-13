#include "DirectXImportant.h"

ComPtr<ID3D12Device> DirectXImportant::dev;
ComPtr<IDXGIFactory6> DirectXImportant::dxgiFactory;
ComPtr<IDXGISwapChain4> DirectXImportant::swapchain;
ComPtr<ID3D12CommandAllocator> DirectXImportant::cmdAllocator;
ComPtr<ID3D12GraphicsCommandList> DirectXImportant::cmdList;
//«ƒƒ“ƒ`ƒƒƒ“•K—v‚È‚¢‚©‚à
ComPtr<ID3D12CommandQueue> DirectXImportant::cmdQueue;
ComPtr<ID3D12DescriptorHeap> DirectXImportant::rtvHeaps;