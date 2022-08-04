#include "ImguiControl.h"
#include "imgui.h"
#include "../SourceCodes/DirectX/DirectXImportant.h"

float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 2000.0f;
float ImguiControl::Imgui_lightColor_r = 0.6f;
float ImguiControl::Imgui_lightColor_g = 0.6f;
float ImguiControl::Imgui_lightColor_b = 0.6f;
float ImguiControl::Imgui_lightDir_x = 0.0f;
float ImguiControl::Imgui_lightDir_y = 0.1f;
float ImguiControl::Imgui_lightDir_z = -0.1f;

bool ImguiControl::Imgui_targetDraw = false;
bool ImguiControl::Imgui_isTargetMove = true;
bool ImguiControl::Imgui_isOBBDraw = true;
bool ImguiControl::Imgui_isHitStopAnimation = false;
bool ImguiControl::Imgui_playerDraw = true;

float ImguiControl::boxPos1_x = 50.0f;
float ImguiControl::boxPos1_y = 25.0f;
float ImguiControl::boxPos1_z = 0.0f;

float ImguiControl::boxRot1_x = -50.0f;
float ImguiControl::boxRot1_y = 25.0f;
float ImguiControl::boxRot1_z = 0.0f;

float ImguiControl::boxSca1_x = 10.0f;
float ImguiControl::boxSca1_y = 20.0f;
float ImguiControl::boxSca1_z = 10.0f;

bool ImguiControl::Imgui_isMonochromatic = false;
bool ImguiControl::Imgui_isPhong = true;
bool ImguiControl::Imgui_isToon = false;
bool ImguiControl::Imgui_isGaussian = false;
bool ImguiControl::Imgui_isBloom = true;
bool ImguiControl::Imgui_isShadowMap = true;

int ImguiControl::texnum = 1;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ImguiControl::l_descHeap;
UINT64 ImguiControl::address;

int ImguiControl::width = 64;
int ImguiControl::height = 64;

UINT64 ImguiControl::LoadTexture(const wchar_t* filename)
{
	HRESULT result;

	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename, DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg);
	if (FAILED(result)) { assert(0); }

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);

	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	ID3D12Resource* texbuff;
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));
	if (FAILED(result)) { assert(0); }

	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);
	if (FAILED(result)) { assert(0); }

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texbuff->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	DirectXImportant::dev->CreateShaderResourceView(
		texbuff,
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			l_descHeap->GetCPUDescriptorHandleForHeapStart(),
			texnum,
			DirectXImportant::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV))
	);

	//return
	CD3DX12_GPU_DESCRIPTOR_HANDLE m_address(
		l_descHeap->GetGPUDescriptorHandleForHeapStart(),
		texnum,
		DirectXImportant::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	texnum++;

	return m_address.ptr;
}

void ImguiControl::Init()
{
	address = LoadTexture(L"Resources/hamurabyss.png");
}

void ImguiControl::Update()
{
	if (ImGui::CollapsingHeader("CameraSettings"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera");
		ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
		ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);
	}

	if (ImGui::CollapsingHeader("LightSettings"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Light");

		ImGui::SliderFloat("LIGHT_DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);

		ImGui::SliderFloat("LIGHT_COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);
		ImGui::Spacing();
	}


	ImGui::Text("GPU handle = %p", address);
	ImGui::DragInt("width", &width);
	ImGui::DragInt("height", &height);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SIZE = %d x %d", width, height);
	ImGui::Separator();
	ImGui::Image((ImTextureID)address, ImVec2(width, height));
}
