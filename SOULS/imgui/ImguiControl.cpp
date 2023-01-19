#include "ImguiControl.h"
#include "imgui.h"

//Camera
float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 2500.0f;
float ImguiControl::Imgui_eye_x = 0.0f;
float ImguiControl::Imgui_eye_y = 0.0f;
float ImguiControl::Imgui_eye_z = 0.0f;
float ImguiControl::Imgui_target_x = 0.0f;
float ImguiControl::Imgui_target_y = 0.0f;
float ImguiControl::Imgui_target_z = 0.0f;

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

int ImguiControl::C_KICK_COLLISION_TIMER;
int ImguiControl::C_KICK_COLLISION_ENDTIMER;

int ImguiControl::C_PUNCH_COLLISION_TIMER;
int ImguiControl::C_PUNCH_COLLISION_ENDTIMER;

int ImguiControl::C_BACK_COLLISION_TIMER;
int ImguiControl::C_BACK_COLLISION_ENDTIMER;

int ImguiControl::C_EXPLOSION_COLLISION_TIMER;
int ImguiControl::C_EXPLOSION_COLLISION_DELAY;
int ImguiControl::C_EXPLOSION_COLLISION_ENDTIMER;

int ImguiControl::C_SWIP_COLLISION_TIMER;
int ImguiControl::C_SWIP_COLLISION_ENDTIMER;

int ImguiControl::C_RAZER_COLLISION_STARTTIMER;
int ImguiControl::C_RAZER_COLLISION_TIMER;
int ImguiControl::C_RAZER_COLLISION_DELAY;

int ImguiControl::C_SWINGDOWN_COLLISION_TIMER;
int ImguiControl::C_SWINGDOWN_COLLISION_ENDTIMER;

float ImguiControl::C_MAX_DIST;
float ImguiControl::C_MAX_TURN_RAD;
float ImguiControl::C_MAX_BACK_RAD;
float ImguiControl::C_MAX_BLEND_TIMER;

int ImguiControl::C_MAX_POWER;
int ImguiControl::C_MAX_EXPLOSION_POWER;
int ImguiControl::C_MAX_HP;

int ImguiControl::C_RISE_TIMER;
int ImguiControl::C_SWING_DOWN_TIMER;
float ImguiControl::C_MAX_RISE_HEIGHT;
float ImguiControl::C_MAX_MOVE_SPEED;
float ImguiControl::C_MAX_TURN_TIMER;
float ImguiControl::C_MAX_RISE_TIMER;
float ImguiControl::C_MAX_SWING_DOWN_TIMER;

int ImguiControl::C_MAX_TACKLE_TIMER;
int ImguiControl::C_MAX_TACKLE_COUNT;
float ImguiControl::C_MAX_TACKLE_SPEED;
float ImguiControl::C_MAX_TACKLE_RANGE;
float ImguiControl::C_CALC_TACKLE_RANGE;
float ImguiControl::c_beforeBattleAddTimer;

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
bool ImguiControl::Imgui_export = false;
bool ImguiControl::Imgui_inport = false;
bool ImguiControl::Imgui_addMenbers = false;

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
	const ImVec4 text_cameraColor = { 1.0f, 1.0f, 0.3f, 1.0f };
	const ImVec4 text_lightColor = { 0.3f, 1.0f, 1.0f, 1.0f };
	const ImVec4 text_isDrawColor = { 1.0f, 0.3f, 1.0f, 1.0f };
	const ImVec4 text_enemyStatusColor = { 1.0f, 0.3f, 0.3f, 1.0f };
	const ImVec4 text_playerStatusColor = { 0.3f, 0.3f, 1.0f, 1.0f };
	const ImVec4 text_obbColor = { 0.3f, 1.0f, 0.3f, 1.0f };
	const ImVec4 text_shadowSetColor = { 0.3f, 0.5f, 0.7f, 1.0f };
	const ImVec4 text_stageSetColor = { 0.5f, 0.3f, 0.7f, 1.0f };
	ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);

	/*-----日本語追加時はu8を指定-----*/

	//カメラ関係
	ImGui::GetStyle().Colors[ImGuiCol_Text] = text_cameraColor;
	if (ImGui::TreeNode("CameraSettings"))
	{
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
		ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);
		ImGui::SliderFloat("TARGET_DIST", &Imgui_cameraDist, 50.0f, 200.0f);

		ImGui::DragFloat("EYE_X", &Imgui_eye_x);
		ImGui::DragFloat("EYE_Y", &Imgui_eye_y);
		ImGui::DragFloat("EYE_Z", &Imgui_eye_z);

		ImGui::DragFloat("TARGET_X", &Imgui_target_x);
		ImGui::DragFloat("TARGET_Y", &Imgui_target_y);
		ImGui::DragFloat("TARGET_Z", &Imgui_target_z);
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

		ImGui::Spacing();
		ImGui::DragInt("C_KICK_COLLISION_TIMER", &C_KICK_COLLISION_TIMER);
		ImGui::DragInt("C_KICK_COLLISION_ENDTIMER", &C_KICK_COLLISION_ENDTIMER);

		ImGui::DragInt("C_PUNCH_COLLISION_TIMER", &C_PUNCH_COLLISION_TIMER);
		ImGui::DragInt("C_PUNCH_COLLISION_ENDTIMER", &C_PUNCH_COLLISION_ENDTIMER);

		ImGui::DragInt("C_BACK_COLLISION_TIMER", &C_BACK_COLLISION_TIMER);
		ImGui::DragInt("C_BACK_COLLISION_ENDTIMER", &C_BACK_COLLISION_ENDTIMER);

		ImGui::DragInt("C_EXPLOSION_COLLISION_TIMER", &C_EXPLOSION_COLLISION_TIMER);
		ImGui::DragInt("C_EXPLOSION_COLLISION_DELAY", &C_EXPLOSION_COLLISION_DELAY);
		ImGui::DragInt("C_EXPLOSION_COLLISION_ENDTIMER", &C_EXPLOSION_COLLISION_ENDTIMER);

		ImGui::DragInt("C_SWIP_COLLISION_TIMER", &C_SWIP_COLLISION_TIMER);
		ImGui::DragInt("C_SWIP_COLLISION_ENDTIMER", &C_SWIP_COLLISION_ENDTIMER);

		ImGui::DragInt("C_RAZER_COLLISION_STARTTIMER", &C_RAZER_COLLISION_STARTTIMER);
		ImGui::DragInt("C_RAZER_COLLISION_TIMER", &C_RAZER_COLLISION_TIMER);
		ImGui::DragInt("C_RAZER_COLLISION_DELAY", &C_RAZER_COLLISION_DELAY);

		ImGui::DragInt("C_SWINGDOWN_COLLISION_TIMER", &C_SWINGDOWN_COLLISION_TIMER);
		ImGui::DragInt("C_SWINGDOWN_COLLISION_ENDTIMER", &C_SWINGDOWN_COLLISION_ENDTIMER);

		ImGui::DragFloat("C_MAX_DIST", &C_MAX_DIST);
		ImGui::DragFloat("C_MAX_TURN_RAD", &C_MAX_TURN_RAD);
		ImGui::DragFloat("C_MAX_BACK_RAD", &C_MAX_BACK_RAD);
		ImGui::DragFloat("C_MAX_BLEND_TIMER", &C_MAX_BLEND_TIMER);

		ImGui::DragInt("C_MAX_POWER", &C_SWINGDOWN_COLLISION_TIMER);
		ImGui::DragInt("C_MAX_EXPLOSION_POWER", &C_SWINGDOWN_COLLISION_TIMER);
		ImGui::DragInt("C_MAX_HP", &C_SWINGDOWN_COLLISION_TIMER);

		ImGui::DragInt("C_RISE_TIMER", &C_RISE_TIMER);
		ImGui::DragInt("C_SWING_DOWN_TIMER", &C_SWING_DOWN_TIMER);
		ImGui::DragFloat("C_MAX_RISE_HEIGHT", &C_MAX_RISE_HEIGHT);
		ImGui::DragFloat("C_MAX_MOVE_SPEED", &C_MAX_MOVE_SPEED);
		ImGui::DragFloat("C_MAX_TURN_TIMER", &C_MAX_TURN_TIMER);
		ImGui::DragFloat("C_MAX_RISE_TIMER", &C_MAX_RISE_TIMER);
		ImGui::DragFloat("C_MAX_SWING_DOWN_TIMER", &C_MAX_SWING_DOWN_TIMER);

		ImGui::DragInt("C_MAX_TACKLE_TIMER", &C_MAX_TACKLE_TIMER);
		ImGui::DragInt("C_MAX_TACKLE_COUNT", &C_MAX_TACKLE_COUNT);
		ImGui::DragFloat("C_MAX_TACKLE_SPEED", &C_MAX_TACKLE_SPEED);
		ImGui::DragFloat("C_MAX_TACKLE_RANGE", &C_MAX_TACKLE_RANGE);
		ImGui::DragFloat("C_CALC_TACKLE_RANGE", &C_CALC_TACKLE_RANGE);
		ImGui::DragFloat("c_beforeBattleAddTimer", &c_beforeBattleAddTimer);
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
			ImGui::SliderFloat3(u8"自機　体　　座標", Imgui_playerOBBPos[0], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　右手　座標", Imgui_playerOBBPos[1], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　頭　　座標", Imgui_playerOBBPos[2], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　右肩　座標", Imgui_playerOBBPos[3], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　左肩　座標", Imgui_playerOBBPos[4], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　左手　座標", Imgui_playerOBBPos[5], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　右膝　座標", Imgui_playerOBBPos[6], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　右足　座標", Imgui_playerOBBPos[7], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　左膝　座標", Imgui_playerOBBPos[8], -5.0f, 5.0f);
			ImGui::SliderFloat3(u8"自機　左足　座標", Imgui_playerOBBPos[9], -5.0f, 5.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("PLAYER_SCALE"))
		{
			ImGui::SliderFloat3(u8"自機　体　　スケール", Imgui_playerOBBScale[0], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　右手　スケール", Imgui_playerOBBScale[1], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　頭　　スケール", Imgui_playerOBBScale[2], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　右肩　スケール", Imgui_playerOBBScale[3], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　左肩　スケール", Imgui_playerOBBScale[4], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　左手　スケール", Imgui_playerOBBScale[5], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　右膝　スケール", Imgui_playerOBBScale[6], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　右足　スケール", Imgui_playerOBBScale[7], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　左膝　スケール", Imgui_playerOBBScale[8], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"自機　左足　スケール", Imgui_playerOBBScale[9], -10.0f, 10.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ENEMY_POS"))
		{
			ImGui::SliderFloat3(u8"敵　体　　座標", Imgui_enemyOBBPos[0], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　頭　　座標", Imgui_enemyOBBPos[1], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　右肩　座標", Imgui_enemyOBBPos[2], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　右手　座標", Imgui_enemyOBBPos[3], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　左肩　座標", Imgui_enemyOBBPos[4], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　左手　座標", Imgui_enemyOBBPos[5], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　右膝　座標", Imgui_enemyOBBPos[6], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　右足　座標", Imgui_enemyOBBPos[7], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　左膝　座標", Imgui_enemyOBBPos[8], -10.0f, 10.0f);
			ImGui::SliderFloat3(u8"敵　左足　座標", Imgui_enemyOBBPos[9], -10.0f, 10.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ENEMY_SCALE"))
		{
			ImGui::SliderFloat3(u8"敵　体　　スケール", Imgui_enemyOBBScale[0], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　頭　　スケール", Imgui_enemyOBBScale[1], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　右肩　スケール", Imgui_enemyOBBScale[2], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　右手　スケール", Imgui_enemyOBBScale[3], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　左肩　スケール", Imgui_enemyOBBScale[4], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　左手　スケール", Imgui_enemyOBBScale[5], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　右膝　スケール", Imgui_enemyOBBScale[6], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　右足　スケール", Imgui_enemyOBBScale[7], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　左膝　スケール", Imgui_enemyOBBScale[8], -25.0f, 25.0f);
			ImGui::SliderFloat3(u8"敵　左足　スケール", Imgui_enemyOBBScale[9], -25.0f, 25.0f);
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

	Imgui_gameInit = ImGui::Button("GAME_INIT");
	Imgui_enemyKill = ImGui::Button("ENEMY_KILL");
	Imgui_export = ImGui::Button("EXPORT");
	Imgui_inport = ImGui::Button("INPORT");
	Imgui_addMenbers = ImGui::Button("ADD_MENBERS");
}
