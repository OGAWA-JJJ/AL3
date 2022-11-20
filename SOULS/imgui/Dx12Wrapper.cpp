#include "Dx12Wrapper.h"
#include "ImguiControl.h"
#include "../SourceCodes/DirectX/DirectXImportant.h"

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::_heapForImgui;
bool Dx12Wrapper::bInResult;

Dx12Wrapper::Dx12Wrapper()
{
}

Dx12Wrapper::~Dx12Wrapper()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX12_Shutdown();
}

void Dx12Wrapper::ImguiInit()
{
	_heapForImgui = CreateDescriptorHeapForImgui();

	if (_heapForImgui == nullptr) {
		assert(0);
	}

	if (ImGui::CreateContext() == nullptr) {
		assert(0);
	}
	//[ImGui::]
}

void Dx12Wrapper::WindowsInit(HWND hwnd)
{
	bInResult = ImGui_ImplWin32_Init(hwnd);
	if (!bInResult) {
		assert(0);
	}
}

void Dx12Wrapper::DirectXInit()
{
	bInResult = ImGui_ImplDX12_Init(
		DirectXImportant::dev.Get(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		GetHeapForImgui().Get(),
		GetHeapForImgui()->GetCPUDescriptorHandleForHeapStart(),
		GetHeapForImgui()->GetGPUDescriptorHandleForHeapStart());
}

void Dx12Wrapper::Draw(bool isDraw)
{
	if (!isDraw) {
		return;
	}

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//static bool blnChk = false;
	//ImGui::Checkbox("CheakboxTest", &blnChk);

	//static int radio = 0;
	//ImGui::RadioButton("Radio 1", &radio, 0);
	//ImGui::SameLine();
	//ImGui::RadioButton("Radio 2", &radio, 1);
	//ImGui::SameLine();
	//ImGui::RadioButton("Radio 3", &radio, 2);

	//static int nSlider = 0;
	//ImGui::SliderInt("Int Slider", &nSlider, 0, 100);

	//static float fSlider = 0.0f;
	//ImGui::SliderFloat("Float Slider", &fSlider, 0.0f, 100.0f);

	//static float col3[3] = {};
	//ImGui::ColorPicker3("ColorPicker3", col3,
		//ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayRGB);

	//static float col4[4] = {};
	//ImGui::ColorPicker4("ColorPicker4", col4,
		//ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

	ImGui::Begin("Settings");
	ImGui::SetWindowSize(
		ImVec2(
			400,
			600),
		ImGuiCond_::ImGuiCond_FirstUseEver);

	ImguiControl::Update();

	ImGui::End();

	ImGui::Render();
	DirectXImportant::cmdList->SetDescriptorHeaps(
		1,
		GetHeapForImgui().GetAddressOf());

	ImGui_ImplDX12_RenderDrawData(
		ImGui::GetDrawData(),
		DirectXImportant::cmdList.Get());
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::CreateDescriptorHeapForImgui()
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret;
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DirectXImportant::dev->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));

	return ret;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Dx12Wrapper::GetHeapForImgui()
{
	return _heapForImgui;
}
