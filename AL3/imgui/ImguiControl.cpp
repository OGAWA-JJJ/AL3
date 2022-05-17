#include "ImguiControl.h"
#include "imgui.h"

float ImguiControl::Imgui_eye_x = 0.0f;
float ImguiControl::Imgui_eye_y = 25.0f;
float ImguiControl::Imgui_eye_z = -100.0f;
float ImguiControl::Imgui_fov = 60.0f;
float ImguiControl::Imgui_far_z = 1000.0f;
float ImguiControl::Imgui_backColor_r = 0.2f;
float ImguiControl::Imgui_backColor_g = 0.2f;
float ImguiControl::Imgui_backColor_b = 0.2f;
//int ImguiControl::Imgui_windowSize_x = 400;
//int ImguiControl::Imgui_windowSize_y = 200;
float ImguiControl::Imgui_lightColor_r = 0.6f;
float ImguiControl::Imgui_lightColor_g = 0.6f;
float ImguiControl::Imgui_lightColor_b = 0.6f;
float ImguiControl::Imgui_lightDir_x = 0.0f;
float ImguiControl::Imgui_lightDir_y = -1.0f;
float ImguiControl::Imgui_lightDir_z = 0.0f;

void ImguiControl::Update()
{
	//static float Imgui_eye[3] = { Imgui_eye_x,Imgui_eye_y,Imgui_eye_z };
	//ImGui::SliderFloat("EYE_X", &Imgui_eye_x, -200.0f, 200.0f);
	//ImGui::SliderFloat("EYE_Y", &Imgui_eye_y, -200.0f, 200.0f);
	//ImGui::SliderFloat("EYE_Z", &Imgui_eye_z, -200.0f, 200.0f);

	ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
	ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);

	ImGui::SliderFloat("BACK_COLOR_R", &Imgui_backColor_r, 0.0f, 1.0f);
	ImGui::SliderFloat("BACK_COLOR_G", &Imgui_backColor_g, 0.0f, 1.0f);
	ImGui::SliderFloat("BACK_COLOR_B", &Imgui_backColor_b, 0.0f, 1.0f);

	ImGui::SliderFloat("LIGHT_COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);

	ImGui::SliderFloat("LIGHT_DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
	ImGui::SliderFloat("LIGHT_DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);

	//ImGui::SliderInt("WINDOWSIZE_X", &Imgui_windowSize_x, 400, 800);
	//ImGui::SliderInt("WINDOWSIZE_Y", &Imgui_windowSize_y, 200, 800);
	//static float BackColor[3] = { Imgui_backColor_r ,Imgui_backColor_g,Imgui_backColor_b };
	//ImGui::ColorPicker3("BACK_COLOR", BackColor);
}
