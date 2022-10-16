#include "GameScene.h"
//#include "../3D/FbxLoader.h"
//#include "../3D/FbxObject3D.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"
#include "../Input/Input.h"
#include "../DirectX/Camera.h"
#include "../Math/OBBCollision.h"

GameScene::GameScene()
{
	//Sprite::LoadTexture(0, L"Resources/ss.png");
	Sprite::LoadTexture(0, L"Resources/hamurabyss.png");
	GH1 = Sprite::Create(0, DirectX::XMFLOAT2(0, 0));
	//GH1->SetSize(DirectX::XMFLOAT2(128 * 2.0, 72 * 2.0));
	GH1->SetSize(DirectX::XMFLOAT2(64, 64));

	posY = 64.0f;

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

	FbxObjects::FbxInitData initData;
	initData.m_vsEntryPoint = "PSmain";
	initData.m_vsEntryPoint = "VSmain";
	l_normal = FbxObjects::CreateGraphicsPipeline(initData);

	//–¼‘O•ª‚©‚è‚É‚­‚¢
	l_model0 = FbxModels::CreateFromFbx("RunMiku");
	l_obj0 = FbxObjects::Create(l_model0);

	l_obj0->SetScale(XMFLOAT3(0.015, 0.015, 0.015));
	l_obj0->SetRotation(XMFLOAT3(0, 150, 0));
	l_obj0->PlayAnimation();
}

GameScene::~GameScene()
{
	delete GH1;
}

void GameScene::Init(ID3D12Resource* texbuff)
{
}

void GameScene::Update()
{
	//MT4
	dist = 360.0f - posY;
	const float m = 0.1f;
	accY = 0.2f;
	accY += dist * 0.01f / m;
	velY += accY;
	velY -= velY * 0.1f;
	posY += velY;
	GH1->SetPosition(XMFLOAT2(0, posY));

	if (Input::isKeyTrigger(DIK_R)) { posY = 0.0f; }

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
	FbxObjects::SetLight(light);

	XMFLOAT3 rot = l_obj0->GetRotation();
	rot.y += 0.5f;
	//l_obj0->SetRotation(rot);
	l_obj0->Update();
}

void GameScene::Draw()
{
	//Sprite::PreDraw(DirectXImportant::cmdList.Get());
	//if (isHit) { GH1->Draw(); }
	//GH1->Draw();
	//Sprite::PostDraw();

	l_obj0->Draw(l_normal);
}

void GameScene::LuminanceDraw()
{
}

void GameScene::ShadowDraw()
{
}
