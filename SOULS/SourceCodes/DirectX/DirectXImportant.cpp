#include "DirectXImportant.h"

Microsoft::WRL::ComPtr<ID3D12Device> DirectXImportant::dev;
Microsoft::WRL::ComPtr<IDXGIFactory6> DirectXImportant::dxgiFactory;
Microsoft::WRL::ComPtr<IDXGISwapChain4> DirectXImportant::swapchain;
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> DirectXImportant::cmdAllocator;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> DirectXImportant::cmdList;
//�������`�����K�v�Ȃ�����
Microsoft::WRL::ComPtr<ID3D12CommandQueue> DirectXImportant::cmdQueue;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXImportant::rtvHeaps;