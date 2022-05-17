#pragma once

class ImguiControl
{
public:
	static float Imgui_eye_x;
	static float Imgui_eye_y;
	static float Imgui_eye_z;
	static float Imgui_fov;
	static float Imgui_far_z;
	static float Imgui_backColor_r;
	static float Imgui_backColor_g;
	static float Imgui_backColor_b;
	//static int Imgui_windowSize_x;
	//static int Imgui_windowSize_y;
	static float Imgui_lightColor_r;
	static float Imgui_lightColor_g;
	static float Imgui_lightColor_b;
	static float Imgui_lightDir_x;
	static float Imgui_lightDir_y;
	static float Imgui_lightDir_z;
	static float Imgui_ground_y;

public:
	//InguiControl();
	static void Update();
};

