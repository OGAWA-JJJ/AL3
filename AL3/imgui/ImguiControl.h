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

	///static float swordPos_x;
	///static float swordPos_y;
	///static float swordPos_z;
	//static float Imgui_ground_y;
	//static float Imgui_swordPos_x;
	//static float Imgui_swordPos_y;
	//static float Imgui_swordPos_z;
	//static float Imgui_swordRot_x;
	//static float Imgui_swordRot_y;
	//static float Imgui_swordRot_z;
	//static bool Imgui_isTargetDraw;

	static bool Imgui_isOBBDraw;
	static bool Imgui_isPlayerDraw;
	static bool Imgui_isWeaponDraw;

	static float Imgui_cameraY;

public:
	//InguiControl();
	static void Update();
};

