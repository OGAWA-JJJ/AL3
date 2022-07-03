#include "ImguiControl.h"
#include "imgui.h"

float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 2000.0f;
float ImguiControl::Imgui_lightColor_r = 0.6f;
float ImguiControl::Imgui_lightColor_g = 0.6f;
float ImguiControl::Imgui_lightColor_b = 0.6f;
float ImguiControl::Imgui_lightDir_x = 0.0f;
float ImguiControl::Imgui_lightDir_y = 0.1f;
float ImguiControl::Imgui_lightDir_z = 0.0f;
float ImguiControl::Imgui_ground_y = 0.0f;

float ImguiControl::Imgui_swordPos_x = 0.0f;
float ImguiControl::Imgui_swordPos_y = 0.0f;
float ImguiControl::Imgui_swordPos_z = 0.0f;

float ImguiControl::Imgui_swordRot_x = 310.0f;
float ImguiControl::Imgui_swordRot_y = 300.0f;
float ImguiControl::Imgui_swordRot_z = 310.0f;

bool ImguiControl::Imgui_targetDraw = false;

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

	ImGui::SliderFloat("SWORD_POS_X", &Imgui_swordPos_x, -100.0f, 100.0f);
	ImGui::SliderFloat("WORD_POS_Y", &Imgui_swordPos_y, -100.0f, 100.0f);
	ImGui::SliderFloat("SWORD_POS_Z", &Imgui_swordPos_z, -100.0f, 100.0f);

	ImGui::SliderFloat("SWORD_ROT_X", &Imgui_swordRot_x, 0.0f, 360.0f);
	ImGui::SliderFloat("SWORD_ROT_Y", &Imgui_swordRot_y, 0.0f, 360.0f);
	ImGui::SliderFloat("SWORD_ROT_Z", &Imgui_swordRot_z, 0.0f, 360.0f);

	ImGui::Checkbox("TARGET_DRAW", &Imgui_targetDraw);
}
