#include "GameScene.h"
#include "../3D/FbxLoader.h"
#include "../3D/FbxObject3D.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"
#include "../Input/FunaInput.h"

GameScene::GameScene()
{
	ObjectInitData objInitData;
	objInitData.m_psEntryPoint = "PSmain";
	objInitData.m_vsEntryPoint = "VSmain";
	pipelineSet = Object::CreateGraphicsPipeline(objInitData);

	ObjectInitData shadowInitData;
	shadowInitData.m_psEntryPoint = "PSBlack";
	shadowInitData.m_vsEntryPoint = "VSShadowMain";
	shadow = Object::CreateGraphicsPipeline(shadowInitData);

	ObjectInitData objData;
	objData.m_psEntryPoint = "PSShadowMain";
	objData.m_vsEntryPoint = "VSShadowMain";
	shadow2 = Object::CreateGraphicsPipeline(objData);


	const float objA_Scale = 40.0f;
	const float objB_Scale = 20.0f;
	const float objC_Scale = 20.0f;
	const float fbx1_Scale = 0.05f;
	const float fbx2_Scale = 10.0f;
	const float fbx3_Scale = 10.0f;

	XMFLOAT3 camera = Camera::GetEye();
	OgaJHelper::ConvertToRadian(camera.y);
	cameraAngle = { 0,camera.y,0 };

	modelA = Model::CreateFromObj("triangle");
	modelB = Model::CreateFromObj("yuka");
	//modelB = Model::CreateFromObj("sponza");
	modelC = Model::CreateFromObj("monkey");
	modelD = Model::CreateFromObj("triangle");

	objA = Object::Create(modelA);
	objB = Object::Create(modelB);
	objC = Object::Create(modelC);
	objD = Object::Create(modelD);

	objA->SetScale(XMFLOAT3(objA_Scale, objA_Scale, objA_Scale));
	objB->SetScale(XMFLOAT3(objB_Scale, objB_Scale, objB_Scale));
	objC->SetScale(XMFLOAT3(objC_Scale, objC_Scale, objC_Scale));
	objD->SetScale(XMFLOAT3(5, 5, 5));

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

	fbxModel1 = FbxLoader::GetInstance()->LoadModelFromFile("RunMiku");
	//fbxModel2 = FbxLoader::GetInstance()->LoadModelFromFile("SlowRunMiku");
	//fbxModel3 = FbxLoader::GetInstance()->LoadModelFromFile("RunMiku");

	fbxObj1 = new FbxObject3D();
	fbxObj1->Init();
	fbxObj1->SetModel(fbxModel1);
	fbxObj1->SetPosition(XMFLOAT3(0, 30, 0));
	fbxObj1->SetScale(XMFLOAT3(fbx1_Scale, fbx1_Scale, fbx1_Scale));
	fbxObj1->SetRotation(XMFLOAT3(0, 0, 0));
	fbxObj1->PlayAnimation();

	//fbxObj2 = new FbxObject3D();
	//fbxObj2->Init();
	//fbxObj2->SetModel(fbxModel2);
	//fbxObj2->SetScale(XMFLOAT3(fbx2_Scale, fbx2_Scale, fbx2_Scale));
	//fbxObj2->SetRotation(XMFLOAT3(0, 0, 0));
	//fbxObj2->PlayAnimation();

	//fbxObj3 = new FbxObject3D();
	//fbxObj3->Init();
	//fbxObj3->SetModel(fbxModel3);
	//fbxObj3->SetScale(XMFLOAT3(fbx3_Scale, fbx3_Scale, fbx3_Scale));
	//fbxObj3->SetRotation(XMFLOAT3(0, 0, 0));
	//fbxObj3->PlayAnimation();

	Sprite::LoadTexture(0, L"Resources/hamurabyss.png");
	GH1 = Sprite::Create(0, XMFLOAT2(0, 0));

	obj1 = Object::Create(modelC);
	obj2 = Object::Create(modelC);
	obj3 = Object::Create(modelC);
	obj4 = Object::Create(modelC);
	obj5 = Object::Create(modelC);

	obj1->SetPosition(XMFLOAT3(0.0f, 20.0f, 0.0f));
	obj2->SetPosition(XMFLOAT3(-50, 0, 0));
	obj3->SetPosition(XMFLOAT3(0, 0, 0));
	obj4->SetPosition(XMFLOAT3(50, 0, 0));
	obj5->SetPosition(XMFLOAT3(100, 0, 0));

	const float obj_Scale = 20.0f;
	obj1->SetScale(XMFLOAT3(obj_Scale, obj_Scale, obj_Scale));
	obj2->SetScale(XMFLOAT3(obj_Scale, obj_Scale, obj_Scale));
	obj3->SetScale(XMFLOAT3(obj_Scale, obj_Scale, obj_Scale));
	obj4->SetScale(XMFLOAT3(obj_Scale, obj_Scale, obj_Scale));
	obj5->SetScale(XMFLOAT3(obj_Scale, obj_Scale, obj_Scale));
}

GameScene::~GameScene()
{
	//書き忘れるからヤメロ！
	delete modelA;
	delete objA;
	delete modelB;
	delete objB;
	delete modelC;
	delete objC;
	delete modelD;
	delete objD;
	delete fbxModel1;
	delete fbxObj1;
	delete fbxModel2;
	delete fbxObj2;
	delete fbxModel3;
	delete fbxObj3;
	delete GH1;

	delete obj1;
	delete obj2;
	delete obj3;
	delete obj4;
	delete obj5;
}

void GameScene::Init(ID3D12Resource* texbuff)
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

	//eye
	objD->SetBillboard(true);

	Camera::SetTarget(XMFLOAT3(
		objC->GetPosition().x,
		objC->GetPosition().y,
		objC->GetPosition().z));

	XMFLOAT3 enemyToPlayer = OgaJHelper::CalcDirectionVec3(objA->GetPosition(), objC->GetPosition());
	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);

	Camera::SetEye(XMFLOAT3(
		objC->GetPosition().x + enemyToPlayer.x * MAX_DISTANCE,
		50.0f,
		objC->GetPosition().z + enemyToPlayer.z * MAX_DISTANCE));

	cameraY = Camera::GetEye().y;

	objC->AddTexture(texbuff, modelC->GetDescHeap());
}

void GameScene::Update()
{

#pragma region Calc

	//FunaInput
	FunaInput::Update();

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

	//各種変数
	XMFLOAT3 objCpos = objC->GetPosition();
	XMFLOAT3 cameraPos = Camera::GetEye();
	XMFLOAT3 targetPos = Camera::GetTarget();
	XMFLOAT3 enemyToPlayer = OgaJHelper::CalcDirectionVec3(objA->GetPosition(), objC->GetPosition());
	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);
	XMFLOAT3 cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), objC->GetPosition());
	cameraToPlayer = OgaJHelper::CalcNormalizeVec3(cameraToPlayer);

	if (!isTarget)
	{
		//pad
		if (FunaInput::isConnect)
		{
			if (FunaInput::isPadTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
			{
				cameraMoveEase = 0.0f;
				isTarget = true;
			}

			if (0.3 < fabs(FunaInput::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
				0.3 < fabs(FunaInput::isPadThumb(XINPUT_THUMB_LEFTVERT)))
			{
				XMFLOAT3 vec = { 0,0,0 };
				vec.x = FunaInput::isPadThumb(XINPUT_THUMB_LEFTSIDE);
				vec.z = FunaInput::isPadThumb(XINPUT_THUMB_LEFTVERT);

				objCpos.x += vec.z * cameraToPlayer.x * MAX_MOVE_SPEED;
				objCpos.z += vec.z * cameraToPlayer.z * MAX_MOVE_SPEED;

				float rad = atan2(cameraToPlayer.x, cameraToPlayer.z);
				rad += XM_PI / 2;
				objCpos.x += vec.x * sinf(rad) * MAX_MOVE_SPEED;
				objCpos.z += vec.x * cosf(rad) * MAX_MOVE_SPEED;

				cameraPos.x += vec.z * cameraToPlayer.x * MAX_MOVE_SPEED;
				cameraPos.z += vec.z * cameraToPlayer.z * MAX_MOVE_SPEED;
				cameraPos.x += vec.x * sinf(rad) * MAX_MOVE_SPEED;
				cameraPos.z += vec.x * cosf(rad) * MAX_MOVE_SPEED;

				targetPos.x += vec.z * cameraToPlayer.x * MAX_MOVE_SPEED;
				targetPos.z += vec.z * cameraToPlayer.z * MAX_MOVE_SPEED;
				targetPos.x += vec.x * sinf(rad) * MAX_MOVE_SPEED;
				targetPos.z += vec.x * cosf(rad) * MAX_MOVE_SPEED;

				float deg = atan2(vec.x, vec.z);
				OgaJHelper::ConvertToDegree(deg);
				float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
				OgaJHelper::ConvertToDegree(cameraRad);

				Camera::SetEye(cameraPos);
				Camera::SetTarget(targetPos);
				objC->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
			}

			if (0.3 < fabs(FunaInput::isPadThumb(XINPUT_THUMB_RIGHTSIDE)) ||
				0.3 < fabs(FunaInput::isPadThumb(XINPUT_THUMB_RIGHTVERT)))
			{
				//ここをどうにかしたい,targetは最悪補完かける
				XMFLOAT3 objc = objC->GetPosition();
				XMFLOAT3 eye = Camera::GetEye();

				XMFLOAT3 playerToCamera = OgaJHelper::CalcDirectionVec3(objC->GetPosition(), Camera::GetEye());
				playerToCamera = OgaJHelper::CalcNormalizeVec3(playerToCamera);

				float xz = atan2(playerToCamera.x, playerToCamera.z);
				OgaJHelper::ConvertToDegree(xz);
				//計算
				xz += FunaInput::isPadThumb(XINPUT_THUMB_RIGHTSIDE) * MAX_CAMERA_MOVE_SPEED;
				cameraPos.y += FunaInput::isPadThumb(XINPUT_THUMB_RIGHTVERT) * MAX_CAMERA_MOVE_SPEED;

				//距離と差分
				float diff = 0;
				if (cameraPos.y < 0)
				{
					//地面との仮判定
					diff = cameraPos.y;
					cameraPos.y = 0;
				}

				//radに戻す
				OgaJHelper::ConvertToRadian(xz);
				float s = sinf(xz);
				float c = cosf(xz);
				//sin.cos
				Camera::SetEye(XMFLOAT3(
					objCpos.x + s * (MAX_DISTANCE + diff),
					cameraPos.y,
					objCpos.z + c * (MAX_DISTANCE + diff)
				));
			}

			if (isEase)
			{
				XMFLOAT3 target;
				if (cameraMoveEase < 1.0f)
				{
					target = OgaJEase::easeOutCubicXMFLOAT3(Camera::GetTarget(), objC->GetPosition(), cameraMoveEase);
					cameraMoveEase += EASE_CAMERA_TIMER * 2;
				}
				else
				{
					cameraMoveEase = 1.0f;
					target = OgaJEase::easeOutCubicXMFLOAT3(Camera::GetTarget(), objC->GetPosition(), cameraMoveEase);
				}
				Camera::SetTarget(target);
			}
		}

		//キー(一旦放置)
		else
		{
			if (Input::isKeyTrigger(DIK_T))
			{
				cameraMoveEase = 0.0f;
				isTarget = true;
			}

			//カメラ系
			if (Input::isKey(DIK_RIGHT) || Input::isKey(DIK_LEFT) || Input::isKey(DIK_UP) || Input::isKey(DIK_DOWN))
			{
				if (Input::isKey(DIK_RIGHT))
				{
					cameraPos.x += MAX_CAMERA_MOVE_SPEED;
					targetPos.x -= MAX_CAMERA_MOVE_SPEED;
				}
				if (Input::isKey(DIK_LEFT))
				{
					cameraPos.x -= MAX_CAMERA_MOVE_SPEED;
					targetPos.x += MAX_CAMERA_MOVE_SPEED;
				}
				if (Input::isKey(DIK_UP))
				{
					cameraPos.y += MAX_CAMERA_MOVE_SPEED;
					targetPos.y -= MAX_CAMERA_MOVE_SPEED;
				}
				if (Input::isKey(DIK_DOWN))
				{
					cameraPos.y -= MAX_CAMERA_MOVE_SPEED;
					targetPos.y += MAX_CAMERA_MOVE_SPEED;
				}

				//距離と差分
				float diff = 0;

				//地面との仮判定
				if (cameraPos.y < 0) {
					//diff = cameraPos.y;
					//cameraPos.y = 0;
				}

				//セット
				XMFLOAT3 targetToCamera = OgaJHelper::CalcDirectionVec3(targetPos, cameraPos);
				targetToCamera = OgaJHelper::CalcNormalizeVec3(targetToCamera);
				XMFLOAT3 cameraToTarget = OgaJHelper::CalcDirectionVec3(cameraPos, targetPos);
				cameraToTarget = OgaJHelper::CalcNormalizeVec3(cameraToTarget);

				/*Camera::SetEye(XMFLOAT3(
					objC->GetPosition().x + targetToCamera.x * (MAX_DISTANCE + diff),
					objC->GetPosition().y + targetToCamera.y * (MAX_DISTANCE + diff),
					objC->GetPosition().z + targetToCamera.z * (MAX_DISTANCE + diff)
				));*/
				Camera::SetEye(cameraPos);
				/*Camera::SetTarget(XMFLOAT3(
					objC->GetPosition().x + cameraToTarget.x * (MAX_DISTANCE + diff),
					objC->GetPosition().y + cameraToTarget.y * (MAX_DISTANCE + diff),
					objC->GetPosition().z + cameraToTarget.z * (MAX_DISTANCE + diff)
				));*/
				Camera::SetTarget(targetPos);

				XMFLOAT3 eye = Camera::GetEye();
				XMFLOAT3 target = Camera::GetTarget();
			}

			//移動と回転
			if (Input::isKey(DIK_W) || Input::isKey(DIK_S) || Input::isKey(DIK_D) || Input::isKey(DIK_A))
			{
				XMFLOAT2 vec = { 0,0 };

				if (Input::isKey(DIK_W))
				{
					objCpos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					objCpos.z += cameraToPlayer.z * MAX_MOVE_SPEED;

					cameraPos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					cameraPos.z += cameraToPlayer.z * MAX_MOVE_SPEED;

					targetPos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					targetPos.z += cameraToPlayer.z * MAX_MOVE_SPEED;

					vec.x += 0;
					vec.y += 1;
				}

				if (Input::isKey(DIK_S))
				{
					objCpos.x += cameraToPlayer.x * MAX_MOVE_SPEED * -1;
					objCpos.z += cameraToPlayer.z * MAX_MOVE_SPEED * -1;

					cameraPos.x += cameraToPlayer.x * MAX_MOVE_SPEED * -1;
					cameraPos.z += cameraToPlayer.z * MAX_MOVE_SPEED * -1;

					targetPos.x += cameraToPlayer.x * MAX_MOVE_SPEED * -1;
					targetPos.z += cameraToPlayer.z * MAX_MOVE_SPEED * -1;

					vec.x += 0;
					vec.y += -1;
				}

				if (Input::isKey(DIK_D))
				{
					float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					cameraRad += XM_PI / 2;
					objCpos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					objCpos.z += cosf(cameraRad) * MAX_MOVE_SPEED;

					cameraPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					cameraPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;

					targetPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					targetPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;
					//OgaJHelper::ConvertToDegree(cameraRad);

					vec.x += -1;
					vec.y += 0;
				}

				if (Input::isKey(DIK_A)) {
					float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					cameraRad -= XM_PI / 2;
					objCpos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					objCpos.z += cosf(cameraRad) * MAX_MOVE_SPEED;

					cameraPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					cameraPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;

					targetPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					targetPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;
					//OgaJHelper::ConvertToDegree(cameraRad);

					vec.x += 1;
					vec.y += 0;
				}

				float deg = atan2(vec.y, vec.x);
				OgaJHelper::ConvertToDegree(deg);
				float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
				OgaJHelper::ConvertToDegree(cameraRad);

				Camera::SetEye(cameraPos);
				Camera::SetTarget(targetPos);
				objC->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
				obj1->SetRotation(XMFLOAT3(0, deg + cameraRad + 90.0f, 0));
			}
		}
	}

	else
	{
		//pad
		if (FunaInput::isConnect)
		{
			if (FunaInput::isPadTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
			{
				cameraMoveEase = 0.0f;
				isTarget = false;
				isEase = true;
			}

			if (0.3 < fabs(FunaInput::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
				0.3 < fabs(FunaInput::isPadThumb(XINPUT_THUMB_LEFTVERT)))
			{
				XMFLOAT3 vec = { 0,0,0 };
				vec.x += FunaInput::isPadThumb(XINPUT_THUMB_LEFTSIDE);
				vec.z += FunaInput::isPadThumb(XINPUT_THUMB_LEFTVERT);
				//XMFLOAT3 normalize = OgaJHelper::CalcNormalizeVec3(XMFLOAT3(vec.x, vec.y, 0));

				objCpos.x += vec.z * cameraToPlayer.x * MAX_MOVE_SPEED;
				objCpos.z += vec.z * cameraToPlayer.z * MAX_MOVE_SPEED;

				float rad = atan2(cameraToPlayer.x, cameraToPlayer.z);
				rad += XM_PI / 2;
				objCpos.x += vec.x * sinf(rad) * MAX_MOVE_SPEED;
				objCpos.z += vec.x * cosf(rad) * MAX_MOVE_SPEED;
			}
		}

		else
		{
			//切り替え
			if (Input::isKeyTrigger(DIK_T))
			{
				cameraMoveEase = 0.0f;
				isTarget = false;
			}

			//移動
			if (Input::isKey(DIK_W) || Input::isKey(DIK_S) || Input::isKey(DIK_D) || Input::isKey(DIK_A))
			{
				if (Input::isKey(DIK_W))
				{
					objCpos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					objCpos.z += cameraToPlayer.z * MAX_MOVE_SPEED;
				}

				if (Input::isKey(DIK_S))
				{
					objCpos.x += cameraToPlayer.x * MAX_MOVE_SPEED * -1;
					objCpos.z += cameraToPlayer.z * MAX_MOVE_SPEED * -1;
				}

				if (Input::isKey(DIK_D))
				{
					float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					cameraRad += XM_PI / 2;
					objCpos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					objCpos.z += cosf(cameraRad) * MAX_MOVE_SPEED;
				}

				if (Input::isKey(DIK_A)) {
					float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					cameraRad -= XM_PI / 2;
					objCpos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					objCpos.z += cosf(cameraRad) * MAX_MOVE_SPEED;
				}
			}
		}

		//ゴール地点
		const XMFLOAT3 GoalCameraTarget = {
			objA->GetPosition().x,
			objA->GetPosition().y - 30.0f,
			objA->GetPosition().z
		};
		const XMFLOAT3 GoalCameraEye = {
			objCpos.x + enemyToPlayer.x * MAX_DISTANCE,
			50.0f,
			objCpos.z + enemyToPlayer.z * MAX_DISTANCE,
		};

		XMFLOAT3 target;
		XMFLOAT3 eye;

		//カメラ挙動管理用
		if (cameraMoveEase < 1.0f)
		{
			target = OgaJEase::easeOutCubicXMFLOAT3(
				Camera::GetTarget(),
				GoalCameraTarget,
				cameraMoveEase);
			eye = OgaJEase::easeOutCubicXMFLOAT3(
				Camera::GetEye(),
				GoalCameraEye,
				cameraMoveEase);

			cameraMoveEase += EASE_CAMERA_TIMER;
		}

		else
		{
			cameraMoveEase = 1.0f;

			target = OgaJEase::easeOutCubicXMFLOAT3(
				Camera::GetTarget(),
				GoalCameraTarget,
				cameraMoveEase);
			eye = OgaJEase::easeOutCubicXMFLOAT3(
				Camera::GetEye(),
				GoalCameraEye,
				cameraMoveEase);
		}

		//セット
		Camera::SetTarget(target);
		Camera::SetEye(eye);

		//向き
		float pRadian = atan2(cosf(objC->GetRotation().z), sinf(objC->GetRotation().x));
		OgaJHelper::ConvertToDegree(pRadian);
		float cRadian = atan2(cameraToPlayer.z, cameraToPlayer.x);
		OgaJHelper::ConvertToDegree(cRadian);
		float rot = OgaJHelper::RotateEarliestArc(pRadian, cRadian) * -1;
		//float sub = objC->GetRotation().y - rot;
		float diff = 0;
		if (objC->GetRotation().y - rot > 0) { diff = 2.0f; }
		else if (objC->GetRotation().y - rot < 0) { diff = -2.0f; }
		objC->SetRotation(XMFLOAT3(
			objC->GetRotation().x,
			rot + diff,
			objC->GetRotation().z
		));
	}

#pragma endregion

	/*----------Update,Setter----------*/
	objB->SetPosition(XMFLOAT3(0.0f, ImguiControl::Imgui_ground_y, 0.0f));
	objC->SetPosition(objCpos);
	objCpos.x *= -1;
	objCpos.y *= -1;
	objCpos.z *= -1;
	obj1->SetPosition(objCpos);
	objD->SetPosition(Camera::GetTarget());

	Object::SetLight(light);
	FbxObject3D::SetLight(light);

	light->Update();

	objA->Update();
	objB->Update();
	objC->Update();
	objD->Update();

	if (Input::isKeyTrigger(DIK_1)) { fbxObj1->StopAnimation(); }
	if (Input::isKeyTrigger(DIK_2)) { fbxObj1->ResetAnimation(); }
	if (Input::isKeyTrigger(DIK_3)) { fbxObj1->ReplayAnimation(); }

	fbxObj1->Update();
	/*----------Update,Setter----------*/

	obj1->Update(true);
	obj2->Update();
	obj3->Update();
	obj4->Update();
	obj5->Update();
}

void GameScene::Draw(ID3D12Resource* texbuff)
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	//objA->Draw(pipelineSet);
	//objC->AddTexture(texbuff, modelC->GetDescHeap());
	objC->Draw(pipelineSet);
	objB->Draw(shadow2);
	//objB->Draw(pipelineSet);
	//objD->Draw(pipelineSet);

	//obj1->Draw();
	//obj2->Draw();
	//obj3->Draw();
	//obj4->Draw();
	//obj5->Draw();
	Object::PostDraw();

	fbxObj1->Draw(DirectXImportant::cmdList.Get());

	//Sprite::PreDraw(DirectXImportant::cmdList.Get());
	//GH1->Draw();
	//Sprite::PostDraw();
}

void GameScene::LuminanceDraw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	objC->Draw(pipelineSet);
	//obj2->Draw();
	//obj4->Draw();
	Object::PostDraw();
}

void GameScene::ShadowDraw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj1->Draw(shadow);
	//objC->Draw(shadow);
	Object::PostDraw();
}
