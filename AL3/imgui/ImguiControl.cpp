#include "ImguiControl.h"
#include "imgui.h"

float ImguiControl::Imgui_eye_x = 0.0f;
float ImguiControl::Imgui_eye_y = 50.0f;
float ImguiControl::Imgui_eye_z = -100.0f;
float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 4000.0f;
float ImguiControl::Imgui_lightColor_r = 0.6f;
float ImguiControl::Imgui_lightColor_g = 0.6f;
float ImguiControl::Imgui_lightColor_b = 0.6f;
float ImguiControl::Imgui_lightDir_x = 0.0f;
float ImguiControl::Imgui_lightDir_y = -1.0f;
float ImguiControl::Imgui_lightDir_z = 0.0f;
float ImguiControl::Imgui_ground_y = 0.0f;

float ImguiControl::Imgui_Sword_x = 0.0f;
float ImguiControl::Imgui_Sword_y = 0.0f;
float ImguiControl::Imgui_Sword_z = 0.0f;

float ImguiControl::Imgui_Sword_rotx = 310.0f;
float ImguiControl::Imgui_Sword_roty = 300.0f;
float ImguiControl::Imgui_Sword_rotz = 310.0f;

void ImguiControl::Update()
{
	ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
	ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);

	ImGui::SliderFloat("LIGHT_COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);

	ImGui::SliderFloat("LIGHT_DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);

	ImGui::SliderFloat("GROUND_Y", &Imgui_ground_y, -100.0f, 100.0f);

	ImGui::SliderFloat("Sword_X", &Imgui_Sword_x, -100.0f, 100.0f);
	ImGui::SliderFloat("Sword_Y", &Imgui_Sword_y, -100.0f, 100.0f);
	ImGui::SliderFloat("Sword_Z", &Imgui_Sword_z, -100.0f, 100.0f);

	ImGui::SliderFloat("Sword_rotX", &Imgui_Sword_rotx, 0.0f, 360.0f);
	ImGui::SliderFloat("Sword_rotY", &Imgui_Sword_roty, 0.0f, 360.0f);
	ImGui::SliderFloat("Sword_rotZ", &Imgui_Sword_rotz, 0.0f, 360.0f);
}
