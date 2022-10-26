#pragma once
#include <d3dx12.h> 
#include <DirectXTex.h>
#include <unordered_map>

class ImguiControl
{
public:
	static float Imgui_fov;
	static float Imgui_far_z;
	static float Imgui_lightColor_r;
	static float Imgui_lightColor_g;
	static float Imgui_lightColor_b;
	static float Imgui_lightDir_x;
	static float Imgui_lightDir_y;
	static float Imgui_lightDir_z;

	static bool Imgui_targetDraw;
	static bool Imgui_isTargetMove;
	static bool Imgui_isOBBDraw;
	static bool Imgui_isHitStopAnimation;
	static bool Imgui_playerDraw;

	static float boxPos1_x;
	static float boxPos1_y;
	static float boxPos1_z;

	static float boxRot1_x;
	static float boxRot1_y;
	static float boxRot1_z;

	static float boxSca1_x;
	static float boxSca1_y;
	static float boxSca1_z;

	static bool Imgui_isMonochromatic;
	static bool Imgui_isPhong;
	static bool Imgui_isToon;
	static bool Imgui_isGaussian;
	static bool Imgui_isBloom;
	static bool Imgui_isShadowMap;

	static bool Imgui_isFastRun;

public:
	static const int MAX_TEX_NUM = 10;

private:
	static int texnum;
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> l_descHeap;
	static UINT64 address;

	static int width;
	static int height;

public:
	~ImguiControl() {};
	static UINT64 LoadTexture(const wchar_t* filename);

	static void SetDescHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap) { l_descHeap = descHeap; }

	static void Init();
	static void Update();
};

