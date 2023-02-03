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

	obj_bonefire = Object::Create(ModelManager::model_bonefire);
	const float bonefireScale = 4.0f;
	obj_bonefire->SetScale(
		DirectX::XMFLOAT3(bonefireScale, bonefireScale, bonefireScale));
	obj_bonefire->SetPosition(DirectX::XMFLOAT3(
		0,
		5,
		0));
}

StageObject::~StageObject()
{
}

void StageObject::Init()
{
	obj_bonefire->SetColor(DirectX::XMFLOAT4(1, 1, 1, 0));
}

void StageObject::Update()
{
	obj_sphere->SetPosition(DirectX::XMFLOAT3(
		ImguiControl::Imgui_shadowEye_x,
		ImguiControl::Imgui_shadowEye_y,
		ImguiControl::Imgui_shadowEye_z));
	obj_sphere->Update();
}

void StageObject::AfterBattleUpdate()
{
	float l_alpha = obj_bonefire->GetColor().w;
	if (l_alpha < 1.0f)
	{
		l_alpha += 0.05f;
	}
	obj_bonefire->SetColor(DirectX::XMFLOAT4(1, 1, 1, l_alpha));
	obj_bonefire->Update();
}

void StageObject::Draw()
{
	obj_sphere->Draw(PipelineManager::obj_texColorReceice, false);
}

void StageObject::AfterBattleDraw()
{
	obj_bonefire->Draw(PipelineManager::obj_receiveShadow);
}

void StageObject::ShadowDraw()
{
}

void StageObject::LuminanceDraw()
{
}
