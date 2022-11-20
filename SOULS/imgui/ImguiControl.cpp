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

bool ImguiControl::Imgui_isOBBDraw = false;
bool ImguiControl::Imgui_isPlayerDraw = true;
bool ImguiControl::Imgui_isEnemyDraw = true;
bool ImguiControl::Imgui_isWeaponDraw = true;
bool ImguiControl::Imgui_isSponzaDraw = true;

float ImguiControl::Imgui_cameraDist;

float ImguiControl::Imgui_enemyBlendTimer = 0.0f;
float ImguiControl::Imgui_enemyCurrentAniTimer = 0.0f;
float ImguiControl::Imgui_enemyOldAniTimer = 0.0f;
char* ImguiControl::Imgui_enemyAniType = "NONE";

float ImguiControl::Imgui_playerBlendTimer = 0.0f;
float ImguiControl::Imgui_playerCurrentAniTimer = 0.0f;
float ImguiControl::Imgui_playerOldAniTimer = 0.0f;
char* ImguiControl::Imgui_playerAniType = "NONE";
char* ImguiControl::Imgui_playerOldAniType = "NONE";
char* ImguiControl::Imgui_playerIsAccept = "NONE";
char* ImguiControl::Imgui_playerIsChange = "NONE";

void ImguiControl::Update()
{
	ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 0.3f, 1.0f);
	if (ImGui::TreeNode("CameraSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
		ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);
		ImGui::SliderFloat("CAMERA_DIST", &Imgui_cameraDist, 50.0f, 200.0f);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.3f, 1.0f, 1.0f, 1.0f);
	if (ImGui::TreeNode("ColorSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);

		ImGui::SliderFloat("LIGHT_DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.3f, 1.0f, 1.0f);
	if (ImGui::TreeNode("IsDraw"))
	{
		ImGui::Checkbox("OBB_BOX_DRAW", &Imgui_isOBBDraw);
		ImGui::Checkbox("PLAYER_DRAW", &Imgui_isPlayerDraw);
		ImGui::Checkbox("ENEMY_DRAW", &Imgui_isEnemyDraw);
		ImGui::Checkbox("WEAPON_DRAW", &Imgui_isWeaponDraw);
		ImGui::Checkbox("SPONZA_DRAW", &Imgui_isSponzaDraw);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);
	if (ImGui::TreeNode("EnemyStatus"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::Text("ENEMY_BLEND_TIMER          ： %f", Imgui_enemyBlendTimer);
		ImGui::Text("ENEMY_CURRENT_ANI_TIMER    ： %f", Imgui_enemyCurrentAniTimer);
		ImGui::Text("ENEMY_OLD_ANI_TIMER        ： %f", Imgui_enemyOldAniTimer);
		ImGui::Text("ENEMY_ANIMATION_TYPE       ： %s", Imgui_enemyAniType);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.1f, 0.1f, 1.0f, 1.0f);
	if (ImGui::TreeNode("PlayerStatus"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::Text("PLAYER_BLEND_TIMER          ： %f", Imgui_playerBlendTimer);
		ImGui::Text("PLAYER_CURRENT_ANI_TIMER    ： %f", Imgui_playerCurrentAniTimer);
		ImGui::Text("PLAYER_OLD_ANI_TIMER        ： %f", Imgui_playerOldAniTimer);
		ImGui::Text("PLAYER_ANIMATION_TYPE       ： %s", Imgui_playerAniType);
		ImGui::Text("PLAYER_OLD_ANIMATION_TYPE   ： %s", Imgui_playerOldAniType);
		ImGui::Text("PLAYER_IS_ACCEPT            ： %s", Imgui_playerIsAccept);
		ImGui::Text("PLAYER_IS_CHANGE            ： %s", Imgui_playerIsChange);
		ImGui::TreePop();
	}

	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}
