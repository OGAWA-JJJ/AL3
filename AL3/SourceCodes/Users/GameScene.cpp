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

	Sprite::LoadTexture(0, L"Resources/circle.png");
	Sprite::LoadTexture(1, L"Resources/bar.png");
	Sprite::LoadTexture(2, L"Resources/bar2.png");

	GH1 = Sprite::Create(0, XMFLOAT2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
	GH1->SetAnchorPoint({ 0.5,0.5 });
	GH2 = Sprite::Create(1, XMFLOAT2(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2));
	GH2->SetAnchorPoint({ 0.5,0.5 });
	GH3 = Sprite::Create(2, XMFLOAT2(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2));
	GH3->SetAnchorPoint({ 0.5,0.5 });

	GH1->SetSize(XMFLOAT2(64, 64));
	GH2->SetSize(XMFLOAT2(1, 128));
	GH3->SetSize(XMFLOAT2(1, 128));
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
	isHit = false;
	const float speed = 2.0f;
	if (Input::isKey(DIK_RIGHT))
	{
		barBottom.x += speed;
		barTip.x += speed;
	}
	if (Input::isKey(DIK_LEFT))
	{
		barBottom.x -= speed;
		barTip.x -= speed;
	}
	if (Input::isKey(DIK_DOWN))
	{
		barBottom.y += speed;
		barTip.y += speed;
	}
	if (Input::isKey(DIK_UP))
	{
		barBottom.y -= speed;
		barTip.y -= speed;
	}

	const float circleRad = 32.0f;
	const XMFLOAT2 circleCenter = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };

	XMFLOAT2 bottomToTip = { barTip.x - barBottom.x,  barTip.y - barBottom.y };
	XMFLOAT2 tipToCenter = { circleCenter.x - barTip.x, circleCenter.y - barTip.y };
	XMFLOAT2 bottomToCenter = { circleCenter.x - barBottom.x, circleCenter.y - barBottom.y };

	float length = sqrtf(powf(bottomToTip.x, 2) + powf(bottomToTip.y, 2));
	XMFLOAT2 normalizeVec = { bottomToTip.x / length,bottomToTip.y / length };

	float dist = bottomToCenter.x * normalizeVec.y - normalizeVec.x * bottomToCenter.y;

	if (fabs(dist) < circleRad)
	{
		float dot1 = bottomToCenter.x * bottomToTip.x + bottomToCenter.y * bottomToTip.y;
		float dot2 = tipToCenter.x * bottomToTip.x + tipToCenter.y * bottomToTip.y;

		if (dot1 * dot2 <= 0.0f) { isHit = true; }
	}

	float length2 = sqrtf(powf(tipToCenter.x, 2) + powf(tipToCenter.y, 2));
	float length3 = sqrtf(powf(bottomToCenter.x, 2) + powf(bottomToCenter.y, 2));
	if (length2 < circleRad || length3 < circleRad) { isHit = true; }

	XMFLOAT2 size = { barTip.x,barTip.y - 64 };
	GH2->SetPosition(size);
	GH3->SetPosition(size);
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
	if (isHit) { GH3->Draw(); }
	Sprite::PostDraw();
}
