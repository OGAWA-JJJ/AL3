#include "Player.h"
#include "../3D/FbxLoader.h"
#include "../3D/FbxObject3D.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"
#include "../Input/Input.h"
#include "../DirectX/Camera.h"
#include "PipelineManager.h"
#include "ModelManager.h"

Player::Player()
{
	m_obb = {};
	m_pos = { 0,0,0 };
	m_cameraAngle = { 0,0,0 };
	m_rollingAngle = { 0,0,0 };
	m_animationType = STAND;
	m_animationTimer = 0;
	m_healTimer = 0;
	m_padState = 0;
	m_padRetentionTimer = 0;
	m_cameraMoveEase = 0.0f;
	m_cameraY = 0.0f;
	m_isTarget = false;
	m_isEase = false;
	m_isAttack = false;
	m_isAccept = true;

	m_hp = C_MAX_HP;
	m_mp = C_MAX_MP;
	m_stamina = C_MAX_STAMINA;
	m_power = C_MAX_POWER;

#pragma region ModelCreate

	//Obj
	obj_Sword = Object::Create(ModelManager::model_sword);
	obj_ShadowSword = Object::Create(ModelManager::model_sword);

	for (int i = 0; i < 28; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
	}
	obj_SwordBox = Object::Create(ModelManager::model_box);
	obj_HitBox = Object::Create(ModelManager::model_box);

	//Fbx
	fbxobj_StandMiku = new FbxObject3D();
	fbxobj_StandMiku->Init();
	fbxobj_StandMiku->SetModel(ModelManager::fbxmodel_standMiku);
	fbxobj_StandMiku->PlayAnimation();

	fbxobj_SlowRunMiku = new FbxObject3D();
	fbxobj_SlowRunMiku->Init();
	fbxobj_SlowRunMiku->SetModel(ModelManager::fbxmodel_slowRunMiku);
	fbxobj_SlowRunMiku->PlayAnimation();

	fbxobj_FastRunMiku = new FbxObject3D();
	fbxobj_FastRunMiku->Init();
	fbxobj_FastRunMiku->SetModel(ModelManager::fbxmodel_fastRunMiku);
	fbxobj_FastRunMiku->PlayAnimation();

	//Fbx(Shadow)
	fbxobj_StandShadowMiku = new FbxObject3D();
	fbxobj_StandShadowMiku->Init();
	fbxobj_StandShadowMiku->SetModel(ModelManager::fbxmodel_standMiku);
	fbxobj_StandShadowMiku->PlayAnimation();

	fbxobj_SlowRunShadowMiku = new FbxObject3D();
	fbxobj_SlowRunShadowMiku->Init();
	fbxobj_SlowRunShadowMiku->SetModel(ModelManager::fbxmodel_slowRunMiku);
	fbxobj_SlowRunShadowMiku->PlayAnimation();

	fbxobj_FastRunShadowMiku = new FbxObject3D();
	fbxobj_FastRunShadowMiku->Init();
	fbxobj_FastRunShadowMiku->SetModel(ModelManager::fbxmodel_fastRunMiku);
	fbxobj_FastRunShadowMiku->PlayAnimation();

	//Attack
	fbxobj_OneSwordAttack = new FbxObject3D();
	fbxobj_OneSwordAttack->Init();
	fbxobj_OneSwordAttack->SetModel(ModelManager::fbxmodel_oneSwrordAttack);
	fbxobj_OneSwordAttack->PlayAnimation();
	fbxobj_OneSwordAttack->StopAnimation();

	fbxobj_OneSwordAttackShadow = new FbxObject3D();
	fbxobj_OneSwordAttackShadow->Init();
	fbxobj_OneSwordAttackShadow->SetModel(ModelManager::fbxmodel_oneSwrordAttack);
	fbxobj_OneSwordAttackShadow->PlayAnimation();
	fbxobj_OneSwordAttackShadow->StopAnimation();

	//追加
	fbxobj_dieMiku = new FbxObject3D();
	fbxobj_dieMiku->Init();
	fbxobj_dieMiku->SetModel(ModelManager::fbxmodel_dieMiku);
	fbxobj_dieMiku->PlayAnimation();

	fbxobj_impactMiku = new FbxObject3D();
	fbxobj_impactMiku->Init();
	fbxobj_impactMiku->SetModel(ModelManager::fbxmodel_impactMiku);
	fbxobj_impactMiku->PlayAnimation();

	fbxobj_rollingMiku = new FbxObject3D();
	fbxobj_rollingMiku->Init();
	fbxobj_rollingMiku->SetModel(ModelManager::fbxmodel_rollingMiku);
	fbxobj_rollingMiku->PlayAnimation();

	//影
	fbxobj_dieShadowMiku = new FbxObject3D();
	fbxobj_dieShadowMiku->Init();
	fbxobj_dieShadowMiku->SetModel(ModelManager::fbxmodel_dieMiku);
	fbxobj_dieShadowMiku->PlayAnimation();

	fbxobj_impactShadowMiku = new FbxObject3D();
	fbxobj_impactShadowMiku->Init();
	fbxobj_impactShadowMiku->SetModel(ModelManager::fbxmodel_impactMiku);
	fbxobj_impactShadowMiku->PlayAnimation();

	fbxobj_rollingShadowMiku = new FbxObject3D();
	fbxobj_rollingShadowMiku->Init();
	fbxobj_rollingShadowMiku->SetModel(ModelManager::fbxmodel_rollingMiku);
	fbxobj_rollingShadowMiku->PlayAnimation();

#pragma endregion

#pragma region ModelInit

	const float Sword_Scale = 1.0f;
	const float Miku_Scale = 0.02f;

	//Obj
	obj_Sword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));
	obj_ShadowSword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));

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

	//追加
	fbxobj_dieMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_dieMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_dieMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_impactMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_impactMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_impactMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_rollingMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_rollingMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_rollingMiku->SetRotation(XMFLOAT3(0, 0, 0));

	//影
	fbxobj_dieShadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_dieShadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_dieShadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_impactShadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_impactShadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_impactShadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_rollingShadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_rollingShadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_rollingShadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

#pragma endregion

}

Player::~Player()
{
	delete obj_Sword;
	delete obj_ShadowSword;
	delete obj_Box[28];
	delete obj_SwordBox;
	delete obj_HitBox;

	delete fbxobj_StandMiku;
	delete fbxobj_SlowRunMiku;
	delete fbxobj_FastRunMiku;
	delete fbxobj_StandShadowMiku;
	delete fbxobj_SlowRunShadowMiku;
	delete fbxobj_FastRunShadowMiku;

	delete fbxobj_OneSwordAttack;
	delete fbxobj_OneSwordAttackShadow;

	delete fbxobj_dieMiku;
	delete fbxobj_impactMiku;
	delete fbxobj_rollingMiku;

	delete fbxobj_dieShadowMiku;
	delete fbxobj_impactShadowMiku;
	delete fbxobj_rollingShadowMiku;
}

void Player::Init()
{
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
	fbxobj_OneSwordAttack->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	fbxobj_dieMiku->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	fbxobj_impactMiku->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	fbxobj_rollingMiku->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));

	//Camera
	Camera::SetTarget(XMFLOAT3(
		fbxobj_StandMiku->GetPosition().x,
		fbxobj_StandMiku->GetPosition().y,
		fbxobj_StandMiku->GetPosition().z));

	m_cameraY = Camera::GetEye().y;

	//Test
	obj_SwordBox->SetPosition(DirectX::XMFLOAT3(
		0.0f,
		6.0f,
		0.0f
	));
}

void Player::Update(DirectX::XMFLOAT3 enemyPos)
{
	if (IsDead())
	{
		if (fbxobj_dieMiku->GetNowTime() == fbxobj_dieMiku->GetEndTime())
		{
			fbxobj_dieMiku->StopAnimation();
			fbxobj_dieShadowMiku->StopAnimation();
		}

		fbxobj_dieMiku->SetPosition(m_pos);
		fbxobj_dieShadowMiku->SetPosition(m_pos);
		fbxobj_dieMiku->Update();
		fbxobj_dieShadowMiku->Update();
		return;
	}

#pragma region Game

	if (m_animationType != DAMAGED)
	{
		//変数
		//m_pos = fbxobj_StandMiku->GetPosition();
		XMFLOAT3 cameraPos = Camera::GetEye();
		XMFLOAT3 targetPos = Camera::GetTarget();
		XMFLOAT3 enemyToPlayer = {};
		XMFLOAT3 cameraToPlayer = {};

		if (m_animationType == STAND)
		{
			enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_StandMiku->GetPosition());
			cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_StandMiku->GetPosition());
		}
		else if (m_animationType == SLOWRUN)
		{
			enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_SlowRunMiku->GetPosition());
			cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_SlowRunMiku->GetPosition());
		}
		else if (m_animationType == RUN)
		{
			enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_FastRunMiku->GetPosition());
			cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_FastRunMiku->GetPosition());
		}
		else if (m_animationType == ATTACK)
		{
			enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_OneSwordAttack->GetPosition());
			cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_OneSwordAttack->GetPosition());
		}
		else if (m_animationType == ROLLING)
		{
			enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_rollingMiku->GetPosition());
			cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_rollingMiku->GetPosition());
		}

		enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);
		cameraToPlayer = OgaJHelper::CalcNormalizeVec3(cameraToPlayer);

		//ターゲット非固定時処理
		if (!m_isTarget)
		{
			//pad
			if (Input::isPadConnect())
			{
				//ターゲット切り替え
				if (Input::isPadTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
				{
					m_cameraMoveEase = 0.0f;
					m_isTarget = true;
				}

				//移動
				if (m_animationType != ROLLING)
				{
					if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
						0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
					{
						bool isAttack = false;
						if (m_animationType == ATTACK) { isAttack = true; }

						if (!isAttack)
						{
							m_animationType = RUN;

							XMFLOAT3 vec = { 0,0,0 };
							vec.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
							vec.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

							m_pos.x += vec.z * cameraToPlayer.x * C_MAX_MOVE_SPEED;
							m_pos.z += vec.z * cameraToPlayer.z * C_MAX_MOVE_SPEED;

							float rad = atan2(cameraToPlayer.x, cameraToPlayer.z);
							rad += DirectX::XM_PI / 2;
							m_pos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
							m_pos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

							cameraPos.x += vec.z * cameraToPlayer.x * C_MAX_MOVE_SPEED;
							cameraPos.z += vec.z * cameraToPlayer.z * C_MAX_MOVE_SPEED;
							cameraPos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
							cameraPos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

							targetPos.x += vec.z * cameraToPlayer.x * C_MAX_MOVE_SPEED;
							targetPos.z += vec.z * cameraToPlayer.z * C_MAX_MOVE_SPEED;
							targetPos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
							targetPos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

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
							fbxobj_dieMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_impactMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_rollingMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));

							fbxobj_StandShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_SlowRunShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_FastRunShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_OneSwordAttackShadow->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_dieShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_impactShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
							fbxobj_rollingShadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
						}
					}

					//強制移行
					else { if (m_animationType != ATTACK) { m_animationType = STAND; } }
				}

				//カメラ
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
					xz += Input::isPadThumb(XINPUT_THUMB_RIGHTSIDE) * C_MAX_CAMERA_MOVE_SPEED;
					cameraPos.y += Input::isPadThumb(XINPUT_THUMB_RIGHTVERT) * C_MAX_CAMERA_MOVE_SPEED;
					m_cameraY += Input::isPadThumb(XINPUT_THUMB_RIGHTVERT) * C_MAX_CAMERA_MOVE_SPEED;

					//距離と差分
					float diff = 0;
					const float MAX_DIST = -50.0f;
					const float MAX_HEIGHT = 100.0f;
					if (m_cameraY < 0.1f)
					{
						if (m_cameraY < MAX_DIST) { m_cameraY = MAX_DIST; }
						diff = m_cameraY;

						//地面との仮判定
						cameraPos.y = 0.1f;
					}
					else if (m_cameraY > MAX_HEIGHT)
					{
						cameraPos.y = MAX_HEIGHT;
						m_cameraY = MAX_HEIGHT;
					}

					//radに戻す
					OgaJHelper::ConvertToRadian(xz);
					float s = sinf(xz);
					float c = cosf(xz);
					//sin.cos
					Camera::SetEye(XMFLOAT3(
						m_pos.x + s * (C_MAX_DISTANCE + diff),
						cameraPos.y,
						m_pos.z + c * (C_MAX_DISTANCE + diff)
					));
				}

				//ターゲット移動処理
				if (m_isEase)
				{
					XMFLOAT3 target;
					if (m_cameraMoveEase < 1.0f)
					{
						target = OgaJEase::easeOutCubicXMFLOAT3(Camera::GetTarget(), fbxobj_StandMiku->GetPosition(), m_cameraMoveEase);
						m_cameraMoveEase += C_EASE_CAMERA_TIMER * 2;
					}
					else
					{
						m_cameraMoveEase = 1.0f;
						target = OgaJEase::easeOutCubicXMFLOAT3(Camera::GetTarget(), fbxobj_StandMiku->GetPosition(), m_cameraMoveEase);
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
					m_isTarget = true;
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
					if (m_cameraY < 0.1f)
					{
						if (m_cameraY < MAX_DIST) { m_cameraY = MAX_DIST; }
						diff = m_cameraY;

						//地面との仮判定
						cameraPos.y = 0.1f;
					}
					else if (m_cameraY > MAX_HEIGHT)
					{
						cameraPos.y = MAX_HEIGHT;
						m_cameraY = MAX_HEIGHT;
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
					m_cameraMoveEase = 0.0f;
					m_isTarget = false;
					m_isEase = true;
				}

				if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
					0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
				{
					bool isAttack = false;
					if (m_animationType == ATTACK) { isAttack = true; }

					if (!isAttack)
					{
						m_animationType = RUN;

						XMFLOAT3 vec = { 0,0,0 };
						vec.x += Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
						vec.z += Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

						m_pos.x += vec.z * cameraToPlayer.x * C_MAX_MOVE_SPEED;
						m_pos.z += vec.z * cameraToPlayer.z * C_MAX_MOVE_SPEED;

						float rad = atan2(cameraToPlayer.x, cameraToPlayer.z);
						rad += DirectX::XM_PI / 2;
						m_pos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
						m_pos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;
					}
				}

				else { if (m_animationType != ATTACK) { m_animationType = STAND; } }
			}

			else
			{
				//切り替え
				/*if (Input::isKeyTrigger(DIK_T))
				{
					cameraMoveEase = 0.0f;
					//m_isTarget = false;
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
				enemyPos.x,
				enemyPos.y - 30.0f,
				enemyPos.z
			};
			const XMFLOAT3 GoalCameraEye = {
				m_pos.x + enemyToPlayer.x * C_MAX_DISTANCE,
				50.0f,
				m_pos.z + enemyToPlayer.z * C_MAX_DISTANCE,
			};

			XMFLOAT3 target;
			XMFLOAT3 eye;

			//カメラ挙動管理用
			if (m_cameraMoveEase < 1.0f)
			{
				target = OgaJEase::easeOutCubicXMFLOAT3(
					Camera::GetTarget(),
					GoalCameraTarget,
					m_cameraMoveEase);
				eye = OgaJEase::easeOutCubicXMFLOAT3(
					Camera::GetEye(),
					GoalCameraEye,
					m_cameraMoveEase);

				m_cameraMoveEase += C_EASE_CAMERA_TIMER;
			}

			else
			{
				m_cameraMoveEase = 1.0f;

				target = OgaJEase::easeOutCubicXMFLOAT3(
					Camera::GetTarget(),
					GoalCameraTarget,
					m_cameraMoveEase);
				eye = OgaJEase::easeOutCubicXMFLOAT3(
					Camera::GetEye(),
					GoalCameraEye,
					m_cameraMoveEase);
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
			fbxobj_dieMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_impactMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			//fbxobj_rollingMiku->SetRotation(fbxobj_StandMiku->GetRotation());

			fbxobj_StandShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_SlowRunShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_FastRunShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_OneSwordAttackShadow->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_dieShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_impactShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
			fbxobj_rollingShadowMiku->SetRotation(fbxobj_StandMiku->GetRotation());
		}
	}

#pragma endregion

	//ゲージ管理
	if (m_healTimer > 0)
	{
		m_healTimer--;
	}
	else
	{
		if (m_stamina < C_MAX_STAMINA)
		{
			m_stamina += C_HEAL_VOL;
		}
	}

	Setter();
	Input();
	OtherUpdate();
	CalcOBB();

	if (m_isAttack)
	{
		obj_Sword->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		obj_Sword->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	if (Input::isPadTrigger(XINPUT_GAMEPAD_LEFT_THUMB))
	{
		fbxobj_OneSwordAttack->ResetAnimation();
		m_animationType = STAND;
	}
}

void Player::Draw()
{
	if (IsDead())
	{
		fbxobj_dieMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		return;
	}

	if (ImguiControl::Imgui_playerDraw)
	{
		if (m_animationType == STAND)
		{
			fbxobj_StandMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		}
		else if (m_animationType == SLOWRUN)
		{
			fbxobj_SlowRunMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		}
		else if (m_animationType == RUN)
		{
			fbxobj_FastRunMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		}
		else if (m_animationType == ATTACK)
		{
			fbxobj_OneSwordAttack->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		}
		else if (m_animationType == DAMAGED)
		{
			fbxobj_impactMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		}
		else if (m_animationType == ROLLING)
		{
			fbxobj_rollingMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		}
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_Sword->Draw(PipelineManager::obj_normal);
	//obj_HitBox->Draw(PipelineManager::obj_normal);

	if (ImguiControl::Imgui_isOBBDraw && m_animationType != DAMAGED)
	{
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_normal);
		}
		obj_SwordBox->Draw(PipelineManager::obj_normal);
	}

	Object::PostDraw();
}

void Player::LuminanceDraw()
{
	if (IsDead())
	{
		return;
	}

	if (m_animationType == STAND)
	{
		fbxobj_StandMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == SLOWRUN)
	{
		fbxobj_SlowRunMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == RUN)
	{
		fbxobj_FastRunMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == ATTACK)
	{
		fbxobj_OneSwordAttack->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == DAMAGED)
	{
		fbxobj_impactMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == ROLLING)
	{
		fbxobj_rollingMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
}

void Player::ShadowDraw()
{
	if (IsDead())
	{
		fbxobj_dieShadowMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		return;
	}

	if (ImguiControl::Imgui_playerDraw)
	{
		if (m_animationType == STAND)
		{
			fbxobj_StandShadowMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		}
		else if (m_animationType == SLOWRUN)
		{
			fbxobj_SlowRunShadowMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		}
		else if (m_animationType == RUN)
		{
			fbxobj_FastRunShadowMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		}
		else if (m_animationType == ATTACK)
		{
			fbxobj_OneSwordAttackShadow->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		}
		else if (m_animationType == DAMAGED)
		{
			fbxobj_impactShadowMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		}
		else if (m_animationType == ROLLING)
		{
			fbxobj_rollingShadowMiku->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_shadow);
		}
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_ShadowSword->Draw(PipelineManager::obj_shadow);
	Object::PostDraw();
}

void Player::Input()
{
	//先行入力削除
	if (m_padRetentionTimer < C_MAX_PAD_RETENTION)
	{
		m_padRetentionTimer++;
	}
	else
	{
		m_padState = 0;
	}

	//先行入力
	if (Input::isPadTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		m_padRetentionTimer = 0;
		m_padState = XINPUT_GAMEPAD_RIGHT_SHOULDER;
	}
	else if (Input::isPadTrigger(XINPUT_GAMEPAD_B))
	{
		m_padRetentionTimer = 0;
		m_padState = XINPUT_GAMEPAD_B;
	}

	//攻撃
	if (m_stamina >= C_ATTACK_SUB_STAMINA)
	{
		if (m_padState == XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			if (m_animationType != ATTACK &&
				m_animationType != DAMAGED &&
				m_animationType != ROLLING)
			{
				m_isAccept = false;
				m_padState = 0;

				m_healTimer = C_HEAL_TIMER;
				m_stamina -= C_ATTACK_SUB_STAMINA;

				m_animationType = ATTACK;
				fbxobj_OneSwordAttack->ReplayAnimation();
				fbxobj_OneSwordAttackShadow->ReplayAnimation();
			}
		}

		//アニメーションが一定フレームを超えたら判定開始
		else if (m_animationType == ATTACK && !m_isAttack)
		{
			if (m_animationTimer >= C_ATTACK_COLLISION_TIMER)
			{
				m_animationTimer = 0;
				m_isAttack = true;
			}
			else { m_animationTimer++; }
		}
	}

	//回避
	if (m_stamina >= C_ROLLING_SUB_STAMINA && !m_isTarget)
	{
		if (m_padState == XINPUT_GAMEPAD_B)
		{
			if (m_isAccept &&
				m_animationType != DAMAGED &&
				m_animationType != ROLLING)
			{
				fbxobj_OneSwordAttack->ResetAnimation();
				fbxobj_OneSwordAttackShadow->ResetAnimation();

				m_padState = 0;

				m_healTimer = C_HEAL_TIMER;
				m_stamina -= C_ROLLING_SUB_STAMINA;

				m_animationType = ROLLING;
				fbxobj_rollingMiku->ReplayAnimation();
				m_isInvincible = true;

				//向きの計算
				if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
					0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
				{
					XMFLOAT3 l_deg = { 0,0,0 };
					l_deg.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
					l_deg.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);
					l_deg = OgaJHelper::CalcNormalizeVec3(l_deg);

					m_rollingAngle = { -l_deg.x,0,-l_deg.z };
					float l_degY = atan2(m_rollingAngle.x, m_rollingAngle.z) * 180.0f / DirectX::XM_PI;
					fbxobj_rollingMiku->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
					fbxobj_rollingShadowMiku->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
				}
				else
				{
					XMFLOAT3 l_deg = fbxobj_StandMiku->GetRotation();
					OgaJHelper::ConvertToRadian(l_deg.y);
					float l_radX = sinf(l_deg.y);
					float l_radZ = cosf(l_deg.y);

					m_rollingAngle = { l_radX,0,l_radZ };
				}
			}
		}
	}

	//アニメーション管理
	/*if (Input::isKeyTrigger(DIK_1))
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
	}*/
}

void Player::Setter()
{
	fbxobj_StandMiku->SetPosition(m_pos);
	fbxobj_SlowRunMiku->SetPosition(m_pos);
	fbxobj_FastRunMiku->SetPosition(m_pos);
	fbxobj_OneSwordAttack->SetPosition(m_pos);
	fbxobj_impactMiku->SetPosition(m_pos);
	//fbxobj_rollingMiku->SetPosition(m_pos);

	fbxobj_StandShadowMiku->SetPosition(m_pos);
	fbxobj_SlowRunShadowMiku->SetPosition(m_pos);
	fbxobj_FastRunShadowMiku->SetPosition(m_pos);
	fbxobj_OneSwordAttackShadow->SetPosition(m_pos);
	//fbxobj_dieShadowMiku->SetPosition(m_pos);
	fbxobj_impactShadowMiku->SetPosition(m_pos);
	//fbxobj_rollingShadowMiku->SetPosition(m_pos);
}

void Player::CalcOBB()
{
	obj_SwordBox->MultiMatrix(obj_Sword->GetMatWorld());
	obj_SwordBox->SetScale(DirectX::XMFLOAT3(
		ModelManager::model_sword->GetModelSize().x / 2.0f,
		ModelManager::model_sword->GetModelSize().y / 2.0f,
		ModelManager::model_sword->GetModelSize().z / 2.0f
	));

	obj_SwordBox->Update();
	//obj_HitBox->Update();

	OBB swordOBB;
	//攻撃時の当たり判定(OBB)←計算してるだけ
	if (m_animationType == ATTACK)
	{
		DirectX::XMFLOAT3 pos = {
			obj_SwordBox->GetMatWorld().r[3].m128_f32[0],
			obj_SwordBox->GetMatWorld().r[3].m128_f32[1],
			obj_SwordBox->GetMatWorld().r[3].m128_f32[2] };

		swordOBB.pos = pos;
		swordOBB.matRot = fbxobj_OneSwordAttack->GetMatrix();
		swordOBB.length = obj_SwordBox->GetScale();
		m_obb = swordOBB;
	}

	std::vector<OBB> l_obbs;
	for (int i = 0; i < matRot.size(); i++)
	{
		OBB l_obb;
		l_obb.pos = DirectX::XMFLOAT3(
			bones[i].second.r[3].m128_f32[0],
			bones[i].second.r[3].m128_f32[1],
			bones[i].second.r[3].m128_f32[2]);
		l_obb.matRot = matRot[i];
		l_obb.length = obj_Box[i]->GetScale();
		l_obbs.push_back(l_obb);
	}
	m_obbs = l_obbs;
}

void Player::OtherUpdate()
{
	if (m_animationType == STAND)
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
	else if (m_animationType == SLOWRUN)
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
	else if (m_animationType == RUN)
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
	else if (m_animationType == ATTACK)
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

		//攻撃アニメーション中断ローリング
		if (!m_isAccept)
		{
			if (fbxobj_OneSwordAttack->GetNowTime() >=
				fbxobj_OneSwordAttack->GetFrameTime() * C_ATTACK_COLLISION_ENDTIMER)
			{
				m_isAttack = false;
				m_isAccept = true;
			}
		}

		if (fbxobj_OneSwordAttack->GetNowTime() == fbxobj_OneSwordAttack->GetEndTime())
		{
			m_animationType = STAND;
			fbxobj_OneSwordAttack->ResetAnimation();
			fbxobj_OneSwordAttack->StopAnimation();

			fbxobj_OneSwordAttackShadow->ResetAnimation();
			fbxobj_OneSwordAttackShadow->StopAnimation();

			m_isAttack = false;
		}
	}
	else if (m_animationType == DAMAGED)
	{
		//fbxobj_impactMiku->SetPosition(m_pos);
		fbxobj_impactMiku->Update();
		fbxobj_impactShadowMiku->Update(true);

		std::vector<std::pair<std::string, DirectX::XMMATRIX>> affine = fbxobj_impactMiku->GetAffineTrans();
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->MultiMatrix(affine[i].second);
			obj_Box[i]->Update();
		}

		bones = fbxobj_impactMiku->GetAffineTrans();
		matRot = fbxobj_impactMiku->GetMatRots();

		obj_Sword->MultiMatrix(fbxobj_impactMiku->GetMatrix());
		obj_ShadowSword->MultiMatrix(fbxobj_impactMiku->GetMatrix());

		if (fbxobj_impactMiku->GetNowTime() == fbxobj_impactMiku->GetEndTime())
		{
			fbxobj_impactMiku->ResetAnimation();
			fbxobj_impactShadowMiku->ResetAnimation();
			m_animationType = STAND;
		}
	}
	else if (m_animationType == ROLLING)
	{
		//ローリング
		float l_addPosX = m_rollingAngle.x * C_MAX_MOVE_SPEED;
		float l_addPosZ = m_rollingAngle.z * C_MAX_MOVE_SPEED;
		m_pos.x += l_addPosX;
		m_pos.z += l_addPosZ;

		XMFLOAT3 l_cameraPos = Camera::GetEye();
		l_cameraPos.x += l_addPosX;
		l_cameraPos.z += l_addPosZ;
		Camera::SetEye(l_cameraPos);
		if (!m_isTarget)
		{
			Camera::SetTarget(m_pos);
		}

		fbxobj_rollingMiku->SetPosition(m_pos);
		fbxobj_rollingShadowMiku->SetPosition(m_pos);
		fbxobj_rollingMiku->Update();
		fbxobj_rollingShadowMiku->Update(true);

		std::vector<std::pair<std::string, DirectX::XMMATRIX>> affine = fbxobj_rollingMiku->GetAffineTrans();
		for (int i = 0; i < 28; i++)
		{
			obj_Box[i]->MultiMatrix(affine[i].second);
			obj_Box[i]->Update();
		}

		bones = fbxobj_rollingMiku->GetAffineTrans();
		matRot = fbxobj_rollingMiku->GetMatRots();

		obj_Sword->MultiMatrix(fbxobj_rollingMiku->GetMatrix());
		obj_ShadowSword->MultiMatrix(fbxobj_rollingMiku->GetMatrix());

		if (fbxobj_rollingMiku->GetNowTime() == fbxobj_rollingMiku->GetEndTime())
		{
			m_isInvincible = false;
			fbxobj_rollingMiku->ResetAnimation();
			fbxobj_rollingShadowMiku->ResetAnimation();
			m_animationType = STAND;
		}
	}

	obj_Sword->Update();
	obj_ShadowSword->SetRotation(obj_Sword->GetRotation());
	obj_ShadowSword->Update(true);
}

/*----------呼ぶやつ----------*/
bool Player::IsDead()
{
	if (m_hp <= 0) { return true; }
	return false;
}

void Player::HitAttack(int damage)
{
	m_hp -= damage;
	m_isInvincible = true;
	m_isAccept = true;
	m_animationType = DAMAGED;
	fbxobj_OneSwordAttack->ResetAnimation();
	fbxobj_OneSwordAttackShadow->ResetAnimation();
	fbxobj_rollingMiku->ResetAnimation();
	fbxobj_rollingShadowMiku->ResetAnimation();
	if (m_hp < 0) { m_hp = 0; }
	OutputDebugStringA("Hit!\n");
}
