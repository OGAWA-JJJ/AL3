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
	const float fbx1_Scale = 20.0f;

	XMFLOAT3 camera = Camera::GetEye();
	OgaJHelper::ConvertToRadian(camera.y);
	cameraAngle = { 0,camera.y,0 };
	isChangedRot = false;

	modelA = Model::CreateFromObj("Lich");
	//modelA = Model::CreateFromObj("number1");
	modelB = Model::CreateFromObj("ground");
	//modelB = Model::CreateFromObj("sponza");
	modelC = Model::CreateFromObj("monkey");
	//modelC = Model::CreateFromObj("newOthello");

	objA = Object::Create(modelA);
	objB = Object::Create(modelB);
	objC = Object::Create(modelC);

	objA->SetScale(XMFLOAT3(objA_Scale, objA_Scale, objA_Scale));
	objB->SetScale(XMFLOAT3(objB_Scale, objB_Scale, objB_Scale));
	objC->SetScale(XMFLOAT3(objC_Scale, objC_Scale, objC_Scale));

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

	FbxObject3D::SetDevice(DirectXImportant::dev.Get());
	FbxObject3D::CreateGraphicsPipeline();

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	fbxObj1 = new FbxObject3D();
	fbxObj1->Init();
	fbxObj1->SetModel(fbxModel1);
	fbxObj1->SetScale(XMFLOAT3(fbx1_Scale, fbx1_Scale, fbx1_Scale));
	fbxObj1->SetRotation(XMFLOAT3(0, 0, 0));
	fbxObj1->PlayAnimation();

	Sprite::LoadTexture(0, L"Resources/hamurabyss.png");
	GH1 = Sprite::Create(0, XMFLOAT2(0, 0));
	GH1->SetColor(XMFLOAT4(1, 1, 1, 0.5));
}

GameScene::~GameScene()
{
	//‘‚«–Y‚ê‚é‚©‚çƒ„ƒƒI
	delete modelA;
	delete objA;
	delete modelB;
	delete objB;
	delete modelC;
	delete objC;
	delete fbxModel1;
	delete fbxObj1;
}

void GameScene::Init()
{
	//Lich
	objA->SetPosition(XMFLOAT3(0.0f, 65.0f, -400.0f));
	objA->SetRotation(XMFLOAT3(0, -100, 0));

	//skydome or sponza
	objB->SetPosition(XMFLOAT3(0.0f, -10.0f, 0.0f));
	objB->SetRotation(XMFLOAT3(0, 90, 0));

	//monkey
	objC->SetPosition(XMFLOAT3(0.0f, 20.0f, 0.0f));
	objC->SetRotation(XMFLOAT3(0, 180, 0));

	//Camera::SetEye({ 20,20.0f,-50.0f });
	Camera::SetTarget({ 0.0f,15.0f,0.0f });
	//Camera::SetUp({ 0.0f,1.0f,0.0f });
}

void GameScene::Update()
{

#pragma region AEì•i

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

	XMFLOAT3 objCpos = objC->GetPosition();
	float isWkey = 0;
	float isSkey = 0;
	float isDkey = 0;
	float isAkey = 0;
	float myVec = 0.0f;
	float nearArc = 0.0f;

	if (Input::isKey(DIK_W) || Input::isKey(DIK_S) || Input::isKey(DIK_D) || Input::isKey(DIK_A))
	{
		if (Input::isKey(DIK_W)) { isWkey = -1; }
		if (Input::isKey(DIK_S)) { isSkey = 1; }
		if (Input::isKey(DIK_D)) { isDkey = -1; }
		if (Input::isKey(DIK_A)) { isAkey = 1; }

		isChangedRot = true;
	}

	if (isChangedRot)
	{
		float rotY = objC->GetRotation().y * -1;
		OgaJHelper::ConvertToRadian(rotY);

		myVec = atan2(cosf(rotY), sinf(rotY));
		OgaJHelper::ConvertToDegree(myVec);
		if (myVec < 0.0f) { myVec += 360.0f; }

		float keyVec = atan2(isWkey + isSkey, isDkey + isAkey) * -1;
		OgaJHelper::ConvertToDegree(keyVec);
		if (keyVec < 0.0f) { keyVec += 360.0f; }

		nearArc = OgaJHelper::RotateEarliestArc(myVec, keyVec);

		myVec += nearArc + 90.0f;
		objC->SetRotation(XMFLOAT3(0, myVec, 0));

		XMFLOAT2 vec2 = { cosf(myVec) ,sinf(myVec) };

		float ws = vec2.y;
		float ad = vec2.x;

		/*if (isWkey != 0) {
			objCpos.z -= vec2.y * MAX_MOVE_SPEED;
		}
		if (isSkey != 0) {
			objCpos.z += vec2.y * MAX_MOVE_SPEED;
		}
		if (isDkey != 0) {
			objCpos.x -= vec2.x * MAX_MOVE_SPEED;
		}
		if (isAkey != 0) {
			objCpos.x += vec2.x * MAX_MOVE_SPEED;
		}*/

		if (isWkey != 0) {
			objCpos.z -= MAX_MOVE_SPEED;
		}
		if (isSkey != 0) {
			objCpos.z += MAX_MOVE_SPEED;
		}
		if (isDkey != 0) {
			objCpos.x -= MAX_MOVE_SPEED;
		}
		if (isAkey != 0) {
			objCpos.x += MAX_MOVE_SPEED;
		}

		isChangedRot = false;
	}


	if (Input::isKey(DIK_RIGHT)) { cameraAngle.x += MAX_CAMERA_MOVE_SPEED; }
	if (Input::isKey(DIK_LEFT)) { cameraAngle.x -= MAX_CAMERA_MOVE_SPEED; }

	if (Input::isKey(DIK_UP)) {
		if (cameraAngle.y < XM_PI / 2) { cameraAngle.y += MAX_CAMERA_MOVE_SPEED; }
	}
	if (Input::isKey(DIK_DOWN)) {
		if (cameraAngle.y > -XM_PI / 5) { cameraAngle.y -= MAX_CAMERA_MOVE_SPEED; }
	}

	XMFLOAT3 cameraPos = objC->GetPosition();
	float distance = MAX_DISTANCE;
	float diff = 0;

	cameraPos.y = sinf(cameraAngle.y) * MAX_DISTANCE;
	//‹ßŠñ‚é‚©‚Ì”»’è
	if (cameraPos.y < 0) {
		diff = cameraPos.y; cameraPos.y = 0;
	}

	cameraPos.x += sinf(cameraAngle.x) * (distance + diff);
	cameraPos.z += cosf(cameraAngle.x) * (distance + diff);

#pragma endregion


#pragma region AEì•i(Update,Setter)

	/*----------Update,Setter----------*/

	objB->SetPosition(XMFLOAT3(0.0f, ImguiControl::Imgui_ground_y, 0.0f));
	objC->SetPosition(objCpos);

	Camera::SetTarget(objCpos);
	Camera::SetEye(cameraPos);

	Object::SetLight(light);
	FbxObject3D::SetLight(light);

	light->Update();

	objA->Update();
	objB->Update();
	objC->Update();

	fbxObj1->Update();

	/*----------Update,Setter----------*/

#pragma endregion

}

void GameScene::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	//objA->Draw();
	//objB->Draw();
	//objC->Draw();
	Object::PostDraw();

	fbxObj1->Draw(DirectXImportant::cmdList.Get());

	Sprite::PreDraw(DirectXImportant::cmdList.Get());
	GH1->Draw();
	Sprite::PostDraw();
}
