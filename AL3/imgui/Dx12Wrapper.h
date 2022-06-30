#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"
#include <wrl.h>

class Dx12Wrapper {
private:
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _heapForImgui;
	static bool bInResult;

public:
	Dx12Wrapper();
	static void ImguiInit();
	static void WindowsInit(HWND hwnd);
	static void DirectXInit();
	static void Draw(bool isDraw = true);

private:
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeapForImgui();
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeapForImgui();
};

