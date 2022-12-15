#include "DirectXBase.h"
#include "../2D/TexManager.h"
#include <string>
#include "d3dx12.h"

HRESULT DirectXBase::result;
Microsoft::WRL::ComPtr<IDXGIAdapter> DirectXBase::tmpAdapter = nullptr;
Microsoft::WRL::ComPtr<ID3D12Fence> DirectXBase::fence = nullptr;
UINT64 DirectXBase::fenceVal = 0;
D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::rtvH;
D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::dsvH;
std::array<UINT, 2> DirectXBase::bbIndex;
UINT DirectXBase::incrementSizeRTV = 0;
UINT DirectXBase::incrementSizeDSV = 0;
UINT DirectXBase::dsvIndex = 0;
UINT DirectXBase::currentIndex = 0;

#pragma region 初期化
void DirectXBase::DXGIFactory()
{
	//DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&DirectXImportant::dxgiFactory));
}

void DirectXBase::GraphicsAdapter()
{
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter>>adapters;

	for (int i = 0;
		DirectXImportant::dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		//動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};

		//アダプターの情報を取得
		adapters[i]->GetDesc(&adesc);

		//アダプター名
		std::wstring strDesc = adesc.Description;

		//Microsoft Basic Render Driver,Intel UHD Graphicsを回避
		if (strDesc.find(L"Microsoft") == std::wstring::npos
			&& strDesc.find(L"Intel") == std::wstring::npos)
		{
			//採用
			tmpAdapter = adapters[i];
			break;
		}
	}
}

void DirectXBase::Device()
{
	/*デバイスの生成*/

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(
			tmpAdapter.Get(),
			levels[i],
			IID_PPV_ARGS(DirectXImportant::dev.ReleaseAndGetAddressOf()));

		if (result == S_OK)
		{
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectXBase::CommandAllocator()
{
	//コマンドアロケータを生成
	result = DirectXImportant::dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&DirectXImportant::cmdAllocator));
}

void DirectXBase::CommandList()
{
	result = DirectXImportant::dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		DirectXImportant::cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&DirectXImportant::cmdList));
}

void DirectXBase::CommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	DirectXImportant::dev->CreateCommandQueue(
		&cmdQueueDesc,
		IID_PPV_ARGS(&DirectXImportant::cmdQueue));
}

void DirectXBase::SwapChain(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;

	//色情報の書式
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//マルチサンプルしない
	swapchainDesc.SampleDesc.Count = 1;

	//バックバッファ用
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	//フリップ後は破棄
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain1;

	DirectXImportant::dxgiFactory->CreateSwapChainForHwnd(
		DirectXImportant::cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		swapchain1.ReleaseAndGetAddressOf());

	swapchain1.As(&DirectXImportant::swapchain);
}

void DirectXBase::RenderTargetView()
{
	incrementSizeRTV = DirectXImportant::dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//表裏の2つ分について
	for (int i = 0; i < 2; i++)
	{
		bbIndex.at(i) = TexManager::GetOffsetRTV();

		//スワップチェーンからバッファを取得
		result = DirectXImportant::swapchain->GetBuffer(
			TexManager::GetOffsetRTV(),
			IID_PPV_ARGS(&TexManager::GetBuffer()));

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				TexManager::GetCpuHeapStartRTV(),
				TexManager::GetOffsetRTV(),
				incrementSizeRTV);

		//レンダーターゲットビューの生成
		DirectXImportant::dev->CreateRenderTargetView(
			TexManager::GetBuffer().Get(),
			nullptr,
			handle);

		TexManager::AddOffsetSRV();
		TexManager::AddOffsetRTV();
	}
}

void DirectXBase::DepthBuffer()
{
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	//リソース生成
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&TexManager::GetBuffer()));
}

void DirectXBase::DepthStencilView()
{
	incrementSizeDSV = DirectXImportant::dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dsvIndex = TexManager::GetOffsetDSV();

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			TexManager::GetCpuHeapStartDSV(),
			dsvIndex,
			incrementSizeDSV);

	//深度ステンシルビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		TexManager::GetBuffer().Get(),
		&dsvDesc,
		handle);

	TexManager::AddOffsetSRV();
	TexManager::AddOffsetDSV();
}

void DirectXBase::Fence()
{
	//フェンスの生成
	result = DirectXImportant::dev->CreateFence(
		fenceVal,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence));
}
#pragma endregion

#pragma region 描画前
void DirectXBase::SetDrawMode()
{
	currentIndex = bbIndex.at(DirectXImportant::swapchain->GetCurrentBackBufferIndex());
	DirectXImportant::cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			TexManager::GetBuffer(currentIndex).Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
}

void DirectXBase::RenderTarget()
{
	rtvH = TexManager::GetCpuHeapStartRTV();
	rtvH.ptr += currentIndex * incrementSizeRTV;

	dsvH = TexManager::GetCpuHeapStartDSV();
	dsvH.ptr += dsvIndex * incrementSizeDSV;
	DirectXImportant::cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
}

void DirectXBase::ClearScreen(float* ClearColor)
{
	DirectXImportant::cmdList->ClearRenderTargetView(rtvH, ClearColor, 0, nullptr);
	DirectXImportant::cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXBase::Viewports()
{
	/*ビューポートの設定コマンド*/
	DirectXImportant::cmdList->RSSetViewports(
		1,
		&CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			WINDOW_WIDTH,
			WINDOW_HEIGHT
			//min,maxは標準設定
		));
}

void DirectXBase::ScissorRects()
{
	/*シザー短径の設定コマンド*/
	DirectXImportant::cmdList->RSSetScissorRects(
		1,
		&CD3DX12_RECT(
			0,
			0,
			WINDOW_WIDTH,
			WINDOW_HEIGHT
		));
}
#pragma endregion

#pragma region 描画後
void DirectXBase::SetIndicateMode()
{
	DirectXImportant::cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			TexManager::GetBuffer(currentIndex).Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));
}

void DirectXBase::CloseCommandList()
{
	DirectXImportant::cmdList->Close();
}

void DirectXBase::ExecuteCommandList()
{
	ID3D12CommandList* cmdLists[] = { DirectXImportant::cmdList.Get() };//コマンドリストの配列
	DirectXImportant::cmdQueue->ExecuteCommandLists(1, cmdLists);
}

void DirectXBase::WaitForSingle()
{
	DirectXImportant::cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

void DirectXBase::ResetCmdAllocator()
{
	DirectXImportant::cmdAllocator->Reset();
}

void DirectXBase::ResetCommandList()
{
	DirectXImportant::cmdList->Reset(DirectXImportant::cmdAllocator.Get(), nullptr);
}

void DirectXBase::Swap()
{
	DirectXImportant::swapchain->Present(1, 0);
}
#pragma endregion

void DirectXBase::CreateDevice()
{
	DXGIFactory();
	GraphicsAdapter();
	Device();
}

void DirectXBase::Init(HWND hwnd)
{
	CommandAllocator();
	CommandList();
	CommandQueue();
	SwapChain(hwnd);
	RenderTargetView();
	DepthBuffer();
	DepthStencilView();
	Fence();
}

void DirectXBase::BeforeDraw(float* ClearColor)
{
	SetDrawMode();
	RenderTarget();
	ClearScreen(ClearColor);
	Viewports();
	ScissorRects();
}

void DirectXBase::AfterDraw()
{
	SetIndicateMode();
	CloseCommandList();
	ExecuteCommandList();
	WaitForSingle();
	ResetCmdAllocator();
	ResetCommandList();
	Swap();
}
