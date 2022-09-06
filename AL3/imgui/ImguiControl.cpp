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

float ImguiControl::boxPos1_x = 50.0f;
float ImguiControl::boxPos1_y = 25.0f;
float ImguiControl::boxPos1_z = 0.0f;

float ImguiControl::boxRot1_x = -50.0f;
float ImguiControl::boxRot1_y = 25.0f;
float ImguiControl::boxRot1_z = 0.0f;

float ImguiControl::boxSca1_x = 10.0f;
float ImguiControl::boxSca1_y = 20.0f;
float ImguiControl::boxSca1_z = 10.0f;

//float ImguiControl::swordPos_x = 0.0f;
//float ImguiControl::swordPos_y = 6.0f;
//float ImguiControl::swordPos_z = 0.0f;
// 
//float ImguiControl::Imgui_ground_y = 0.0f;

//float ImguiControl::Imgui_swordPos_x = 0.0f;
//float ImguiControl::Imgui_swordPos_y = 0.0f;
//float ImguiControl::Imgui_swordPos_z = 0.0f;

//float ImguiControl::Imgui_swordRot_x = 310.0f;
//float ImguiControl::Imgui_swordRot_y = 300.0f;
//float ImguiControl::Imgui_swordRot_z = 310.0f;

bool ImguiControl::Imgui_targetDraw = false;
bool ImguiControl::Imgui_isTargetMove = true;
bool ImguiControl::Imgui_isOBBDraw = true;
bool ImguiControl::Imgui_isHitStopAnimation = false;
bool ImguiControl::Imgui_playerDraw = true;

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

	//ImGui::SliderFloat("GROUND_Y", &Imgui_ground_y, -100.0f, 100.0f);

	//ImGui::SliderFloat("SWORD_POS_X", &Imgui_swordPos_x, -100.0f, 100.0f);
	//ImGui::SliderFloat("SWORD_POS_Y", &Imgui_swordPos_y, -100.0f, 100.0f);
	//ImGui::SliderFloat("SWORD_POS_Z", &Imgui_swordPos_z, -100.0f, 100.0f);

	//ImGui::SliderFloat("SWORD_ROT_X", &Imgui_swordRot_x, 0.0f, 360.0f);
	//ImGui::SliderFloat("SWORD_ROT_Y", &Imgui_swordRot_y, 0.0f, 360.0f);
	//ImGui::SliderFloat("SWORD_ROT_Z", &Imgui_swordRot_z, 0.0f, 360.0f);

	//ImGui::Checkbox("CAMERA_TARGET_DRAW(Click or Y Button)", &Imgui_targetDraw);
	//ImGui::Checkbox("ENEMY_MOVE(Click or X Button)", &Imgui_isTargetMove);
	//ImGui::Checkbox("HITSTOP_MODE(LSTICK PUSH->REPLAY)", &Imgui_isHitStopAnimation);
	ImGui::Checkbox("OBB_BOX_DRAW", &Imgui_isOBBDraw);
	ImGui::Checkbox("PLAYER_DRAW", &Imgui_playerDraw);

	//ImGui::SliderFloat("boxPos_x", &boxPos1_x, -100.0f, 100.0f);
	//ImGui::SliderFloat("boxPos_y", &boxPos1_y, -100.0f, 100.0f);
	//ImGui::SliderFloat("boxPos_z", &boxPos1_z, -100.0f, 100.0f);

	//ImGui::SliderFloat("boxRot_x", &boxRot1_x, 0.0f, 360.0f);
	//ImGui::SliderFloat("boxRot_y", &boxRot1_y, 0.0f, 360.0f);
	//ImGui::SliderFloat("boxRot_z", &boxRot1_z, 0.0f, 360.0f);

	//ImGui::SliderFloat("boxSca_x", &boxSca1_x, 1.0f, 30.0f);
	//ImGui::SliderFloat("boxSca_y", &boxSca1_y, 1.0f, 30.0f);
	//ImGui::SliderFloat("boxSca_z", &boxSca1_z, 1.0f, 30.0f);

	///ImGui::DragFloat("swordPos_x", &swordPos_x, 0.01f);
	///ImGui::DragFloat("swordPos_y", &swordPos_y, 0.01f);
	///ImGui::DragFloat("swordPos_z", &swordPos_z, 0.01f);
}
