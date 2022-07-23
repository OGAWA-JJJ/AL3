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

bool ImguiControl::Imgui_targetDraw = false;
bool ImguiControl::Imgui_isTargetMove = true;
bool ImguiControl::Imgui_isOBBDraw = true;
bool ImguiControl::Imgui_isHitStopAnimation = false;
bool ImguiControl::Imgui_playerDraw = true;

float ImguiControl::boxPos1_x = 50.0f;
float ImguiControl::boxPos1_y = 25.0f;
float ImguiControl::boxPos1_z = 0.0f;

float ImguiControl::boxRot1_x = -50.0f;
float ImguiControl::boxRot1_y = 25.0f;
float ImguiControl::boxRot1_z = 0.0f;

float ImguiControl::boxSca1_x = 10.0f;
float ImguiControl::boxSca1_y = 20.0f;
float ImguiControl::boxSca1_z = 10.0f;

bool ImguiControl::Imgui_isMonochromatic = false;
bool ImguiControl::Imgui_isPhong = true;
bool ImguiControl::Imgui_isToon = false;
bool ImguiControl::Imgui_isGaussian = false;
bool ImguiControl::Imgui_isBloom = true;
bool ImguiControl::Imgui_isShadowMap = true;

void ImguiControl::Update()
{
	if (ImGui::CollapsingHeader("CameraSettings"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera");
		ImGui::SliderFloat("FOV", &Imgui_fov, 30.0f, 150.0f);
		ImGui::SliderFloat("FAR_Z", &Imgui_far_z, 50.0f, 5000.0f);
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("LightSettings"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Light");

		ImGui::SliderFloat("LIGHT_DIR_X", &Imgui_lightDir_x, -1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_DIR_Y", &Imgui_lightDir_y, -1.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_DIR_Z", &Imgui_lightDir_z, -1.0f, 1.0f);

		ImGui::SliderFloat("LIGHT_COLOR_R", &Imgui_lightColor_r, 0.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_G", &Imgui_lightColor_g, 0.0f, 1.0f);
		ImGui::SliderFloat("LIGHT_COLOR_B", &Imgui_lightColor_b, 0.0f, 1.0f);
	}
}
