#include "StageObject.h"
#include "../DirectX/DirectXImportant.h"
#include "PipelineManager.h"
#include "ModelManager.h"

#include "../../imgui/ImguiControl.h"

StageObject::StageObject()
{
	obj_sphere = Object::Create(ModelManager::model_sphere);
	const float sphereScale = 0.5f;
	obj_sphere->SetScale(
		DirectX::XMFLOAT3(sphereScale, sphereScale, sphereScale));
}

StageObject::~StageObject()
{
}

void StageObject::Init()
{
}

void StageObject::Update()
{
	obj_sphere->SetPosition(DirectX::XMFLOAT3(
		ImguiControl::Imgui_shadowEye_x,
		ImguiControl::Imgui_shadowEye_y,
		ImguiControl::Imgui_shadowEye_z));
	obj_sphere->Update();
}

void StageObject::Draw()
{
	obj_sphere->Draw(PipelineManager::obj_texColorReceice, false);
}

void StageObject::ShadowDraw()
{
}

void StageObject::LuminanceDraw()
{
}
