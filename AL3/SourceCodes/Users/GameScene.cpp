#include "GameScene.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/DirectXImportant.h"
#include "PipelineManager.h"

#include "../Math/OgaJHelper.h"

GameScene::GameScene()
{
	PipelineManager::Init();

	light = Light::Create();
	light->SetLightColor(
		{
			ImguiControl::Imgui_lightColor_r,
			ImguiControl::Imgui_lightColor_g,
			ImguiControl::Imgui_lightColor_b
		});
	light->SetLightDir(
		{
			ImguiControl::Imgui_lightDir_x,
			ImguiControl::Imgui_lightDir_y,
			ImguiControl::Imgui_lightDir_z,
		});

	Object::SetLight(light);
}

GameScene::~GameScene()
{
}

void GameScene::Init(ID3D12Resource* texbuff)
{
	l_player->Init();
	l_enemy->Init();
	l_stage->Init(texbuff);

	//Camera
	/*DirectX::XMFLOAT3 enemyToPlayer = OgaJHelper::CalcDirectionVec3(l_enemy->GetPos(), l_player->GetPos());
	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);

	Camera::SetEye(DirectX::XMFLOAT3(
		l_player->GetPos().x + enemyToPlayer.x * l_player->MAX_DISTANCE,
		50.0f,
		l_player->GetPos().z + enemyToPlayer.z * l_player->MAX_DISTANCE));*/
}

void GameScene::Update()
{
	light->SetLightColor(
		{
			ImguiControl::Imgui_lightColor_r,
			ImguiControl::Imgui_lightColor_g,
			ImguiControl::Imgui_lightColor_b
		});
	light->SetLightDir(
		{
			ImguiControl::Imgui_lightDir_x,
			ImguiControl::Imgui_lightDir_y,
			ImguiControl::Imgui_lightDir_z,
		});
	light->Update();

	//Lightî•ñ‚ðXV
	Object::SetLight(light);
	FbxObject3D::SetLight(light);

	l_player->Update(l_enemy->GetPos());
	l_enemy->Update(l_player->GetPos());
	l_stage->Update();
}

void GameScene::Draw()
{
	l_player->Draw();
	l_enemy->Draw();
	l_stage->Draw();
}

void GameScene::LuminanceDraw()
{
	l_player->LuminanceDraw();
}

void GameScene::ShadowDraw()
{
	l_player->ShadowDraw();
}
