#include "ImguiControl.h"
#include "imgui.h"

float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 3000.0f;
float ImguiControl::Imgui_lightColor_r = 0.6f;
float ImguiControl::Imgui_lightColor_g = 0.6f;
float ImguiControl::Imgui_lightColor_b = 0.6f;
float ImguiControl::Imgui_lightDir_x = 0.0f;
float ImguiControl::Imgui_lightDir_y = 0.1f;
float ImguiControl::Imgui_lightDir_z = 0.0f;

bool ImguiControl::Imgui_isOBBDraw = true;
bool ImguiControl::Imgui_isPlayerDraw = true;
bool ImguiControl::Imgui_isWeaponDraw = true;
bool ImguiControl::Imgui_isSponzaDraw = true;

char* ImguiControl::Imgui_AniType = "NONE";

void ImguiControl::Update()
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "CameraSettings");
	ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
	ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);

	ImGui::Spacing();

	ImGui::TextColored(ImVec4(0.3f, 1.0f, 1.0f, 1.0f), "ColorSettings");
	ImGui::SliderFloat("LIGHT_COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);

	ImGui::SliderFloat("LIGHT_DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);

	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 0.3f, 1.0f, 1.0f), "isDraw");
	ImGui::Checkbox("OBB_BOX_DRAW", &Imgui_isOBBDraw);
	ImGui::Checkbox("PLAYER_DRAW", &Imgui_isPlayerDraw);
	ImGui::Checkbox("WEAPON_DRAW", &Imgui_isWeaponDraw);
	ImGui::Checkbox("SPONZA_DRAW", &Imgui_isSponzaDraw);

	ImGui::Spacing();

	ImGui::Text("ENEMY_ANIMATION_TYPE ÅF %s", Imgui_AniType);
}
