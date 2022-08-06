#include "GameScene.h"
#include "../3D/FbxLoader.h"
#include "../3D/FbxObject3D.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"
#include "../Input/Input.h"
#include "../DirectX/Camera.h"
#include "../Math/OBBCollision.h"

GameScene::GameScene()
{
	animationType = STAND;
	cameraAngle = { 0,0,0 };

#pragma region Light

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

#pragma endregion

#pragma region Settings

	Object::SetLight(light);
	FbxObject3D::SetDevice(DirectXImportant::dev.Get());

#pragma endregion

#pragma region Pipeline

	ObjectInitData normalInit;
	normalInit.m_psEntryPoint = "PSmain";
	normalInit.m_vsEntryPoint = "VSmain";
	normal = Object::CreateGraphicsPipeline(normalInit);

	ObjectInitData shadowInit;
	shadowInit.m_psEntryPoint = "PSBlack";
	shadowInit.m_vsEntryPoint = "VSShadowMain";
	shadow = Object::CreateGraphicsPipeline(shadowInit);

	ObjectInitData receiveInit;
	receiveInit.m_psEntryPoint = "PSShadowMain";
	receiveInit.m_vsEntryPoint = "VSShadowMain";
	receiveShadow = Object::CreateGraphicsPipeline(receiveInit);

	FbxInitData fbxNormalInit;
	fbxNormalInit.m_vsEntryPoint = "PSmain";
	fbxNormalInit.m_vsEntryPoint = "VSmain";
	fbx_normal = FbxObject3D::CreateGraphicsPipeline(fbxNormalInit);

	FbxInitData fbxShadowInit;
	fbxShadowInit.m_psEntryPoint = "PSBlack";
	fbxShadowInit.m_vsEntryPoint = "VSShadowMain";
	fbx_shadow = FbxObject3D::CreateGraphicsPipeline(fbxShadowInit);

#pragma endregion

#pragma region ModelLoad

	model_Lich = Model::CreateFromObj("Lich");
	model_stage = Model::CreateFromObj("yuka");
	model_sponza = Model::CreateFromObj("triangle");
	model_eyeball = Model::CreateFromObj("eyeball");
	model_sword = Model::CreateFromObj("Sword");

	model_box = Model::CreateFromObj("TestBox");
	model_box2 = Model::CreateFromObj("TestBox");

	fbxmodel_standMiku = FbxLoader::GetInstance()->LoadModelFromFile("StandMiku");
	fbxmodel_slowRunMiku = FbxLoader::GetInstance()->LoadModelFromFile("SlowRunMiku");
	fbxmodel_fastRunMiku = FbxLoader::GetInstance()->LoadModelFromFile("RunMiku");

	fbxmodel_oneSwrordAttack = FbxLoader::GetInstance()->LoadModelFromFile("OneSwordAttack");

#pragma endregion

#pragma region ModelCreate

	//Obj
	obj_Lich = Object::Create(model_Lich);
	obj_Stage = Object::Create(model_stage);
	obj_EyeBall = Object::Create(model_eyeball);
	obj_Sword = Object::Create(model_sword);
	obj_ShadowSword = Object::Create(model_sword);
	obj_Sponza = Object::Create(model_sponza);

	for (int i = 0; i < 28; i++)
	{
		obj_Box[i] = Object::Create(model_box);
	}
	obj_SwordBox = Object::Create(model_box);
	obj_HitBox = Object::Create(model_box2);

	//Fbx
	fbxobj_StandMiku = new FbxObject3D();
	fbxobj_StandMiku->Init();
	fbxobj_StandMiku->SetModel(fbxmodel_standMiku);
	fbxobj_StandMiku->PlayAnimation();

	fbxobj_SlowRunMiku = new FbxObject3D();
	fbxobj_SlowRunMiku->Init();
	fbxobj_SlowRunMiku->SetModel(fbxmodel_slowRunMiku);
	fbxobj_SlowRunMiku->PlayAnimation();

	fbxobj_FastRunMiku = new FbxObject3D();
	fbxobj_FastRunMiku->Init();
	fbxobj_FastRunMiku->SetModel(fbxmodel_fastRunMiku);
	fbxobj_FastRunMiku->PlayAnimation();

	//Fbx(Shadow)
	fbxobj_StandShadowMiku = new FbxObject3D();
	fbxobj_StandShadowMiku->Init();
	fbxobj_StandShadowMiku->SetModel(fbxmodel_standMiku);
	fbxobj_StandShadowMiku->PlayAnimation();

	fbxobj_SlowRunShadowMiku = new FbxObject3D();
	fbxobj_SlowRunShadowMiku->Init();
	fbxobj_SlowRunShadowMiku->SetModel(fbxmodel_slowRunMiku);
	fbxobj_SlowRunShadowMiku->PlayAnimation();

	fbxobj_FastRunShadowMiku = new FbxObject3D();
	fbxobj_FastRunShadowMiku->Init();
	fbxobj_FastRunShadowMiku->SetModel(fbxmodel_fastRunMiku);
	fbxobj_FastRunShadowMiku->PlayAnimation();

	//Attack
	fbxobj_OneSwordAttack = new FbxObject3D();
	fbxobj_OneSwordAttack->Init();
	fbxobj_OneSwordAttack->SetModel(fbxmodel_oneSwrordAttack);
	fbxobj_OneSwordAttack->PlayAnimation();
	fbxobj_OneSwordAttack->StopAnimation();

	fbxobj_OneSwordAttackShadow = new FbxObject3D();
	fbxobj_OneSwordAttackShadow->Init();
	fbxobj_OneSwordAttackShadow->SetModel(fbxmodel_oneSwrordAttack);
	fbxobj_OneSwordAttackShadow->PlayAnimation();
	fbxobj_OneSwordAttackShadow->StopAnimation();

#pragma endregion

#pragma region ModelInit

	const float Lich_Scale = 20.0f;
	const float Stage_Scale = 100.0f;
	const float Sponza_scale = 1.0f;
	const float EyeBall_Scale = 2.0f;
	const float Sword_Scale = 1.0f;

	const float Miku_Scale = 0.02f;

	//Obj
	obj_Lich->SetScale(XMFLOAT3(Lich_Scale, Lich_Scale, Lich_Scale));
	obj_Stage->SetScale(XMFLOAT3(Stage_Scale + 100.0f, Stage_Scale, Stage_Scale));
	obj_EyeBall->SetScale(XMFLOAT3(EyeBall_Scale, EyeBall_Scale, EyeBall_Scale));
	obj_Sword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));
	obj_ShadowSword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));
	obj_Sponza->SetScale(XMFLOAT3(Sponza_scale, Sponza_scale, Sponza_scale));

	//Fbx
	fbxobj_StandMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_StandMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_StandMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_SlowRunMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_SlowRunMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_SlowRunMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_FastRunMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_FastRunMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_FastRunMiku->SetRotation(XMFLOAT3(0, 0, 0));

	//Fbx(Shadow)
	fbxobj_StandShadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_StandShadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_StandShadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_SlowRunShadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_SlowRunShadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_SlowRunShadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_FastRunShadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_FastRunShadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_FastRunShadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

	//Attack
	fbxobj_OneSwordAttack->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_OneSwordAttack->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_OneSwordAttack->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_OneSwordAttackShadow->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_OneSwordAttackShadow->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_OneSwordAttackShadow->SetRotation(XMFLOAT3(0, 0, 0));

#pragma endregion

}

GameScene::~GameScene()
{
	//書き忘れるからヤメロ！←スマートポインタにする
	delete model_Lich;
	delete obj_Lich;
	delete model_stage;
	delete obj_Stage;
	delete model_eyeball;
	delete obj_EyeBall;

	delete fbxmodel_standMiku;
	delete fbxobj_StandMiku;
	delete fbxmodel_slowRunMiku;
	delete fbxobj_SlowRunMiku;
	delete fbxmodel_fastRunMiku;
	delete fbxobj_FastRunMiku;

	delete fbxobj_StandShadowMiku;
	delete fbxobj_SlowRunShadowMiku;
	delete fbxobj_FastRunShadowMiku;
}

void GameScene::Init(ID3D12Resource* texbuff)
{
	//Lich
	obj_Lich->SetPosition(XMFLOAT3(0.0f, 35.0f, -400.0f));
	obj_Lich->SetRotation(XMFLOAT3(0, -100, 0));

	//skydome or sponza
	obj_Stage->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	obj_Stage->SetRotation(XMFLOAT3(0, 90, 0));
	obj_Sponza->SetRotation(XMFLOAT3(0, 90, 0));

	//EyeBall
	obj_EyeBall->SetBillboard(true);

	//Sword
	obj_Sword->SetPosition(XMFLOAT3(
		0,
		0,
		0));
	obj_Sword->SetRotation(XMFLOAT3(
		310.0f,
		300.0f,
		310.0f));

	//Miku
	fbxobj_StandMiku->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	fbxobj_SlowRunMiku->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	fbxobj_FastRunMiku->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	//Camera
	Camera::SetTarget(XMFLOAT3(
		fbxobj_StandMiku->GetPosition().x,
		fbxobj_StandMiku->GetPosition().y,
		fbxobj_StandMiku->GetPosition().z));

	XMFLOAT3 enemyToPlayer = OgaJHelper::CalcDirectionVec3(obj_Lich->GetPosition(), fbxobj_StandMiku->GetPosition());
	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);

	Camera::SetEye(XMFLOAT3(
		fbxobj_StandMiku->GetPosition().x + enemyToPlayer.x * MAX_DISTANCE,
		50.0f,
		fbxobj_StandMiku->GetPosition().z + enemyToPlayer.z * MAX_DISTANCE));

	cameraY = Camera::GetEye().y;

	obj_Stage->AddTexture(texbuff, model_stage->GetDescHeap());

	//Test
	obj_SwordBox->SetPosition(DirectX::XMFLOAT3(
		0.0f,
		6.0f,
		0.0f
	));
}

void GameScene::Update()
{
	//Input::Update();

	//Light情報を更新
	Object::SetLight(light);
	FbxObject3D::SetLight(light);

#pragma region Game
	//変数
	XMFLOAT3 playerPos = fbxobj_StandMiku->GetPosition();
	XMFLOAT3 cameraPos = Camera::GetEye();
	XMFLOAT3 targetPos = Camera::GetTarget();
	XMFLOAT3 enemyToPlayer = OgaJHelper::CalcDirectionVec3(obj_Lich->GetPosition(), fbxobj_StandMiku->GetPosition());
	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);
	XMFLOAT3 cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_StandMiku->GetPosition());
	cameraToPlayer = OgaJHelper::CalcNormalizeVec3(cameraToPlayer);

	if (!isTarget)
	{
		//pad
		if (Input::isPadConnect())
		{
			if (Input::isPadTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
			{
				cameraMoveEase = 0.0f;
				isTarget = true;
			}

			if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
				0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
			{
				bool isAttack = false;
				if (animationType == ATTACK) { isAttack = true; }

				if (!isAttack)
				{
					animationType = RUN;

					XMFLOAT3 vec = { 0,0,0 };
					vec.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
					vec.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

					playerPos.x += vec.z * cameraToPlayer.x * MAX_MOVE_SPEED;
					playerPos.z += vec.z * cameraToPlayer.z * MAX_MOVE_SPEED;

					float rad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					rad += DirectX::XM_PI / 2;
					playerPos.x += vec.x * sinf(rad) * MAX_MOVE_SPEED;
					playerPos.z += vec.x * cosf(rad) * MAX_MOVE_SPEED;

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
					fbxobj_StandMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_SlowRunMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_FastRunMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_OneSwordAttack->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));

					fbxobj_StandShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_SlowRunShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_FastRunShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_OneSwordAttackShadow->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
				}
			}

			else { if (animationType != ATTACK) { animationType = STAND; } }

			if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_RIGHTSIDE)) ||
				0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_RIGHTVERT)))
			{
				//ここをどうにかしたい,targetは最悪補完かける
				XMFLOAT3 objc = fbxobj_StandMiku->GetPosition();
				XMFLOAT3 eye = Camera::GetEye();

				XMFLOAT3 playerToCamera = OgaJHelper::CalcDirectionVec3(fbxobj_StandMiku->GetPosition(), Camera::GetEye());
				playerToCamera = OgaJHelper::CalcNormalizeVec3(playerToCamera);

				float xz = atan2(playerToCamera.x, playerToCamera.z);
				OgaJHelper::ConvertToDegree(xz);
				//計算
				xz += Input::isPadThumb(XINPUT_THUMB_RIGHTSIDE) * MAX_CAMERA_MOVE_SPEED;
				cameraPos.y += Input::isPadThumb(XINPUT_THUMB_RIGHTVERT) * MAX_CAMERA_MOVE_SPEED;
				cameraY += Input::isPadThumb(XINPUT_THUMB_RIGHTVERT) * MAX_CAMERA_MOVE_SPEED;

				//距離と差分
				float diff = 0;
				const float MAX_DIST = -50.0f;
				const float MAX_HEIGHT = 100.0f;
				if (cameraY < 0.1f)
				{
					if (cameraY < MAX_DIST) { cameraY = MAX_DIST; }
					diff = cameraY;

					//地面との仮判定
					cameraPos.y = 0.1f;
				}
				else if (cameraY > MAX_HEIGHT)
				{
					cameraPos.y = MAX_HEIGHT;
					cameraY = MAX_HEIGHT;
				}

				//radに戻す
				OgaJHelper::ConvertToRadian(xz);
				float s = sinf(xz);
				float c = cosf(xz);
				//sin.cos
				Camera::SetEye(XMFLOAT3(
					playerPos.x + s * (MAX_DISTANCE + diff),
					cameraPos.y,
					playerPos.z + c * (MAX_DISTANCE + diff)
				));
			}

			if (isEase)
			{
				XMFLOAT3 target;
				if (cameraMoveEase < 1.0f)
				{
					target = OgaJEase::easeOutCubicXMFLOAT3(Camera::GetTarget(), fbxobj_StandMiku->GetPosition(), cameraMoveEase);
					cameraMoveEase += EASE_CAMERA_TIMER * 2;
				}
				else
				{
					cameraMoveEase = 1.0f;
					target = OgaJEase::easeOutCubicXMFLOAT3(Camera::GetTarget(), fbxobj_StandMiku->GetPosition(), cameraMoveEase);
				}
				Camera::SetTarget(target);
			}
		}

		//キー(一旦放置)
		else
		{
			/*if (Input::isKeyTrigger(DIK_T))
			{
				cameraMoveEase = 0.0f;
				isTarget = true;
			}*/

			//カメラ系
			/*if (Input::isKey(DIK_RIGHT) || Input::isKey(DIK_LEFT) || Input::isKey(DIK_UP) || Input::isKey(DIK_DOWN))
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
				const float MAX_DIST = -50.0f;
				const float MAX_HEIGHT = 100.0f;
				if (cameraY < 0.1f)
				{
					if (cameraY < MAX_DIST) { cameraY = MAX_DIST; }
					diff = cameraY;

					//地面との仮判定
					cameraPos.y = 0.1f;
				}
				else if (cameraY > MAX_HEIGHT)
				{
					cameraPos.y = MAX_HEIGHT;
					cameraY = MAX_HEIGHT;
				}

				//セット
				XMFLOAT3 targetToCamera = OgaJHelper::CalcDirectionVec3(targetPos, cameraPos);
				targetToCamera = OgaJHelper::CalcNormalizeVec3(targetToCamera);
				XMFLOAT3 cameraToTarget = OgaJHelper::CalcDirectionVec3(cameraPos, targetPos);
				cameraToTarget = OgaJHelper::CalcNormalizeVec3(cameraToTarget);

				Camera::SetEye(XMFLOAT3(
					fbxobj_StandMiku->GetPosition().x + targetToCamera.x * (MAX_DISTANCE + diff),
					fbxobj_StandMiku->GetPosition().y + targetToCamera.y * (MAX_DISTANCE + diff),
					fbxobj_StandMiku->GetPosition().z + targetToCamera.z * (MAX_DISTANCE + diff)
				));
				//Camera::SetEye(cameraPos);
				Camera::SetTarget(XMFLOAT3(
					fbxobj_StandMiku->GetPosition().x + cameraToTarget.x * (MAX_DISTANCE + diff),
					fbxobj_StandMiku->GetPosition().y + cameraToTarget.y * (MAX_DISTANCE + diff),
					fbxobj_StandMiku->GetPosition().z + cameraToTarget.z * (MAX_DISTANCE + diff)
				));
				//Camera::SetTarget(targetPos);

				XMFLOAT3 eye = Camera::GetEye();
				XMFLOAT3 target = Camera::GetTarget();
			}*/

			//移動と回転
			/*if (Input::isKey(DIK_W) || Input::isKey(DIK_S) || Input::isKey(DIK_D) || Input::isKey(DIK_A))
			{
				//animationType = RUN;

				XMFLOAT2 vec = { 0,0 };

				if (Input::isKey(DIK_W))
				{
					playerPos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					playerPos.z += cameraToPlayer.z * MAX_MOVE_SPEED;

					cameraPos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					cameraPos.z += cameraToPlayer.z * MAX_MOVE_SPEED;

					targetPos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					targetPos.z += cameraToPlayer.z * MAX_MOVE_SPEED;

					vec.x += 0;
					vec.y += 1;
				}

				if (Input::isKey(DIK_S))
				{
					playerPos.x += cameraToPlayer.x * MAX_MOVE_SPEED * -1;
					playerPos.z += cameraToPlayer.z * MAX_MOVE_SPEED * -1;

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
					cameraRad += DirectX::XM_PI / 2;
					playerPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					playerPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;

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
					cameraRad -= DirectX::XM_PI / 2;
					playerPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					playerPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;

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

				//Camera::SetEye(cameraPos);
				//Camera::SetTarget(targetPos);
				//fbxobj_StandMiku->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
				//fbxobj_SlowRunMiku->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
				//fbxobj_FastRunMiku->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));

				//fbxobj_StandShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
				//fbxobj_SlowRunShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
				//fbxobj_FastRunShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
			}*/

			//else { animationType = STAND; }
		}
	}

	else
	{
		//pad
		if (Input::isPadConnect())
		{
			if (Input::isPadTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
			{
				cameraMoveEase = 0.0f;
				isTarget = false;
				isEase = true;
			}

			if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
				0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
			{
				bool isAttack = false;
				if (animationType == ATTACK) { isAttack = true; }

				if (!isAttack)
				{
					animationType = RUN;

					XMFLOAT3 vec = { 0,0,0 };
					vec.x += Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
					vec.z += Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

					playerPos.x += vec.z * cameraToPlayer.x * MAX_MOVE_SPEED;
					playerPos.z += vec.z * cameraToPlayer.z * MAX_MOVE_SPEED;

					float rad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					rad += DirectX::XM_PI / 2;
					playerPos.x += vec.x * sinf(rad) * MAX_MOVE_SPEED;
					playerPos.z += vec.x * cosf(rad) * MAX_MOVE_SPEED;
				}
			}

			else { if (animationType != ATTACK) { animationType = STAND; } }
		}

		else
		{
			//切り替え
			/*if (Input::isKeyTrigger(DIK_T))
			{
				cameraMoveEase = 0.0f;
				//isTarget = false;
			}*/

			//移動
			/*if (Input::isKey(DIK_W) || Input::isKey(DIK_S) || Input::isKey(DIK_D) || Input::isKey(DIK_A))
			{
				animationType = RUN;

				if (Input::isKey(DIK_W))
				{
					playerPos.x += cameraToPlayer.x * MAX_MOVE_SPEED;
					playerPos.z += cameraToPlayer.z * MAX_MOVE_SPEED;
				}

				if (Input::isKey(DIK_S))
				{
					playerPos.x += cameraToPlayer.x * MAX_MOVE_SPEED * -1;
					playerPos.z += cameraToPlayer.z * MAX_MOVE_SPEED * -1;
				}

				if (Input::isKey(DIK_D))
				{
					float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					cameraRad += DirectX::XM_PI / 2;
					playerPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					playerPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;
				}

				if (Input::isKey(DIK_A)) {
					float cameraRad = atan2(cameraToPlayer.x, cameraToPlayer.z);
					cameraRad -= DirectX::XM_PI / 2;
					playerPos.x += sinf(cameraRad) * MAX_MOVE_SPEED;
					playerPos.z += cosf(cameraRad) * MAX_MOVE_SPEED;
				}
			}*/

			//else { animationType = STAND; }
		}

		//ゴール地点
		const XMFLOAT3 GoalCameraTarget = {
			obj_Lich->GetPosition().x,
			obj_Lich->GetPosition().y - 30.0f,
			obj_Lich->GetPosition().z
		};
		const XMFLOAT3 GoalCameraEye = {
			playerPos.x + enemyToPlayer.x * MAX_DISTANCE,
			50.0f,
			playerPos.z + enemyToPlayer.z * MAX_DISTANCE,
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
		float pRadian = atan2(cosf(fbxobj_StandMiku->GetRotation().z), sinf(fbxobj_StandMiku->GetRotation().x));
		OgaJHelper::ConvertToDegree(pRadian);
		float cRadian = atan2(cameraToPlayer.z, cameraToPlayer.x);
		OgaJHelper::ConvertToDegree(cRadian);
		float rot = OgaJHelper::RotateEarliestArc(pRadian, cRadian) * -1;
		float diff = 0;
		if (fbxobj_StandMiku->GetRotation().y - rot > 0) { diff = 2.0f; }
		else if (fbxobj_StandMiku->GetRotation().y - rot < 0) { diff = -2.0f; }
		fbxobj_StandMiku->SetRotation(XMFLOAT3(
			fbxobj_StandMiku->GetRotation().x,
			rot + diff,
			fbxobj_StandMiku->GetRotation().z
		));

		fbxobj_SlowRunMiku->SetRotation(fbxobj_StandMiku->GetRotation());
		fbxobj_FastRunMiku->SetRotation(fbxobj_StandMiku->GetRotation());
		fbxobj_OneSwordAttack->SetRotation(fbxobj_StandMiku->GetRotation());

		fbxobj_StandShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
		fbxobj_SlowRunShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
		fbxobj_FastRunShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
		fbxobj_OneSwordAttackShadow->SetRotation(fbxobj_StandMiku->GetRotation());
	}

#pragma endregion

	//アニメーション管理
	if (Input::isKeyTrigger(DIK_1))
	{
		fbxobj_StandMiku->StopAnimation();
		fbxobj_SlowRunMiku->StopAnimation();
		fbxobj_FastRunMiku->StopAnimation();
		fbxobj_StandShadowMiku->StopAnimation();
		fbxobj_SlowRunShadowMiku->StopAnimation();
		fbxobj_FastRunShadowMiku->StopAnimation();
		fbxobj_OneSwordAttack->StopAnimation();
		fbxobj_OneSwordAttackShadow->StopAnimation();
	}
	if (Input::isKeyTrigger(DIK_2))
	{
		fbxobj_StandMiku->ResetAnimation();
		fbxobj_SlowRunMiku->ResetAnimation();
		fbxobj_FastRunMiku->ResetAnimation();
		fbxobj_StandShadowMiku->ResetAnimation();
		fbxobj_SlowRunShadowMiku->ResetAnimation();
		fbxobj_FastRunShadowMiku->ResetAnimation();
		fbxobj_OneSwordAttack->ResetAnimation();
		fbxobj_OneSwordAttackShadow->ResetAnimation();
	}
	if (Input::isKeyTrigger(DIK_3))
	{
		fbxobj_StandMiku->ReplayAnimation();
		fbxobj_SlowRunMiku->ReplayAnimation();
		fbxobj_FastRunMiku->ReplayAnimation();
		fbxobj_StandShadowMiku->ReplayAnimation();
		fbxobj_SlowRunShadowMiku->ReplayAnimation();
		fbxobj_FastRunShadowMiku->ReplayAnimation();
		fbxobj_OneSwordAttack->ReplayAnimation();
		fbxobj_OneSwordAttackShadow->ReplayAnimation();
	}

	//Target描画用
	if (Input::isPadTrigger(XINPUT_GAMEPAD_Y))
	{
		if (ImguiControl::Imgui_targetDraw) { ImguiControl::Imgui_targetDraw = false; }
		else { ImguiControl::Imgui_targetDraw = true; }
	}

	//Target移動用
	if (Input::isPadTrigger(XINPUT_GAMEPAD_X))
	{
		if (ImguiControl::Imgui_isTargetMove) { ImguiControl::Imgui_isTargetMove = false; }
		else { ImguiControl::Imgui_isTargetMove = true; }
	}

	if (ImguiControl::Imgui_isTargetMove)
	{
		count += 0.02f;
		obj_Lich->SetPosition(XMFLOAT3(
			sinf(count) * 175.0f,
			obj_Lich->GetPosition().y,
			obj_Lich->GetPosition().z
		));
	}

	/*----------Update,Setter----------*/
#pragma region Setter

	//obj_Stage->SetPosition(XMFLOAT3(0.0f, ImguiControl::Imgui_ground_y, 0.0f));
	//obj_Sword->SetPosition(XMFLOAT3(
	//	ImguiControl::Imgui_swordPos_x,
	//	ImguiControl::Imgui_swordPos_y,
	//	ImguiControl::Imgui_swordPos_z));

	//obj_Sword->SetRotation(XMFLOAT3(
	//	ImguiControl::Imgui_swordRot_x,
	//	ImguiControl::Imgui_swordRot_y,
	//	ImguiControl::Imgui_swordRot_z));
	obj_EyeBall->SetPosition(Camera::GetTarget());

	fbxobj_StandMiku->SetPosition(playerPos);
	fbxobj_SlowRunMiku->SetPosition(playerPos);
	fbxobj_FastRunMiku->SetPosition(playerPos);
	fbxobj_OneSwordAttack->SetPosition(playerPos);

	fbxobj_StandShadowMiku->SetPosition(playerPos);
	fbxobj_SlowRunShadowMiku->SetPosition(playerPos);
	fbxobj_FastRunShadowMiku->SetPosition(playerPos);
	fbxobj_OneSwordAttackShadow->SetPosition(playerPos);

	//光
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

	XMFLOAT3 shadowCameraPos = light->GetShadowLigitEye();
	light->SetShadowLigitEye(XMFLOAT3(
		fbxobj_StandMiku->GetPosition().x,
		shadowCameraPos.y,
		fbxobj_StandMiku->GetPosition().z));
	light->SetShadowLigitTarget(fbxobj_StandMiku->GetPosition());

#pragma endregion

#pragma region Update

	obj_Lich->Update();
	obj_Stage->Update();
	obj_Sponza->Update();
	obj_EyeBall->Update();

	light->Update();

#pragma endregion
	/*----------Update,Setter----------*/

	//攻撃用
	if (animationType != ATTACK)
	{
		if (Input::isPadTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			animationType = ATTACK;
			fbxobj_OneSwordAttack->ReplayAnimation();
			fbxobj_OneSwordAttackShadow->ReplayAnimation();
		}
	}

	/*---例外---*/
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> bones;
	std::vector<DirectX::XMMATRIX> matRot;

	if (animationType == STAND)
	{
		fbxobj_StandMiku->Update();
		fbxobj_StandShadowMiku->Update(true);
		obj_Sword->MultiMatrix(fbxobj_StandMiku->GetMatrix());
		obj_ShadowSword->MultiMatrix(fbxobj_StandMiku->GetMatrix());

		std::vector<std::pair<std::string, DirectX::XMMATRIX>> affine = fbxobj_StandMiku->GetAffineTrans();
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->MultiMatrix(affine[i].second);
			obj_Box[i]->Update();
		}

		bones = fbxobj_StandMiku->GetAffineTrans();
		matRot = fbxobj_StandMiku->GetMatRots();
	}
	else if (animationType == SLOWRUN)
	{
		fbxobj_SlowRunMiku->Update();
		fbxobj_SlowRunShadowMiku->Update(true);
		obj_Sword->MultiMatrix(fbxobj_SlowRunMiku->GetMatrix());
		obj_ShadowSword->MultiMatrix(fbxobj_SlowRunMiku->GetMatrix());

		std::vector<std::pair<std::string, DirectX::XMMATRIX>> affine = fbxobj_SlowRunMiku->GetAffineTrans();
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->MultiMatrix(affine[i].second);
			obj_Box[i]->Update();
		}
	}
	else if (animationType == RUN)
	{
		fbxobj_FastRunMiku->Update();
		fbxobj_FastRunShadowMiku->Update(true);
		obj_Sword->MultiMatrix(fbxobj_FastRunMiku->GetMatrix());
		obj_ShadowSword->MultiMatrix(fbxobj_FastRunMiku->GetMatrix());

		std::vector<std::pair<std::string, DirectX::XMMATRIX>> affine = fbxobj_FastRunMiku->GetAffineTrans();
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->MultiMatrix(affine[i].second);
			obj_Box[i]->Update();
		}

		bones = fbxobj_FastRunMiku->GetAffineTrans();
		matRot = fbxobj_FastRunMiku->GetMatRots();
	}
	else if (animationType == ATTACK)
	{
		//攻撃用
		fbxobj_OneSwordAttack->Update();
		fbxobj_OneSwordAttackShadow->Update(true);

		std::vector<std::pair<std::string, DirectX::XMMATRIX>> affine = fbxobj_OneSwordAttack->GetAffineTrans();
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->MultiMatrix(affine[i].second);
			obj_Box[i]->Update();
		}

		bones = fbxobj_OneSwordAttack->GetAffineTrans();
		matRot = fbxobj_OneSwordAttack->GetMatRots();

		obj_Sword->MultiMatrix(fbxobj_OneSwordAttack->GetMatrix());
		obj_ShadowSword->MultiMatrix(fbxobj_OneSwordAttack->GetMatrix());
		if (fbxobj_OneSwordAttack->GetNowTime() == fbxobj_OneSwordAttack->GetEndTime())
		{
			animationType = STAND;
			fbxobj_OneSwordAttack->ResetAnimation();
			fbxobj_OneSwordAttack->StopAnimation();

			fbxobj_OneSwordAttackShadow->ResetAnimation();
			fbxobj_OneSwordAttackShadow->StopAnimation();
		}
	}

	obj_Sword->Update();
	obj_ShadowSword->SetRotation(obj_Sword->GetRotation());
	obj_ShadowSword->Update(true);
	/*---例外---*/

	//Test
	obj_SwordBox->MultiMatrix(obj_Sword->GetMatWorld());
	//obj_SwordBox->SetPosition(DirectX::XMFLOAT3(
	//	ImguiControl::swordPos_x,
	//	ImguiControl::swordPos_y,
	//	ImguiControl::swordPos_z
	//));
	obj_SwordBox->SetScale(DirectX::XMFLOAT3(
		model_sword->GetModelSize().x / 2.0f,
		model_sword->GetModelSize().y / 2.0f,
		model_sword->GetModelSize().z / 2.0f
	));

	obj_HitBox->SetScale(DirectX::XMFLOAT3(
		ImguiControl::boxSca1_x,
		ImguiControl::boxSca1_y,
		ImguiControl::boxSca1_z
	));
	obj_HitBox->SetPosition(DirectX::XMFLOAT3(
		ImguiControl::boxPos1_x,
		ImguiControl::boxPos1_y,
		ImguiControl::boxPos1_z
	));
	obj_HitBox->SetRotation(DirectX::XMFLOAT3(
		ImguiControl::boxRot1_x,
		ImguiControl::boxRot1_y,
		ImguiControl::boxRot1_z
	));

	obj_SwordBox->Update();
	obj_HitBox->Update();

	OBB hitObb;
	hitObb.pos = obj_HitBox->GetPosition();
	hitObb.matRot = obj_HitBox->GetMatRot();
	hitObb.length = obj_HitBox->GetScale();

	OBB obb[28];
	OBB swordOBB;
	isHit = false;
	//攻撃以外の時の当たり判定(OBB)
	if (animationType != ATTACK)
	{
		for (int i = 0; i < 28; i++)
		{
			DirectX::XMFLOAT3 pos = {
				bones[i].second.r[3].m128_f32[0],
				bones[i].second.r[3].m128_f32[1],
				bones[i].second.r[3].m128_f32[2] };

			obb[i].pos = pos;
			obb[i].matRot = matRot[i];
			obb[i].length = obj_Box[i]->GetScale();
			if (OBBCollision::CollisionOBBs(hitObb, obb[i]))
			{
				isHit = true;
			}
		}
	}
	//攻撃時の当たり判定(OBB)
	else
	{
		DirectX::XMFLOAT3 pos = {
			obj_SwordBox->GetMatWorld().r[3].m128_f32[0],
			obj_SwordBox->GetMatWorld().r[3].m128_f32[1],
			obj_SwordBox->GetMatWorld().r[3].m128_f32[2] };

		swordOBB.pos = pos;
		swordOBB.matRot = fbxobj_OneSwordAttack->GetMatrix();
		swordOBB.length = obj_SwordBox->GetScale();
		if (OBBCollision::CollisionOBBs(hitObb, swordOBB))
		{
			if (ImguiControl::Imgui_isHitStopAnimation)
			{
				fbxobj_OneSwordAttack->StopAnimation();
				fbxobj_OneSwordAttackShadow->StopAnimation();
			}
			isHit = true;
		}
	}

	if (Input::isPadTrigger(XINPUT_GAMEPAD_LEFT_THUMB))
	{
		fbxobj_OneSwordAttack->ResetAnimation();
		animationType = STAND;
	}
}

void GameScene::Draw()
{
	if (ImguiControl::Imgui_playerDraw)
	{
		if (animationType == STAND)
		{
			fbxobj_StandMiku->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
		}
		else if (animationType == SLOWRUN)
		{
			fbxobj_SlowRunMiku->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
		}
		else if (animationType == RUN)
		{
			fbxobj_FastRunMiku->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
		}
		else if (animationType == ATTACK)
		{
			fbxobj_OneSwordAttack->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
		}
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_Lich->Draw(normal);
	obj_Sword->Draw(normal);
	obj_Sponza->Draw(normal);
	obj_HitBox->Draw(normal);

	if (ImguiControl::Imgui_isOBBDraw)
	{
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->Draw(normal);
		}
		obj_SwordBox->Draw(normal);
	}

	if (ImguiControl::Imgui_targetDraw) { obj_EyeBall->Draw(normal); }

	obj_Stage->Draw(receiveShadow);
	Object::PostDraw();
}

void GameScene::LuminanceDraw()
{
	if (animationType == STAND)
	{
		fbxobj_StandMiku->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
	}
	else if (animationType == SLOWRUN)
	{
		fbxobj_SlowRunMiku->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
	}
	else if (animationType == RUN)
	{
		fbxobj_FastRunMiku->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
	}
	else if (animationType == ATTACK)
	{
		fbxobj_OneSwordAttack->Draw(DirectXImportant::cmdList.Get(), fbx_normal);
	}
}

void GameScene::ShadowDraw()
{
	if (ImguiControl::Imgui_playerDraw)
	{
		if (animationType == STAND)
		{
			fbxobj_StandShadowMiku->Draw(DirectXImportant::cmdList.Get(), fbx_shadow);
		}
		else if (animationType == SLOWRUN)
		{
			fbxobj_SlowRunShadowMiku->Draw(DirectXImportant::cmdList.Get(), fbx_shadow);
		}
		else if (animationType == RUN)
		{
			fbxobj_FastRunShadowMiku->Draw(DirectXImportant::cmdList.Get(), fbx_shadow);
		}
		else if (animationType == ATTACK)
		{
			fbxobj_OneSwordAttackShadow->Draw(DirectXImportant::cmdList.Get(), fbx_shadow);
		}
	}

	//攻撃が入ってないよ！←入れた

	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_ShadowSword->Draw(shadow);
	Object::PostDraw();
}
