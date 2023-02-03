#include "DirectXImportant.h"

Microsoft::WRL::ComPtr<ID3D12Device> DirectXImportant::dev;
Microsoft::WRL::ComPtr<IDXGIFactory6> DirectXImportant::dxgiFactory;
Microsoft::WRL::ComPtr<IDXGISwapChain4> DirectXImportant::swapchain;
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> DirectXImportant::cmdAllocator;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DirectXImportant::cmdList;
//«ƒƒ“ƒ`ƒƒƒ“•K—v‚È‚¢‚©‚à
Microsoft::WRL::ComPtr<ID3D12CommandQueue> DirectXImportant::cmdQueue;