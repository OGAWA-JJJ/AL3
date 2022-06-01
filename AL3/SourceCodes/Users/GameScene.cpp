#include "GameScene.h"
#include "../3D/FbxLoader.h"
#include "../3D/FbxObject3D.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"

GameScene::GameScene()
{
	const float objA_Scale = 40.0f;
	const float objB_Scale = 20.0f;
	const float objC_Scale = 20.0f;
	const float fbx1_Scale = 0.1f;

	//XMFLOAT3 camera = Camera::GetEye();
	//OgaJHelper::ConvertToRadian(camera.y);
	//cameraAngle = { 0,camera.y,0 };

	//modelA = Model::CreateFromObj("Lich");
	//modelB = Model::CreateFromObj("ground");
	////modelB = Model::CreateFromObj("sponza");
	//modelC = Model::CreateFromObj("monkey");

	//objA = Object::Create(modelA);
	//objB = Object::Create(modelB);
	//objC = Object::Create(modelC);

	//objA->SetScale(XMFLOAT3(objA_Scale, objA_Scale, objA_Scale));
	//objB->SetScale(XMFLOAT3(objB_Scale, objB_Scale, objB_Scale));
	//objC->SetScale(XMFLOAT3(objC_Scale, objC_Scale, objC_Scale));

	//light = Light::Create();
	//light->SetLightColor(
	//	{
	//		ImguiControl::Imgui_lightColor_r,
	//		ImguiControl::Imgui_lightColor_g,
	//		ImguiControl::Imgui_lightColor_b
	//	});
	//light->SetLightDir(
	//	{
	//		ImguiControl::Imgui_lightDir_x,
	//		ImguiControl::Imgui_lightDir_y,
	//		ImguiControl::Imgui_lightDir_z,
	//	});

	//Object::SetLight(light);

	//FbxObject3D::SetDevice(DirectXImportant::dev.Get());
	//FbxObject3D::CreateGraphicsPipeline();

	//fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("player_stay");

	//fbxObj1 = new FbxObject3D();
	//fbxObj1->Init();
	//fbxObj1->SetModel(fbxModel1);
	//fbxObj1->SetScale(XMFLOAT3(fbx1_Scale, fbx1_Scale, fbx1_Scale));
	//fbxObj1->SetRotation(XMFLOAT3(0, 0, 0));
	//fbxObj1->PlayAnimation();

	Sprite::LoadTexture(0, L"Resources/hamurabyss.png");
	GH1 = Sprite::Create(0, XMFLOAT2(0, 0));
	GH2 = Sprite::Create(0, XMFLOAT2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
	GH3 = Sprite::Create(0, XMFLOAT2(0, 0));
	GH1->SetSize(XMFLOAT2(64, 64));
	GH2->SetSize(XMFLOAT2(64, 64));
	GH3->SetSize(XMFLOAT2(64, 64));

}

GameScene::~GameScene()
{
	//‘‚«–Y‚ê‚é‚©‚çƒ„ƒƒI
	//delete modelA;
	//delete objA;
	//delete modelB;
	//delete objB;
	//delete modelC;
	//delete objC;
	//delete fbxModel1;
	//delete fbxObj1;
	delete GH1;
	delete GH2;
	delete GH3;
}

void GameScene::Init()
{
	////Lich
	//objA->SetPosition(XMFLOAT3(0.0f, 65.0f, -400.0f));
	//objA->SetRotation(XMFLOAT3(0, -100, 0));

	////skydome or sponza
	//objB->SetPosition(XMFLOAT3(0.0f, -10.0f, 0.0f));
	//objB->SetRotation(XMFLOAT3(0, 90, 0));

	////monkey
	//objC->SetPosition(XMFLOAT3(0.0f, 20.0f, 0.0f));
	//objC->SetRotation(XMFLOAT3(0, 180, 0));

	//Camera::SetEye({ 20,20.0f,-50.0f });
	Camera::SetTarget({ 0.0f,15.0f,0.0f });
	//Camera::SetUp({ 0.0f,1.0f,0.0f });
}

void GameScene::Update()
{
#pragma region Individual

	//objB->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	//light->SetLightColor(
	//	{
	//		ImguiControl::Imgui_lightColor_r,
	//		ImguiControl::Imgui_lightColor_g,
	//		ImguiControl::Imgui_lightColor_b
	//	});
	//light->SetLightDir(
	//	{
	//		ImguiControl::Imgui_lightDir_x,
	//		ImguiControl::Imgui_lightDir_y,
	//		ImguiControl::Imgui_lightDir_z,
	//	});

	//XMFLOAT3 objApos = objA->GetPosition();
	//if (Input::isKey(DIK_W)) { objApos.z -= MAX_MOVE_SPEED; }
	//if (Input::isKey(DIK_S)) { objApos.z += MAX_MOVE_SPEED; }
	//if (Input::isKey(DIK_D)) { objApos.x -= MAX_MOVE_SPEED; }
	//if (Input::isKey(DIK_A)) { objApos.x += MAX_MOVE_SPEED; }

	//if (Input::isKey(DIK_RIGHT)) { cameraAngle.x += MAX_CAMERA_MOVE_SPEED; }
	//if (Input::isKey(DIK_LEFT)) { cameraAngle.x -= MAX_CAMERA_MOVE_SPEED; }

	//if (Input::isKey(DIK_UP)) {
	//	if (cameraAngle.y < XM_PI / 2) { cameraAngle.y += MAX_CAMERA_MOVE_SPEED; }
	//}
	//if (Input::isKey(DIK_DOWN)) {
	//	if (cameraAngle.y > -XM_PI / 5) { cameraAngle.y -= MAX_CAMERA_MOVE_SPEED; }
	//}

	//XMFLOAT3 cameraPos = { 0,0,0 };
	//float distance = MAX_DISTANCE;
	//float diff = 0;

	//cameraPos.y = sinf(cameraAngle.y) * MAX_DISTANCE;
	////‹ßŠñ‚é‚©‚Ì”»’è
	//if (cameraPos.y < 0) {
	//	diff = cameraPos.y; cameraPos.y = 0;
	//}

	//cameraPos.x = sinf(cameraAngle.x) * (distance + diff);
	//cameraPos.z = cosf(cameraAngle.x) * (distance + diff);

#pragma endregion

#pragma region Update

	/*----------Update,Setter----------*/
	//objB->SetPosition(XMFLOAT3(0.0f, ImguiControl::Imgui_ground_y, 0.0f));
	//Camera::SetEye(cameraPos);

	//Object::SetLight(light);
	//FbxObject3D::SetLight(light);

	//light->Update();

	//objA->Update();
	//objB->Update();
	//objC->Update();

	//fbxObj1->Update();
	/*----------Update,Setter----------*/

#pragma endregion

	/*---MT4---*/
	const float dist = 150.0f;
	XMFLOAT2 pos = { 0,0 };
	XMFLOAT2 pos2 = { 0,0 };

	if (Input::isKey(DIK_UP)) { dist2 += 3.0f; }

	pos = {
		WINDOW_WIDTH / 2 + sinf(radian.x) * dist,
		WINDOW_HEIGHT / 2 + cosf(radian.y) * dist };

	pos2 = {
		WINDOW_WIDTH / 2 + sinf(radian.x) * dist2,
		WINDOW_HEIGHT / 2 + cosf(radian.y) * dist2 };

	radian.x += 0.05f;
	radian.y += 0.05f;

	GH1->SetPosition(pos);
	GH3->SetPosition(pos2);
}

void GameScene::Draw()
{
	//Object::PreDraw(DirectXImportant::cmdList.Get());
	//objA->Draw();
	//objB->Draw();
	//objC->Draw();
	//Object::PostDraw();

	//fbxObj1->Draw(DirectXImportant::cmdList.Get());

	Sprite::PreDraw(DirectXImportant::cmdList.Get());
	GH1->Draw();
	GH2->Draw();
	GH3->Draw();
	Sprite::PostDraw();
}
