#include "ImguiControl.h"
#include "imgui.h"

//Camera
float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 2500.0f;
float ImguiControl::Imgui_lightColor_r = 0.4f;
float ImguiControl::Imgui_lightColor_g = 0.4f;
float ImguiControl::Imgui_lightColor_b = 0.4f;
float ImguiControl::Imgui_lightDir_x = 0.0f;
float ImguiControl::Imgui_lightDir_y = 0.1f;
float ImguiControl::Imgui_lightDir_z = 0.0f;

//IsDraw
bool ImguiControl::Imgui_isOBBDraw = false;
bool ImguiControl::Imgui_isPlayerDraw = true;
bool ImguiControl::Imgui_isEnemyDraw = true;
bool ImguiControl::Imgui_isWeaponDraw = true;
bool ImguiControl::Imgui_isSponzaDraw = true;

float ImguiControl::Imgui_cameraDist;

//Enemy
float ImguiControl::Imgui_enemyBlendTimer = 0.0f;
float ImguiControl::Imgui_enemyCurrentAniTimer = 0.0f;
float ImguiControl::Imgui_enemyOldAniTimer = 0.0f;
char* ImguiControl::Imgui_enemyAniType = "NONE";
char* ImguiControl::Imgui_enemyOldAniType = "NONE";
char* ImguiControl::Imgui_enemyInvi = "FALSE";

//Player
float ImguiControl::Imgui_playerBlendTimer = 0.0f;
float ImguiControl::Imgui_playerCurrentAniTimer = 0.0f;
float ImguiControl::Imgui_playerOldAniTimer = 0.0f;
char* ImguiControl::Imgui_playerAniType = "NONE";
char* ImguiControl::Imgui_playerOldAniType = "NONE";
char* ImguiControl::Imgui_playerIsAccept = "NONE";
char* ImguiControl::Imgui_playerIsChange = "NONE";
char* ImguiControl::Imgui_playerIsAttack = "NONE";
char* ImguiControl::Imgui_playerIsInvincible = "NONE";
bool ImguiControl::isHel = false;

//obb
float ImguiControl::Imgui_playerOBBPos[10][3];
float ImguiControl::Imgui_enemyOBBPos[12][3];

float ImguiControl::Imgui_playerOBBScale[10][3];
float ImguiControl::Imgui_enemyOBBScale[12][3];

//Shadow
float ImguiControl::Imgui_shadowEye_x = 0.0f;
float ImguiControl::Imgui_shadowEye_y = 0.0f;
float ImguiControl::Imgui_shadowEye_z = 0.0f;
float ImguiControl::Imgui_orthoMag_x = 1.0f;
float ImguiControl::Imgui_orthoMag_y = 1.0f;

//Stage
float ImguiControl::Imgui_stageArea = 500.0f;
float ImguiControl::Imgui_stageScale = 3500.0f;

//GameInit
bool ImguiControl::Imgui_gameInit = false;
bool ImguiControl::Imgui_enemyKill = false;

float ImguiControl::SwordXt = -3.0f;
float ImguiControl::SwordYt = 10.0f;
float ImguiControl::SwordZt = -2.0f;

float ImguiControl::SwordXr = 10.0f;
float ImguiControl::SwordYr = 0.0f;
float ImguiControl::SwordZr = -170.0f;

ImguiControl::ImguiControl()
{
	//Player
	ImguiControl::Imgui_playerOBBPos[2][1] = 2.0f;

	//Enemy
	ImguiControl::Imgui_enemyOBBPos[0][1] = 5.0f;
	ImguiControl::Imgui_enemyOBBPos[3][1] = 5.0f;
	ImguiControl::Imgui_enemyOBBPos[5][1] = 3.0f;
}

void ImguiControl::Update()
{
	ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);

	const ImVec4 text_cameraColor = { 1.0f, 1.0f, 0.3f, 1.0f };
	const ImVec4 text_lightColor = { 0.3f, 1.0f, 1.0f, 1.0f };
	const ImVec4 text_isDrawColor = { 1.0f, 0.3f, 1.0f, 1.0f };
	const ImVec4 text_enemyStatusColor = { 1.0f, 0.3f, 0.3f, 1.0f };
	const ImVec4 text_playerStatusColor = { 0.3f, 0.3f, 1.0f, 1.0f };
	const ImVec4 text_obbColor = { 0.3f, 1.0f, 0.3f, 1.0f };
	const ImVec4 text_shadowSetColor = { 0.3f, 0.5f, 0.7f, 1.0f };
	const ImVec4 text_stageSetColor = { 0.5f, 0.3f, 0.7f, 1.0f };

	//カメラ関係
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_cameraColor;
	if (ImGui::TreeNode("CameraSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
		ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);
		ImGui::SliderFloat("TARGET_DIST", &Imgui_cameraDist, 50.0f, 200.0f);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	//ライティング関係
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_lightColor;
	if (ImGui::TreeNode("LightColorSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
		ImGui::SliderFloat("COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
		ImGui::SliderFloat("COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);

		ImGui::SliderFloat("DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
		ImGui::SliderFloat("DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
		ImGui::SliderFloat("DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	//描画関係
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_isDrawColor;
	if (ImGui::TreeNode("IsDraw"))
	{
		ImGui::Checkbox("OBB_BOX_DRAW", &Imgui_isOBBDraw);
		ImGui::Checkbox("PLAYER_DRAW", &Imgui_isPlayerDraw);
		ImGui::Checkbox("ENEMY_DRAW", &Imgui_isEnemyDraw);
		ImGui::Checkbox("WEAPON_DRAW", &Imgui_isWeaponDraw);
		ImGui::Checkbox("ARENA_DRAW", &Imgui_isSponzaDraw);
		ImGui::Checkbox("IS_HELMET", &isHel);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	//敵関係
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_enemyStatusColor;
	if (ImGui::TreeNode("EnemyStatus"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::Text("BLEND_TIMER          ： %f", Imgui_enemyBlendTimer);
		ImGui::Text("CURRENT_ANI_TIMER    ： %f", Imgui_enemyCurrentAniTimer);
		ImGui::Text("OLD_ANI_TIMER        ： %f", Imgui_enemyOldAniTimer);
		ImGui::Text("ANIMATION_TYPE       ： %s", Imgui_enemyAniType);
		ImGui::Text("OLD_ANIMATION_TYPE   ： %s", Imgui_enemyOldAniType);
		ImGui::Text("INVI                 ： %s", Imgui_enemyInvi);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	//プレイヤー関係
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_playerStatusColor;
	if (ImGui::TreeNode("PlayerStatus"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::Text("BLEND_TIMER          ： %f", Imgui_playerBlendTimer);
		ImGui::Text("CURRENT_ANI_TIMER    ： %f", Imgui_playerCurrentAniTimer);
		ImGui::Text("OLD_ANI_TIMER        ： %f", Imgui_playerOldAniTimer);
		ImGui::Text("ANIMATION_TYPE       ： %s", Imgui_playerAniType);
		ImGui::Text("OLD_ANIMATION_TYPE   ： %s", Imgui_playerOldAniType);
		ImGui::Text("IS_ACCEPT            ： %s", Imgui_playerIsAccept);
		ImGui::Text("IS_CHANGE            ： %s", Imgui_playerIsChange);
		ImGui::Text("IS_CHANGE            ： %s", Imgui_playerIsAttack);
		ImGui::Text("IS_INVINCIBLE        ： %s", Imgui_playerIsInvincible);
		ImGui::TreePop();
	}

	//obb
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_obbColor;
	if (ImGui::TreeNode("OBB"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		if (ImGui::TreeNode("PLAYER_POS"))
		{
			for (int i = 0; i < 10; i++)
			{
				char lavel = i + '0';
				ImGui::SliderFloat3(&lavel, Imgui_playerOBBPos[i], -5.0f, 5.0f);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("PLAYER_SCALE"))
		{
			for (int i = 0; i < 10; i++)
			{
				char lavel = i + 10 + '0';
				ImGui::SliderFloat3(&lavel, Imgui_playerOBBScale[i], -10.0f, 10.0f);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ENEMY_POS"))
		{
			for (int i = 0; i < 10; i++)
			{
				char lavel = i + 20 + '0';
				ImGui::SliderFloat3(&lavel, Imgui_enemyOBBPos[i], -10.0f, 10.0f);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ENEMY_SCALE"))
		{
			for (int i = 0; i < 10; i++)
			{
				char lavel = i + 30 + '0';
				ImGui::SliderFloat3(&lavel, Imgui_enemyOBBScale[i], -25.0f, 25.0f);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//影
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_shadowSetColor;
	if (ImGui::TreeNode("ShadowSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::DragFloat("EYE_X", &Imgui_shadowEye_x);
		ImGui::DragFloat("EYE_Y", &Imgui_shadowEye_y);
		ImGui::DragFloat("EYE_Z", &Imgui_shadowEye_z);
		ImGui::DragFloat("ORTHO_MAG_X", &Imgui_orthoMag_x, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("ORTHO_MAG_Y", &Imgui_orthoMag_y, 0.01f, 0.01f, 10.0f);
		ImGui::TreePop();
	}

	//ステージ
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_stageSetColor;
	if (ImGui::TreeNode("StageSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::DragFloat("AREA", &Imgui_stageArea, 5.0f);
		ImGui::DragFloat("SCALE", &Imgui_stageScale, 5.0f);
		ImGui::TreePop();
	}

	//リセット
	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::Checkbox("GAME_INIT", &Imgui_gameInit);
	ImGui::Checkbox("ENEMY_KILL", &Imgui_enemyKill);

	ImGui::DragFloat("SWORD Xt", &SwordXt);
	ImGui::DragFloat("SWORD Yt", &SwordYt);
	ImGui::DragFloat("SWORD Zt", &SwordZt);

	ImGui::DragFloat("SWORD Xr", &SwordXr);
	ImGui::DragFloat("SWORD Yr", &SwordYr);
	ImGui::DragFloat("SWORD Zr", &SwordZr);
}
