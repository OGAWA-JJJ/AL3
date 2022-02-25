#include "GameScene.h"
#include "../3D/FbxLoader.h"
#include "../3D/FbxObject3D.h"

GameScene::GameScene()
{
	modelA = Model::CreateFromObj("skydome");
	modelB = Model::CreateFromObj("jihannki");

	objA = Object::Create(modelA);
	objB = Object::Create(modelB);

	objA->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	objB->SetScale(XMFLOAT3(5.0f, 5.0f, 5.0f));

	light = Light::Create();
	light->SetLightColor({ 1,1,1 });
	Object::SetLight(light);

	Camera::SetEye(XMFLOAT3(0, 10, -50));

	FbxObject3D::SetDevice(DirectXImportant::dev.Get());
	FbxObject3D::CreateGraphicsPipeline();

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	fbxObj1 = new FbxObject3D();
	fbxObj1->Init();
	fbxObj1->SetModel(fbxModel1);
	fbxObj1->PlayAnimation();
}

GameScene::~GameScene()
{
	delete modelA;
	delete modelB;
	delete objA;
	delete objB;
	delete fbxModel1;
	delete fbxObj1;
}

void GameScene::Init()
{
	objA->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	objB->SetPosition(XMFLOAT3(-40.0f, 0.0f, 0.0f));

	objA->SetRotation(XMFLOAT3(0, 0, 0));
	objB->SetRotation(XMFLOAT3(0, -90, 0));

	Camera::SetEye({ 20,20.0f,-50.0f });
	Camera::SetTarget({ 0.0f,5.0f,0.0f });
	Camera::SetUp({ 0.0f,1.0f,0.0f });
}

void GameScene::Update()
{
	objA->Update();
	objB->Update();

	light->Update();

	XMFLOAT3 rot = objA->GetRotation();
	rot.y += 0.03f;
	objA->SetRotation(rot);

	XMFLOAT3 eye = Camera::GetEye();
	if (Input::isKey(DIK_RIGHT)) { eye.x += 1.0f; }
	if (Input::isKey(DIK_LEFT)) { eye.x -= 1.0f; }
	Camera::SetEye(eye);

	fbxObj1->Update();
}

void GameScene::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	objA->Draw();
	objB->Draw();
	Object::PostDraw();

	fbxObj1->Draw(DirectXImportant::cmdList.Get());
}
