#include "GameScene.h"
#include "../3D/FbxLoader.h"
#include "../3D/FbxObject3D.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"

GameScene::GameScene()
{
	const float objA_Scale = 20.0f;
	const float objB_Scale = 2.0f;

	modelA = Model::CreateFromObj("monkey");
	//modelB = Model::CreateFromObj("sponza");
	modelB = Model::CreateFromObj("skydome");

	objA = Object::Create(modelA);
	objB = Object::Create(modelB);

	objA->SetScale(XMFLOAT3(objA_Scale, objA_Scale, objA_Scale));
	objB->SetScale(XMFLOAT3(objB_Scale, objB_Scale, objB_Scale));

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

	//Camera::SetEye(XMFLOAT3(0, 10, -50));

	FbxObject3D::SetDevice(DirectXImportant::dev.Get());
	FbxObject3D::CreateGraphicsPipeline();

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("newPlayer");

	fbxObj1 = new FbxObject3D();
	fbxObj1->Init();
	fbxObj1->SetModel(fbxModel1);
	fbxObj1->SetScale(XMFLOAT3(0.05, 0.05, 0.05));
	fbxObj1->SetRotation(XMFLOAT3(-90, 0, 0));
	fbxObj1->PlayAnimation();
}

GameScene::~GameScene()
{
	//‘‚«–Y‚ê‚é‚©‚çƒ„ƒƒI
	delete modelA;
	delete objA;
	delete modelB;
	delete objB;
	delete fbxModel1;
	delete fbxObj1;
}

void GameScene::Init()
{
	objA->SetPosition(XMFLOAT3(0.0f, 20.0f, 0.0f));
	objA->SetRotation(XMFLOAT3(0, 180, 0));

	//Camera::SetEye({ 20,20.0f,-50.0f });
	Camera::SetTarget({ 0.0f,15.0f,0.0f });
	//Camera::SetUp({ 0.0f,1.0f,0.0f });
}

void GameScene::Update()
{
	objA->Update();
	objB->Update();

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
	//Object::SetLight(light);

	light->Update();

	XMFLOAT3 objApos = objA->GetPosition();
	if (Input::isKey(DIK_W)) { objApos.z -= MAX_MOVE_SPEED; }
	if (Input::isKey(DIK_S)) { objApos.z += MAX_MOVE_SPEED; }
	if (Input::isKey(DIK_D)) { objApos.x -= MAX_MOVE_SPEED; }
	if (Input::isKey(DIK_A)) { objApos.x += MAX_MOVE_SPEED; }
	//objA->SetPosition(objApos);

	//XMFLOAT3 rot = objA->GetRotation();
	//rot.y += 0.06f;
	//objA->SetRotation(rot);

	if (Input::isKey(DIK_RIGHT)) { cameraAngle.x += MAX_CAMERA_MOVE_SPEED; }
	if (Input::isKey(DIK_LEFT)) { cameraAngle.x -= MAX_CAMERA_MOVE_SPEED; }

	if (Input::isKey(DIK_UP)) {
		if (cameraAngle.y < XM_PI / 2) { cameraAngle.y += MAX_CAMERA_MOVE_SPEED; }
	}
	if (Input::isKey(DIK_DOWN)) {
		if (cameraAngle.y > -XM_PI / 5) { cameraAngle.y -= MAX_CAMERA_MOVE_SPEED; }
	}

	//OgaJHelper::ConvertToRadian(cameraAngle.x);
	XMFLOAT3 cameraPos = { 0,0,0 };
	float distance = MAX_DISTANCE;
	float diff = 0;

	cameraPos.y = sinf(cameraAngle.y) * MAX_DISTANCE;
	//‹ßŠñ‚é‚©‚Ì”»’è
	if (cameraPos.y < 0) {
		diff = cameraPos.y; cameraPos.y = 0;
	}

	cameraPos.x = sinf(cameraAngle.x) * (distance + diff);
	cameraPos.z = cosf(cameraAngle.x) * (distance + diff);

	Camera::SetEye(cameraPos);

	fbxObj1->Update();
}

void GameScene::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	//objA->Draw();
	//objB->Draw();
	Object::PostDraw();

	fbxObj1->Draw(DirectXImportant::cmdList.Get());
}
