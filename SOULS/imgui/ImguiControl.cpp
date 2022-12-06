#include "ImguiControl.h"
#include "imgui.h"

float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 3000.0f;
float ImguiControl::Imgui_lightColor_r = 0.4f;
float ImguiControl::Imgui_lightColor_g = 0.4f;
float ImguiControl::Imgui_lightColor_b = 0.4f;
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
char* ImguiControl::Imgui_enemyOldAniType = "NONE";
char* ImguiControl::Imgui_enemyInvi = "FALSE";

float ImguiControl::Imgui_playerBlendTimer = 0.0f;
float ImguiControl::Imgui_playerCurrentAniTimer = 0.0f;
float ImguiControl::Imgui_playerOldAniTimer = 0.0f;
char* ImguiControl::Imgui_playerAniType = "NONE";
char* ImguiControl::Imgui_playerOldAniType = "NONE";
char* ImguiControl::Imgui_playerIsAccept = "NONE";
char* ImguiControl::Imgui_playerIsChange = "NONE";
char* ImguiControl::Imgui_playerIsAttack = "NONE";
char* ImguiControl::Imgui_playerIsInvincible = "NONE";

//obb
float ImguiControl::Imgui_playerOBBPos[10][3];
float ImguiControl::Imgui_enemyOBBPos[12][3];

float ImguiControl::Imgui_playerOBBScale[10][3];
float ImguiControl::Imgui_enemyOBBScale[12][3];

//��
bool ImguiControl::isHel = false;

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
	const ImVec4 text_particleManagerColor = { 0.3f, 0.5f, 0.7f, 1.0f };

	//�J�����֌W
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_cameraColor;
	if (ImGui::TreeNode("CameraSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
		ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);
		ImGui::SliderFloat("CAMERA_DIST", &Imgui_cameraDist, 50.0f, 200.0f);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	//���C�e�B���O�֌W
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_lightColor;
	if (ImGui::TreeNode("LightColorSettings"))
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

	//�`��֌W
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_isDrawColor;
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

	//�G�֌W
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_enemyStatusColor;
	if (ImGui::TreeNode("EnemyStatus"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::Text("ENEMY_BLEND_TIMER          �F %f", Imgui_enemyBlendTimer);
		ImGui::Text("ENEMY_CURRENT_ANI_TIMER    �F %f", Imgui_enemyCurrentAniTimer);
		ImGui::Text("ENEMY_OLD_ANI_TIMER        �F %f", Imgui_enemyOldAniTimer);
		ImGui::Text("ENEMY_ANIMATION_TYPE       �F %s", Imgui_enemyAniType);
		ImGui::Text("ENEMY_OLD_ANIMATION_TYPE   �F %s", Imgui_enemyOldAniType);
		ImGui::Text("ENEMY_INVI                 �F %s", Imgui_enemyInvi);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	//�v���C���[�֌W
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_playerStatusColor;
	if (ImGui::TreeNode("PlayerStatus"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::Text("PLAYER_BLEND_TIMER          �F %f", Imgui_playerBlendTimer);
		ImGui::Text("PLAYER_CURRENT_ANI_TIMER    �F %f", Imgui_playerCurrentAniTimer);
		ImGui::Text("PLAYER_OLD_ANI_TIMER        �F %f", Imgui_playerOldAniTimer);
		ImGui::Text("PLAYER_ANIMATION_TYPE       �F %s", Imgui_playerAniType);
		ImGui::Text("PLAYER_OLD_ANIMATION_TYPE   �F %s", Imgui_playerOldAniType);
		ImGui::Text("PLAYER_IS_ACCEPT            �F %s", Imgui_playerIsAccept);
		ImGui::Text("PLAYER_IS_CHANGE            �F %s", Imgui_playerIsChange);
		ImGui::Text("PLAYER_IS_CHANGE            �F %s", Imgui_playerIsAttack);
		ImGui::Text("PLAYER_IS_INVINCIBLE        �F %s", Imgui_playerIsInvincible);
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

	//�p�[�e�B�N��
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_particleManagerColor;
	if (ImGui::TreeNode("ParticleManager"))
	{
		ImGui::TreePop();
	}

	//���Z�b�g
	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	//��
	ImGui::Checkbox("IS_HELMET", &isHel);
}
