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
	Object::ObjectInitData init;
	init.m_vsEntryPoint = "VSmain";
	init.m_psEntryPoint = "PSmain";
	l_aaa = Object::CreateGraphicsPipeline(init);

	//–¼‘O•ª‚©‚è‚É‚­‚¢
	obj_model = Model::CreateFromObj("Sword");
	obj_Sword = Object::Create(obj_model);
	l_model0 = FbxModels::CreateFromFbx("RunMiku");
	l_obj0 = FbxObjects::Create(l_model0);
	l_model1 = FbxModels::CreateFromFbx("SlowRunMiku");
	l_obj1 = FbxObjects::Create(l_model1);

	obj_Sword->SetScale(XMFLOAT3(1, 1, 1));
	obj_Sword->SetRotation(XMFLOAT3(
		310.0f,
		300.0f,
		310.0f));

	l_obj0->SetScale(XMFLOAT3(0.01, 0.01, 0.01));
	l_obj0->SetRotation(XMFLOAT3(0, 180, 0));
	l_obj0->PlayAnimation();

	l_obj1->SetScale(XMFLOAT3(0.01, 0.01, 0.01));
	l_obj1->SetRotation(XMFLOAT3(0, 180, 0));
	l_obj1->PlayAnimation();
}

GameScene::~GameScene()
{
	delete GH1;
	delete light;
	delete obj_model;
	delete obj_Sword;
	delete l_model0;
	delete l_obj0;
	delete l_model1;
	delete l_obj1;
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
	Object::SetLight(light);

	XMFLOAT3 rot = l_obj0->GetRotation();
	rot.y += 0.5f;

	bool l_isFastRun = isFastRun;
	isFastRun = ImguiControl::Imgui_isFastRun;
	if (l_isFastRun != isFastRun)
	{
		rate = 0.0f;
		if (isFastRun)
		{
			l_obj0->ResetAnimation();
		}
		else
		{
			l_obj1->ResetAnimation();
		}
	}

	float addTimer = 0.015f;
	if (isFastRun)
	{
		if (rate < 1.0f)
		{
			l_obj1->Update();
			rate += addTimer;
			if (rate > 1.0f)
			{
				rate = 1.0f;
			}
		}
		l_obj0->BlendAnimation(l_obj1, OgaJEase::easeOutCubic(rate));
		l_obj0->Update();
		obj_Sword->MultiMatrix(l_obj0->GetMatrix());
	}
	else
	{
		if (rate < 1.0f)
		{
			l_obj0->Update();
			rate += addTimer;
			if (rate > 1.0f)
			{
				rate = 1.0f;
			}
		}
		l_obj1->Update();
		l_obj1->BlendAnimation(l_obj0, OgaJEase::easeOutCubic(rate));
		obj_Sword->MultiMatrix(l_obj1->GetMatrix());
	}

	obj_Sword->Update();
}

void GameScene::Draw()
{
	//Sprite::PreDraw(DirectXImportant::cmdList.Get());
	//if (isHit) { GH1->Draw(); }
	//GH1->Draw();
	//Sprite::PostDraw();

	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_Sword->Draw(l_aaa);
	Object::PostDraw();

	if (isFastRun)
	{
		l_obj0->Draw(l_normal);
	}
	else
	{
		l_obj1->Draw(l_normal);
	}
}

void GameScene::LuminanceDraw()
{
}

void GameScene::ShadowDraw()
{
}
