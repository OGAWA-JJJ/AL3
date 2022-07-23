#pragma once

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

public:
	//InguiControl();
	static void Update();
};

