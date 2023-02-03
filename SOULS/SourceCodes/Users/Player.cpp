#define NOMINMAX

#include "Player.h"
#include "PipelineManager.h"
#include "ModelManager.h"
#include "Config.h"

#include "../Math/OgaJHelper.h"
#include "../Input/Input.h"
#include "../DirectX/Camera.h"

#include <fstream>
#include <iostream>

rapidjson::Document m_pDoc(rapidjson::kObjectType);

Player::Player()
{
	pManagerSword.Init(ModelManager::model_box2);
	pManagerEstus.Init(ModelManager::model_box2);

	pManagerLight0.Init(ModelManager::model_box2);
	pManagerLight1.Init(ModelManager::model_box2);
	pManagerLight2.Init(ModelManager::model_box2);
	pManagerLight3.Init(ModelManager::model_box2);

	trail.Init();

	std::ifstream ifs("Resources/Config/PlayerDataConfig.json");
	std::string str;
	std::string txt;
	while (getline(ifs, str))
	{
		txt += str;
	}
	m_pDoc.Parse(txt.c_str());
	InportJson();

#pragma region ModelCreate

	//Obj
	obj_Sword = Object::Create(ModelManager::model_sword);

	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
	}
	obj_SwordBox = Object::Create(ModelManager::model_box);
	obj_Helmet = Object::Create(ModelManager::model_helmet);

	const float size = 8.0f;
	obj_Helmet->SetScale(DirectX::XMFLOAT3(size, size, size));
	obj_Helmet->SetPosition(DirectX::XMFLOAT3(0.0f, 3.03f, 0.0f));

	//Fbx
	fbxobj_miku = FbxObjects::Create(ModelManager::fbxmodel_mikus);
	fbxobj_miku->PlayAnimation(STAND);
	fbxobj_miku->PlayAnimation(SLOWRUN);
	fbxobj_miku->PlayAnimation(RUN);
	fbxobj_miku->PlayAnimation(BACK_RUN);
	fbxobj_miku->PlayAnimation(R_RUN);
	fbxobj_miku->PlayAnimation(L_RUN);

	fbxobj_miku->StopAnimation(NORMAL_ATTACK_1);
	fbxobj_miku->SetLoopAnimation(NORMAL_ATTACK_1, false);

	fbxobj_miku->StopAnimation(NORMAL_ATTACK_2);
	fbxobj_miku->SetLoopAnimation(NORMAL_ATTACK_2, false);

	fbxobj_miku->StopAnimation(NORMAL_ATTACK_3);
	fbxobj_miku->SetLoopAnimation(NORMAL_ATTACK_3, false);

	fbxobj_miku->PlayAnimation(DIE);
	fbxobj_miku->SetLoopAnimation(DIE, false);

	fbxobj_miku->PlayAnimation(DAMAGED);
	//fbxobj_miku->SetLoopAnimation(DAMAGED, false);

	fbxobj_miku->StopAnimation(ROLLING);
	fbxobj_miku->SetLoopAnimation(ROLLING, false);

	fbxobj_miku->PlayAnimation(HEAL);

#pragma endregion

#pragma region ModelInit

	const float Sword_Scale = 2.0f;
	const float Miku_Scale = 0.02f;

	//Obj
	obj_Sword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));

	//Fbx
	fbxobj_miku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_miku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_miku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_miku->SetDrawSkipNum(3);
	fbxobj_miku->SetDrawSkipNum(4);
	fbxobj_miku->SetDrawSkipNum(5);
	fbxobj_miku->SetDrawSkipNum(6);
	fbxobj_miku->SetDrawSkipNum(7);
	fbxobj_miku->SetDrawSkipNum(10);
	fbxobj_miku->SetDrawSkipNum(12);
	fbxobj_miku->SetDrawSkipNum(14);

#pragma endregion
}

Player::~Player()
{
}

void Player::Init()
{
#pragma region Init

	m_obb = {};
	m_pos = { 0,0,0 };
	m_cameraAngle = { 0,0,0 };
	m_rollingAngle = { 0,0,0 };
	m_cameraToPlayer = { 0,0,0 };
	m_cameraTarget = { 0,0,0 };
	m_enemyPos = { 0,0,0 };

	m_animationTimer = 0;
	m_animationType = STAND;
	m_oldAnimationType = m_animationType;
	m_healTimer = 0;
	m_padState = 0;
	m_padRetentionTimer = 0;
	for (int i = 0; i < 3; i++)
	{
		m_attackCollisionTimer[i] = ImguiControl::P_ATTACK_COLLISION_TIMER[i];
	}
	m_estusHeal = 0;
	m_estusTimer = 0;
	m_trailCount = 0;
	m_keepAnimationType = 0;

	m_cameraMoveEase = 0.0f;
	m_cameraY = 20.0f;
	m_cameraDist = ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE;
	m_blendTimer = 0.0f;
	m_endAngle = 180.0f;

	m_isTarget = false;
	m_isEase = false;
	m_isAttack = false;
	m_isInvincible = false;
	m_isAccept = true;
	m_isChange = false;
	m_isAnimation = false;
	m_isEstus = false;
	m_isHeal = true;
	m_isHelmet = true;
	m_isTrailStart = false;
	m_isStickReleaseTrigger = true;
	m_isSheathed = false;
	m_isOldSheathed = false;

#pragma endregion

#pragma region StatusInit

	m_hp = ImguiControl::P_MAX_HP;
	m_mp = ImguiControl::P_MAX_MP;
	m_stamina = ImguiControl::P_MAX_STAMINA;
	m_power = ImguiControl::P_MAX_POWER[0];
	m_estus = ImguiControl::P_MAX_ESTUS;

#pragma endregion

#pragma region ModelInit

	for (int i = 0; i < C_MIKU_NUM; i++)
	{
		fbxobj_miku->ResetAnimation(i);
	}
	fbxobj_miku->SetRotation(DirectX::XMFLOAT3(0, 180.0f, 0));

#pragma endregion

#pragma region Particle

	//パーティクル(剣)
	const float pScaleSword = 0.5f;
	const float pPowerSword = 0.1f;
	const float pColorSword = 0.9f;
	const int pLifeSword = 30;
	const int pCreateNumSword = 100;

	Particle::ParticleData pDataSword;
	pDataSword.isRandVec = true;
	pDataSword.isRandColor = false;
	pDataSword.scale = { pScaleSword, pScaleSword, pScaleSword };
	pDataSword.power = { pPowerSword, pPowerSword, pPowerSword };
	pDataSword.color = { pColorSword, pColorSword, pColorSword, 1.0f };
	pDataSword.life = pLifeSword;
	for (int i = 0; i < pCreateNumSword; i++)
	{
		pManagerSword.SetParticle(i, pDataSword);
	}
	pManagerSword.SetCreateNum(pCreateNumSword);

	//パーティクル(回復)
	const float pScaleEstus = 0.8f;
	const float pPowerEstus = 0.1f;
	const float pColorEstus = 0.9f;
	const int pCreateNumEstus = 100;

	Particle::ParticleData pDataEstus;
	pDataEstus.isRandVec = true;
	pDataEstus.isRandColor = false;
	pDataEstus.scale = { pScaleEstus, pScaleEstus, pScaleEstus };
	pDataEstus.power = { pPowerEstus, pPowerEstus, pPowerEstus };

	const int pLifeEstusRange = 31;
	for (int i = 0; i < pCreateNumEstus; i++)
	{
		pDataEstus.color = {
			pColorEstus,
			(rand() % 51 * 0.01f) + 0.4f,
			0.0f,
			0.6f };
		pDataEstus.life = rand() % pLifeEstusRange + 60;
		pManagerEstus.SetParticle(i, pDataEstus);
	}
	pManagerEstus.SetCreateNum(pCreateNumEstus);
	pManagerEstus.SetIsCreateStop(true);

	//Light←直す
	pDataEstus.isRandVec = false;
	pDataEstus.scale = { 2.0f, 2.0f, 2.0f };
	pDataEstus.power = { 0.5f, 0.5f, 0.5f };

	for (int i = 0; i < 50; i++)
	{
		pDataEstus.color = {
			pColorEstus,
			(rand() % 51 * 0.01f) + 0.4f,
			0.0f,
			0.6f };
		pManagerLight0.SetLife(i, 50);
		pManagerLight0.SetParticle(i, pDataEstus);
		pManagerLight0.SetVec(i, DirectX::XMFLOAT3(0, 1, 0));
	}
	pManagerLight0.SetCreateNum(1);

	for (int i = 0; i < 50; i++)
	{
		pDataEstus.color = {
			pColorEstus,
			(rand() % 51 * 0.01f) + 0.4f,
			0.0f,
			0.6f };
		pManagerLight1.SetLife(i, 50);
		pManagerLight1.SetParticle(i, pDataEstus);
		pManagerLight1.SetVec(i, DirectX::XMFLOAT3(0, 1, 0));
	}
	pManagerLight1.SetCreateNum(1);

	for (int i = 0; i < 50; i++)
	{
		pDataEstus.color = {
			pColorEstus,
			(rand() % 51 * 0.01f) + 0.4f,
			0.0f,
			0.6f };
		pManagerLight2.SetLife(i, 50);
		pManagerLight2.SetParticle(i, pDataEstus);
		pManagerLight2.SetVec(i, DirectX::XMFLOAT3(0, 1, 0));
	}
	pManagerLight2.SetCreateNum(1);

	for (int i = 0; i < 50; i++)
	{
		pDataEstus.color = {
			pColorEstus,
			(rand() % 51 * 0.01f) + 0.4f,
			0.0f,
			0.6f };
		pManagerLight3.SetLife(i, 50);
		pManagerLight3.SetParticle(i, pDataEstus);
		pManagerLight3.SetVec(i, DirectX::XMFLOAT3(0, 1, 0));
	}
	pManagerLight3.SetCreateNum(1);

#pragma endregion

#pragma region OBB

	//Box
	std::array<float, 10> l_x = {
		3,
		1.7f,
		3,
		2,
		2,
		1.6f,
		1.6f,
		2,
		1.6f,
		2
	};
	std::array<float, 10> l_y = {
		7,
		3.7f,
		3,
		3.5f,
		3.5f,
		3,
		5,
		2,
		5,
		2
	};
	std::array<float, 10> l_z = {
		3,
		1.7f,
		3,
		2,
		2,
		1.6f,
		1.6f,
		3.5f,
		1.6f,
		3.5f
	};
	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(
			l_x[i],
			l_y[i],
			l_z[i]
		));
		ImguiControl::Imgui_playerOBBScale[i][0] = l_x[i];
		ImguiControl::Imgui_playerOBBScale[i][1] = l_y[i];
		ImguiControl::Imgui_playerOBBScale[i][2] = l_z[i];
	}

#pragma endregion

	//Sword
	obj_Sword->SetRotation(XMFLOAT3(
		310.0f,
		300.0f,
		310.0f));

	//Test
	obj_SwordBox->SetPosition(DirectX::XMFLOAT3(
		0.0f,
		6.0f,
		0.0f
	));

	Camera::SetEye(DirectX::XMFLOAT3(0, m_cameraY, ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE));
	ImguiControl::Imgui_cameraDist = ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE;

	//debug
	ImguiControl::Imgui_eye_x = Camera::GetEye().x;
	ImguiControl::Imgui_eye_y = Camera::GetEye().y;
	ImguiControl::Imgui_eye_z = Camera::GetEye().z;
	ImguiControl::Imgui_target_x = Camera::GetEye().x;
	ImguiControl::Imgui_target_x = Camera::GetEye().y;
	ImguiControl::Imgui_target_x = Camera::GetEye().z;
}

void Player::Update(DirectX::XMFLOAT3 enemyPos, bool isInputRecept)
{
	m_enemyPos = enemyPos;

	if (!IsDead() && isInputRecept)
	{
#pragma region Game

		//変数
		XMFLOAT3 cameraPos = Camera::GetEye();
		XMFLOAT3 targetPos = Camera::GetTarget();

		XMFLOAT3 enemyToPlayer =
			OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_miku->GetPosition());
		enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);

		m_cameraToPlayer =
			OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_miku->GetPosition());
		m_cameraToPlayer = OgaJHelper::CalcNormalizeVec3(m_cameraToPlayer);

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
				if (IsInputStick(ImguiControl::P_DOWN_POWER))
				{
					if (!m_isAnimation)
					{
						m_oldAnimationType = m_animationType;
						m_animationType = RUN;
						m_isStickReleaseTrigger = false;

						XMFLOAT3 vec = { 0,0,0 };
						vec.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
						vec.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

						m_pos.x += vec.z * m_cameraToPlayer.x * ImguiControl::P_MAX_MOVE_SPEED;
						m_pos.z += vec.z * m_cameraToPlayer.z * ImguiControl::P_MAX_MOVE_SPEED;

						float rad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
						rad += DirectX::XM_PI / 2;
						m_pos.x += vec.x * sinf(rad) * ImguiControl::P_MAX_MOVE_SPEED;
						m_pos.z += vec.x * cosf(rad) * ImguiControl::P_MAX_MOVE_SPEED;

						cameraPos.x += vec.z * m_cameraToPlayer.x * ImguiControl::P_MAX_MOVE_SPEED;
						cameraPos.z += vec.z * m_cameraToPlayer.z * ImguiControl::P_MAX_MOVE_SPEED;
						cameraPos.x += vec.x * sinf(rad) * ImguiControl::P_MAX_MOVE_SPEED;
						cameraPos.z += vec.x * cosf(rad) * ImguiControl::P_MAX_MOVE_SPEED;

						//移動に完全追従←変更必要(本家参照)
						targetPos.x += vec.z * m_cameraToPlayer.x * ImguiControl::P_MAX_MOVE_SPEED;
						targetPos.z += vec.z * m_cameraToPlayer.z * ImguiControl::P_MAX_MOVE_SPEED;
						targetPos.x += vec.x * sinf(rad) * ImguiControl::P_MAX_MOVE_SPEED;
						targetPos.z += vec.x * cosf(rad) * ImguiControl::P_MAX_MOVE_SPEED;

						float deg = atan2(vec.x, vec.z);
						OgaJHelper::ConvertToDegree(deg);
						float cameraRad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
						OgaJHelper::ConvertToDegree(cameraRad);

						Camera::SetEye(cameraPos);
						Camera::SetTarget(targetPos);

						m_endAngle = deg + cameraRad;
						if (m_endAngle < 0)
						{
							m_endAngle += 360.0f;
						}
					}
				}

				//強制移行
				//どれもアニメーション中じゃなかったら
				else
				{
					if (!m_isAnimation && !m_isStickReleaseTrigger)
					{
						m_isStickReleaseTrigger = true;
						m_oldAnimationType = m_animationType;
						m_animationType = STAND;
					}
				}

#pragma region Camera
				//仮カメラターゲット処理
				//自分にセット
				DirectX::XMFLOAT3 pos = fbxobj_miku->GetPosition();
				pos.y = Camera::GetEye().y;
				DirectX::XMFLOAT3 cameraToPlayer =
					OgaJHelper::CalcDirectionVec3(Camera::GetEye(), pos);
				cameraToPlayer = OgaJHelper::CalcNormalizeVec3(cameraToPlayer);
				const float dist = 100.0f;
				cameraToPlayer.x *= dist;
				cameraToPlayer.y *= dist;
				cameraToPlayer.z *= dist;
				m_cameraTarget = fbxobj_miku->GetPosition() + cameraToPlayer;
				if (!m_isEase)
				{
					Camera::SetTarget(m_cameraTarget);
				}

				//計算
				if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_RIGHTSIDE)) ||
					0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_RIGHTVERT)))
				{
					//ここをどうにかしたい,targetは最悪補完かける
					XMFLOAT3 playerToCamera = OgaJHelper::CalcDirectionVec3(
						fbxobj_miku->GetPosition(),
						Camera::GetEye());
					playerToCamera = OgaJHelper::CalcNormalizeVec3(playerToCamera);

					float xz = atan2(playerToCamera.x, playerToCamera.z);
					OgaJHelper::ConvertToDegree(xz);

					xz += Input::isPadThumb(XINPUT_THUMB_RIGHTSIDE) * ImguiControl::P_MAX_CAMERA_MOVE_SPEED;
					cameraPos.y += Input::isPadThumb(XINPUT_THUMB_RIGHTVERT) * ImguiControl::P_MAX_CAMERA_MOVE_SPEED;
					m_cameraY += Input::isPadThumb(XINPUT_THUMB_RIGHTVERT) * ImguiControl::P_MAX_CAMERA_MOVE_SPEED;

					//距離と差分
					float diff = 0;
					const float MAX_DIST = -50.0f;
					const float MAX_HEIGHT = 100.0f;
					const float MAX_FLOOR = 1.0f;
					if (m_cameraY < MAX_FLOOR)
					{
						if (m_cameraY < MAX_DIST) { m_cameraY = MAX_DIST; }
						diff = m_cameraY;

						//地面との仮判定
						cameraPos.y = MAX_FLOOR;
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
						m_pos.x + s * (m_cameraDist + diff),
						cameraPos.y,
						m_pos.z + c * (m_cameraDist + diff)
					));
				}
#pragma endregion

				//ターゲット移動処理
				if (m_isEase)
				{
					XMFLOAT3 target;
					if (m_cameraMoveEase < 1.0f)
					{
						target = OgaJEase::easeOutCubicXMFLOAT3(
							Camera::GetTarget(),
							m_cameraTarget,
							m_cameraMoveEase);
						m_cameraMoveEase += ImguiControl::P_EASE_CAMERA_TIMER * 2;
					}
					else
					{
						m_cameraMoveEase = 1.0f;
						target = OgaJEase::easeOutCubicXMFLOAT3(
							Camera::GetTarget(),
							m_cameraTarget,
							m_cameraMoveEase);
						m_isEase = false;
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
						fbxobj_miku[AnimationType::STAND]->GetPosition().x + targetToCamera.x * (MAX_DISTANCE + diff),
						fbxobj_miku[AnimationType::STAND]->GetPosition().y + targetToCamera.y * (MAX_DISTANCE + diff),
						fbxobj_miku[AnimationType::STAND]->GetPosition().z + targetToCamera.z * (MAX_DISTANCE + diff)
					));
					//Camera::SetEye(cameraPos);
					Camera::SetTarget(XMFLOAT3(
						fbxobj_miku[AnimationType::STAND]->GetPosition().x + cameraToTarget.x * (MAX_DISTANCE + diff),
						fbxobj_miku[AnimationType::STAND]->GetPosition().y + cameraToTarget.y * (MAX_DISTANCE + diff),
						fbxobj_miku[AnimationType::STAND]->GetPosition().z + cameraToTarget.z * (MAX_DISTANCE + diff)
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
					//fbxobj_miku[AnimationType::STAND]->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
					//fbxobj_miku[AnimationType::SLOWRUN]->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
					//fbxobj_miku[AnimationType::RUN]->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));

					//fbxobj_miku[AnimationType::STAND]->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
					//fbxobj_miku[AnimationType::SLOWRUN]->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
					//fbxobj_miku[AnimationType::RUN]->SetRotation(XMFLOAT3(0, deg + cameraRad - 90.0f, 0));
				}*/

				//else { animationType = STAND; }
			}
		}

		else
		{
			//pad
			bool l_isThumb = false;
			if (Input::isPadConnect())
			{
				if (Input::isPadTrigger(XINPUT_GAMEPAD_RIGHT_THUMB))
				{
					m_cameraMoveEase = 0.0f;
					m_isTarget = false;
					m_isEase = true;
					l_isThumb = true;
					m_cameraY = Camera::GetEye().y;
				}

				if (IsInputStick(ImguiControl::P_DOWN_POWER))
				{
					if (!m_isAnimation)
					{
						m_isStickReleaseTrigger = false;

						XMFLOAT3 vec = { 0,0,0 };
						vec.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
						vec.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

						XMFLOAT3 normVec = OgaJHelper::CalcNormalizeVec3(vec);
						float abs_x = fabsf(normVec.x);
						if (abs_x < 0.5f)
						{
							if (normVec.z > 0)
							{
								if (m_animationType != RUN)
								{
									m_oldAnimationType = m_animationType;
									m_animationType = RUN;
								}
							}
							else
							{
								if (m_animationType != BACK_RUN)
								{
									m_oldAnimationType = m_animationType;
									m_animationType = BACK_RUN;
								}
							}
						}
						else
						{
							if (normVec.x > 0)
							{
								if (m_animationType != R_RUN)
								{
									m_oldAnimationType = m_animationType;
									m_animationType = R_RUN;
								}
							}
							else
							{
								if (m_animationType != L_RUN)
								{
									m_oldAnimationType = m_animationType;
									m_animationType = L_RUN;
								}
							}
						}

						m_pos.x += vec.z * m_cameraToPlayer.x * ImguiControl::P_MAX_MOVE_SPEED;
						m_pos.z += vec.z * m_cameraToPlayer.z * ImguiControl::P_MAX_MOVE_SPEED;

						float rad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
						rad += DirectX::XM_PI / 2;
						m_pos.x += vec.x * sinf(rad) * ImguiControl::P_MAX_MOVE_SPEED;
						m_pos.z += vec.x * cosf(rad) * ImguiControl::P_MAX_MOVE_SPEED;

						cameraPos.x += vec.z * m_cameraToPlayer.x * ImguiControl::P_MAX_MOVE_SPEED;
						cameraPos.z += vec.z * m_cameraToPlayer.z * ImguiControl::P_MAX_MOVE_SPEED;
						cameraPos.x += vec.x * sinf(rad) * ImguiControl::P_MAX_MOVE_SPEED;
						cameraPos.z += vec.x * cosf(rad) * ImguiControl::P_MAX_MOVE_SPEED;

						float deg = atan2(vec.x, vec.z);
						OgaJHelper::ConvertToDegree(deg);
						float cameraRad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
						OgaJHelper::ConvertToDegree(cameraRad);

						Camera::SetEye(cameraPos);
					}
				}

				else
				{
					if (!m_isAnimation && !m_isStickReleaseTrigger)
					{
						m_isStickReleaseTrigger = true;
						m_oldAnimationType = m_animationType;
						m_animationType = STAND;
					}
				}
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

			//ターゲットのゴール地点
			XMFLOAT3 GoalCameraTarget = {
					enemyPos.x,
					enemyPos.y - 30.0f,
					enemyPos.z
			};
			const DirectX::XMFLOAT3 GoalCameraEye = {
					m_pos.x + m_cameraDist * enemyToPlayer.x,
					50.0f,
					m_pos.z + m_cameraDist * enemyToPlayer.z
			};

			float l_dist = OgaJHelper::CalcDist(enemyPos, m_pos);
			if (l_dist < 150.0f)
			{
				if (l_dist < 75.0f)
				{
					GoalCameraTarget.y += 75.0f;
				}
				else
				{
					GoalCameraTarget.y += 150.0f - l_dist;
				}
			}

			if (!l_isThumb && m_cameraMoveEase < 1.0f)
			{
				//カメラ挙動管理用
				m_cameraMoveEase += ImguiControl::P_EASE_CAMERA_TIMER;
				if (m_cameraMoveEase > 1.0f)
				{
					m_cameraMoveEase = 1.0f;
				}

				XMFLOAT3 target = OgaJEase::easeOutCubicXMFLOAT3(
					Camera::GetTarget(),
					GoalCameraTarget,
					m_cameraMoveEase);

				DirectX::XMFLOAT3 eye = OgaJEase::easeOutCubicXMFLOAT3(
					Camera::GetEye(),
					GoalCameraEye,
					m_cameraMoveEase);

				//セット
				Camera::SetTarget(target);
				Camera::SetEye(eye);
			}
			else
			{
				Camera::SetTarget(GoalCameraTarget);
				Camera::SetEye(GoalCameraEye);
			}

			if (m_animationType != ROLLING)
			{
				//向き
				float pRadian = atan2(
					cosf(fbxobj_miku->GetRotation().z),
					sinf(fbxobj_miku->GetRotation().x));
				OgaJHelper::ConvertToDegree(pRadian);
				float cRadian = atan2(m_cameraToPlayer.z, m_cameraToPlayer.x);
				OgaJHelper::ConvertToDegree(cRadian);
				float rot = OgaJHelper::RotateEarliestArc(pRadian, cRadian) * -1;
				float diff = 0;
				if (fbxobj_miku->GetRotation().y - rot > 0) { diff = 0.2f; }
				else if (fbxobj_miku->GetRotation().y - rot < 0) { diff = -0.2f; }

				DirectX::XMFLOAT3 l_rot = {
					fbxobj_miku->GetRotation().x,
					rot + diff,
					fbxobj_miku->GetRotation().z
				};
				fbxobj_miku->SetRotation(l_rot);
			}
		}

#pragma endregion
	}

	//ゲージ管理
	if (m_isHeal)
	{
		if (m_healTimer > 0)
		{
			m_healTimer--;
		}
		else
		{
			if (m_stamina < ImguiControl::P_MAX_STAMINA)
			{
				m_stamina += ImguiControl::P_HEAL_VOL;
			}
		}
	}

	//エスト管理
	if (m_estusHeal > 0)
	{
		//誤差が発生する可能性アリ
		m_estusHeal -= ImguiControl::P_MAX_ESTUS_HEAL_SPEED;
		if (m_hp < ImguiControl::P_MAX_HP)
		{
			m_hp += ImguiControl::P_MAX_ESTUS_HEAL_SPEED;
			if (m_hp >= ImguiControl::P_MAX_HP)
			{
				m_hp = ImguiControl::P_MAX_HP;
				m_estusHeal = 0;
			}
		}
	}

	Setter();

	if (!IsDead())
	{
		Input();
	}
	else
	{
		m_animationType = AnimationType::DIE;
	}

	m_isOldSheathed = m_isSheathed;
	CalcBlendAnimation();

	OtherUpdate();	//ここでm_animationTypeの変更を行うと挙動がおかしくなる可能性アリ
	CalcOBB();

	//パーティクル
	CalcParticle();

	//確認用
	if (m_isAttack)
	{
		obj_Sword->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		obj_Sword->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//Trail
	CalcTrail();

	//エリア外判定
	CalcArea();

	//排斥
	//CalcExpulsion();
}

void Player::Draw()
{
	if (ImguiControl::Imgui_isPlayerDraw)
	{
		fbxobj_miku->Draw(PipelineManager::fbx_receiveShadow);
	}

	if (ImguiControl::isHel)
	{
		obj_Helmet->Draw(PipelineManager::obj_receiveShadow);
	}

	if (ImguiControl::Imgui_isWeaponDraw)
	{
		obj_Sword->Draw(PipelineManager::obj_receiveShadow);
	}

	if (ImguiControl::Imgui_isOBBDraw && m_animationType != DAMAGED)
	{
		for (int i = 0; i < C_BOX_NUM; i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_receiveShadow);
		}
		obj_SwordBox->Draw(PipelineManager::obj_receiveShadow);
	}

	pManagerSword.Draw();
	pManagerEstus.Draw();

	pManagerLight0.Draw();
	pManagerLight1.Draw();
	pManagerLight2.Draw();
	pManagerLight3.Draw();

	if (m_isAttack)
	{
		trail.Draw();
	}
}

void Player::LuminanceDraw()
{
	if (ImguiControl::Imgui_isWeaponDraw)
	{
		obj_Sword->Draw(PipelineManager::obj_normal, false);
	}

	pManagerSword.Draw();
	pManagerEstus.Draw();

	pManagerLight0.Draw();
	pManagerLight1.Draw();
	pManagerLight2.Draw();
	pManagerLight3.Draw();

	if (m_isAttack)
	{
		trail.Draw();
	}
}

void Player::ShadowDraw()
{
	if (ImguiControl::Imgui_isPlayerDraw)
	{
		fbxobj_miku->Draw(PipelineManager::fbx_shadow, false);
	}

	if (ImguiControl::Imgui_isWeaponDraw)
	{
		obj_Sword->Draw(PipelineManager::obj_shadow, false);
	}
}

void Player::ResetCamera()
{
	Camera::SetEye(DirectX::XMFLOAT3(0, m_cameraY, ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE));
}

bool Player::BeforeBattleScene()
{
	return false;
}

void Player::AddMenbers()
{
	rapidjson::Document l_doc(rapidjson::kObjectType);

#pragma region FRAME
	rapidjson::Value l_frameValue(rapidjson::kObjectType);
	l_frameValue.AddMember("ATTACK_COLLISION_TIMER[0]  ", ImguiControl::P_ATTACK_COLLISION_TIMER[0], l_doc.GetAllocator());
	l_frameValue.AddMember("ATTACK_COLLISION_TIMER[1]  ", ImguiControl::P_ATTACK_COLLISION_TIMER[1], l_doc.GetAllocator());
	l_frameValue.AddMember("ATTACK_COLLISION_TIMER[2]  ", ImguiControl::P_ATTACK_COLLISION_TIMER[2], l_doc.GetAllocator());
	l_frameValue.AddMember("ATTACK_COLLISION_ENDTIMER  ", ImguiControl::P_ATTACK_COLLISION_ENDTIMER, l_doc.GetAllocator());
	l_doc.AddMember("FRAME", l_frameValue, l_doc.GetAllocator());
#pragma endregion

#pragma region OTHER
	rapidjson::Value l_frameOther(rapidjson::kObjectType);
	l_frameOther.AddMember("MAX_CAMERA_NEAR_DISTANCE  ", ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE, l_doc.GetAllocator());
	l_frameOther.AddMember("MAX_CAMERA_FAR_DISTANCE   ", ImguiControl::P_MAX_CAMERA_FAR_DISTANCE, l_doc.GetAllocator());
	l_frameOther.AddMember("MAX_PAD_RETENTION         ", ImguiControl::P_MAX_PAD_RETENTION, l_doc.GetAllocator());
	l_frameOther.AddMember("MAX_CAMERA_MOVE_SPEED     ", ImguiControl::P_MAX_CAMERA_MOVE_SPEED, l_doc.GetAllocator());
	l_frameOther.AddMember("EASE_CAMERA_TIMER         ", ImguiControl::P_EASE_CAMERA_TIMER, l_doc.GetAllocator());
	l_frameOther.AddMember("MAX_BLEND_TIMER           ", ImguiControl::P_MAX_BLEND_TIMER, l_doc.GetAllocator());
	l_frameOther.AddMember("DOWN_POWER                ", ImguiControl::P_DOWN_POWER, l_doc.GetAllocator());
	l_frameOther.AddMember("ROTATE_ADD_ANGLE          ", ImguiControl::P_ROTATE_ADD_ANGLE, l_doc.GetAllocator());
	l_frameOther.AddMember("EXPLUSION_RAD             ", ImguiControl::P_EXPLUSION_RAD, l_doc.GetAllocator());
	l_doc.AddMember("OTHER", l_frameOther, l_doc.GetAllocator());
#pragma endregion

#pragma region STATUS
	rapidjson::Value l_frameStatus(rapidjson::kObjectType);
	l_frameStatus.AddMember("MAX_HP                 ", ImguiControl::P_MAX_HP, l_doc.GetAllocator());
	l_frameStatus.AddMember("MAX_MP                 ", ImguiControl::P_MAX_MP, l_doc.GetAllocator());
	l_frameStatus.AddMember("MAX_STAMINA            ", ImguiControl::P_MAX_STAMINA, l_doc.GetAllocator());

	l_frameStatus.AddMember("MAX_POWER[0]           ", ImguiControl::P_MAX_POWER[0], l_doc.GetAllocator());
	l_frameStatus.AddMember("MAX_POWER[1]           ", ImguiControl::P_MAX_POWER[1], l_doc.GetAllocator());
	l_frameStatus.AddMember("MAX_POWER[2]           ", ImguiControl::P_MAX_POWER[2], l_doc.GetAllocator());

	l_frameStatus.AddMember("MAX_ESTUS              ", ImguiControl::P_MAX_ESTUS, l_doc.GetAllocator());
	l_frameStatus.AddMember("ESTUS_TIMER            ", ImguiControl::P_ESTUS_TIMER, l_doc.GetAllocator());
	l_frameStatus.AddMember("MAX_ESTUS_HEAL         ", ImguiControl::P_MAX_ESTUS_HEAL, l_doc.GetAllocator());
	l_frameStatus.AddMember("MAX_ESTUS_HEAL_SPEED   ", ImguiControl::P_MAX_ESTUS_HEAL_SPEED, l_doc.GetAllocator());
	l_frameStatus.AddMember("AUTOHEAL_STAMINA_TIMER ", ImguiControl::P_AUTOHEAL_STAMINA_TIMER, l_doc.GetAllocator());
	l_frameStatus.AddMember("ATTACK_SUB_STAMINA     ", ImguiControl::P_ATTACK_SUB_STAMINA, l_doc.GetAllocator());
	l_frameStatus.AddMember("ROLLING_SUB_STAMINA    ", ImguiControl::P_ROLLING_SUB_STAMINA, l_doc.GetAllocator());
	l_frameStatus.AddMember("HEAL_VOL               ", ImguiControl::P_HEAL_VOL, l_doc.GetAllocator());
	l_frameStatus.AddMember("C_MAX_MOVE_SPEED       ", ImguiControl::P_MAX_MOVE_SPEED, l_doc.GetAllocator());
	l_doc.AddMember("STATUS", l_frameStatus, l_doc.GetAllocator());
#pragma endregion

	Config::Writter("PlayerDataConfig", l_doc);
}

void Player::ExportJson()
{
#pragma region FRAME
	m_pDoc["FRAME"]["ATTACK_COLLISION_TIMER[0]  "].SetInt(ImguiControl::P_ATTACK_COLLISION_TIMER[0]);
	m_pDoc["FRAME"]["ATTACK_COLLISION_TIMER[1]  "].SetInt(ImguiControl::P_ATTACK_COLLISION_TIMER[1]);
	m_pDoc["FRAME"]["ATTACK_COLLISION_TIMER[2]  "].SetInt(ImguiControl::P_ATTACK_COLLISION_TIMER[2]);
	m_pDoc["FRAME"]["ATTACK_COLLISION_ENDTIMER  "].SetInt(ImguiControl::P_ATTACK_COLLISION_ENDTIMER);
#pragma endregion

#pragma region OTHER
	m_pDoc["OTHER"]["MAX_CAMERA_NEAR_DISTANCE  "].SetFloat(ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE);
	m_pDoc["OTHER"]["MAX_CAMERA_FAR_DISTANCE   "].SetFloat(ImguiControl::P_MAX_CAMERA_FAR_DISTANCE);
	m_pDoc["OTHER"]["MAX_PAD_RETENTION         "].SetInt(ImguiControl::P_MAX_PAD_RETENTION);
	m_pDoc["OTHER"]["MAX_CAMERA_MOVE_SPEED     "].SetFloat(ImguiControl::P_MAX_CAMERA_MOVE_SPEED);
	m_pDoc["OTHER"]["EASE_CAMERA_TIMER         "].SetFloat(ImguiControl::P_EASE_CAMERA_TIMER);
	m_pDoc["OTHER"]["MAX_BLEND_TIMER           "].SetFloat(ImguiControl::P_MAX_BLEND_TIMER);
	m_pDoc["OTHER"]["DOWN_POWER                "].SetFloat(ImguiControl::P_DOWN_POWER);
	m_pDoc["OTHER"]["ROTATE_ADD_ANGLE          "].SetFloat(ImguiControl::P_ROTATE_ADD_ANGLE);
	m_pDoc["OTHER"]["EXPLUSION_RAD             "].SetFloat(ImguiControl::P_EXPLUSION_RAD);
#pragma endregion

#pragma region STATUS
	m_pDoc["STATUS"]["MAX_HP                 "].SetInt(ImguiControl::P_MAX_HP);
	m_pDoc["STATUS"]["MAX_MP                 "].SetInt(ImguiControl::P_MAX_MP);
	m_pDoc["STATUS"]["MAX_STAMINA            "].SetInt(ImguiControl::P_MAX_STAMINA);

	m_pDoc["STATUS"]["MAX_POWER[0]           "].SetInt(ImguiControl::P_MAX_POWER[0]);
	m_pDoc["STATUS"]["MAX_POWER[1]           "].SetInt(ImguiControl::P_MAX_POWER[1]);
	m_pDoc["STATUS"]["MAX_POWER[2]           "].SetInt(ImguiControl::P_MAX_POWER[2]);

	m_pDoc["STATUS"]["MAX_ESTUS              "].SetInt(ImguiControl::P_MAX_ESTUS);
	m_pDoc["STATUS"]["ESTUS_TIMER            "].SetInt(ImguiControl::P_ESTUS_TIMER);
	m_pDoc["STATUS"]["MAX_ESTUS_HEAL         "].SetInt(ImguiControl::P_MAX_ESTUS_HEAL);
	m_pDoc["STATUS"]["MAX_ESTUS_HEAL_SPEED   "].SetInt(ImguiControl::P_MAX_ESTUS_HEAL_SPEED);
	m_pDoc["STATUS"]["AUTOHEAL_STAMINA_TIMER "].SetInt(ImguiControl::P_AUTOHEAL_STAMINA_TIMER);
	m_pDoc["STATUS"]["ATTACK_SUB_STAMINA     "].SetInt(ImguiControl::P_ATTACK_SUB_STAMINA);
	m_pDoc["STATUS"]["ROLLING_SUB_STAMINA    "].SetInt(ImguiControl::P_ROLLING_SUB_STAMINA);
	m_pDoc["STATUS"]["HEAL_VOL               "].SetInt(ImguiControl::P_HEAL_VOL);
	m_pDoc["STATUS"]["C_MAX_MOVE_SPEED       "].SetFloat(ImguiControl::P_MAX_MOVE_SPEED);
#pragma endregion

	Config::Writter("PlayerDataConfig", m_pDoc);
}

void Player::InportJson()
{
#pragma region FRAME
	ImguiControl::P_ATTACK_COLLISION_TIMER[0] = m_pDoc["FRAME"]["ATTACK_COLLISION_TIMER[0]  "].GetInt();
	ImguiControl::P_ATTACK_COLLISION_TIMER[1] = m_pDoc["FRAME"]["ATTACK_COLLISION_TIMER[1]  "].GetInt();
	ImguiControl::P_ATTACK_COLLISION_TIMER[2] = m_pDoc["FRAME"]["ATTACK_COLLISION_TIMER[2]  "].GetInt();
	ImguiControl::P_ATTACK_COLLISION_ENDTIMER = m_pDoc["FRAME"]["ATTACK_COLLISION_ENDTIMER  "].GetInt();
#pragma endregion

#pragma region OTHER
	ImguiControl::P_MAX_CAMERA_NEAR_DISTANCE = m_pDoc["OTHER"]["MAX_CAMERA_NEAR_DISTANCE  "].GetFloat();
	ImguiControl::P_MAX_CAMERA_FAR_DISTANCE = m_pDoc["OTHER"]["MAX_CAMERA_FAR_DISTANCE   "].GetFloat();
	ImguiControl::P_MAX_PAD_RETENTION = m_pDoc["OTHER"]["MAX_PAD_RETENTION         "].GetInt();
	ImguiControl::P_MAX_CAMERA_MOVE_SPEED = m_pDoc["OTHER"]["MAX_CAMERA_MOVE_SPEED     "].GetFloat();
	ImguiControl::P_EASE_CAMERA_TIMER = m_pDoc["OTHER"]["EASE_CAMERA_TIMER         "].GetFloat();
	ImguiControl::P_MAX_BLEND_TIMER = m_pDoc["OTHER"]["MAX_BLEND_TIMER           "].GetFloat();
	ImguiControl::P_DOWN_POWER = m_pDoc["OTHER"]["DOWN_POWER                "].GetFloat();
	ImguiControl::P_ROTATE_ADD_ANGLE = m_pDoc["OTHER"]["ROTATE_ADD_ANGLE          "].GetFloat();
	ImguiControl::P_EXPLUSION_RAD = m_pDoc["OTHER"]["EXPLUSION_RAD             "].GetFloat();
#pragma endregion

#pragma region STATUS
	ImguiControl::P_MAX_HP = m_pDoc["STATUS"]["MAX_HP                 "].GetInt();
	ImguiControl::P_MAX_MP = m_pDoc["STATUS"]["MAX_MP                 "].GetInt();
	ImguiControl::P_MAX_STAMINA = m_pDoc["STATUS"]["MAX_STAMINA            "].GetInt();

	ImguiControl::P_MAX_POWER[0] = m_pDoc["STATUS"]["MAX_POWER[0]           "].GetInt();
	ImguiControl::P_MAX_POWER[1] = m_pDoc["STATUS"]["MAX_POWER[1]           "].GetInt();
	ImguiControl::P_MAX_POWER[2] = m_pDoc["STATUS"]["MAX_POWER[2]           "].GetInt();

	ImguiControl::P_MAX_ESTUS = m_pDoc["STATUS"]["MAX_ESTUS              "].GetInt();
	ImguiControl::P_ESTUS_TIMER = m_pDoc["STATUS"]["ESTUS_TIMER            "].GetInt();
	ImguiControl::P_MAX_ESTUS_HEAL = m_pDoc["STATUS"]["MAX_ESTUS_HEAL         "].GetInt();
	ImguiControl::P_MAX_ESTUS_HEAL_SPEED = m_pDoc["STATUS"]["MAX_ESTUS_HEAL_SPEED   "].GetInt();
	ImguiControl::P_AUTOHEAL_STAMINA_TIMER = m_pDoc["STATUS"]["AUTOHEAL_STAMINA_TIMER "].GetInt();
	ImguiControl::P_ATTACK_SUB_STAMINA = m_pDoc["STATUS"]["ATTACK_SUB_STAMINA     "].GetInt();
	ImguiControl::P_ROLLING_SUB_STAMINA = m_pDoc["STATUS"]["ROLLING_SUB_STAMINA    "].GetInt();
	ImguiControl::P_HEAL_VOL = m_pDoc["STATUS"]["HEAL_VOL               "].GetInt();
	ImguiControl::P_MAX_MOVE_SPEED = m_pDoc["STATUS"]["C_MAX_MOVE_SPEED       "].GetFloat();
#pragma endregion
}

void Player::Input()
{
	//先行入力削除
	if (m_padRetentionTimer < ImguiControl::P_MAX_PAD_RETENTION)
	{
		m_padRetentionTimer++;
	}
	else
	{
		m_padState = 0;
	}

	//先行入力
	if (m_stamina > 0)
	{
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
		else if (Input::isPadTrigger(XINPUT_GAMEPAD_X))
		{
			m_padRetentionTimer = 0;
			m_padState = XINPUT_GAMEPAD_X;
		}
	}

	//アニメーションが一定フレームを超えたら攻撃判定開始←連撃非対応←した
	if (!m_isAttack)
	{
		CalcAttackTimer();
	}

	//攻撃←連撃非対応←した
	if (m_stamina > 0)
	{
		if (m_padState == XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			if (m_animationType != DAMAGED &&
				m_animationType != ROLLING)
			{
				if (m_isAccept)
				{
					CheckAttackAnimationType();
				}
			}
		}
	}

	//回避
	if (m_stamina > 0)
	{
		if (m_padState == XINPUT_GAMEPAD_B)
		{
			if (m_isAccept &&
				m_animationType != DAMAGED &&
				m_animationType != ROLLING)
			{
				CalcRollingAngle();
			}
		}
	}

	//回復
	if (m_estus > 0)
	{
		if (m_padState == XINPUT_GAMEPAD_X)
		{
			if (m_isAccept &&
				m_animationType != DAMAGED &&
				m_animationType != ROLLING &&
				m_animationType != HEAL)
			{
				m_animationType = HEAL;
				m_estusTimer = 0;
				m_isAnimation = true;
				m_isStickReleaseTrigger = false;
				m_isEstus = false;
			}
		}
	}
}

void Player::Setter()
{
	fbxobj_miku->SetPosition(m_pos);
}

void Player::CalcOBB()
{
	obj_SwordBox->MultiMatrix(obj_Sword->GetMatWorld());
	obj_SwordBox->SetScale(DirectX::XMFLOAT3(
		ModelManager::model_sword->GetModelSize().x / 2.0f,
		ModelManager::model_sword->GetModelSize().y / 2.0f,
		ModelManager::model_sword->GetModelSize().z / 2.0f
	));

	if (m_animationType == NORMAL_ATTACK_3)
	{
		const float addMultiScale = 2.0f;
		obj_SwordBox->SetScale(DirectX::XMFLOAT3(
			obj_SwordBox->GetScale().x * addMultiScale,
			obj_SwordBox->GetScale().y * addMultiScale,
			obj_SwordBox->GetScale().z * addMultiScale
		));
	}

	obj_SwordBox->Update();

	OBB swordOBB;
	//攻撃時の当たり判定(OBB)←計算してるだけ
	if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_3)
	{
		DirectX::XMFLOAT3 pos = {
			obj_SwordBox->GetMatWorld().r[3].m128_f32[0],
			obj_SwordBox->GetMatWorld().r[3].m128_f32[1],
			obj_SwordBox->GetMatWorld().r[3].m128_f32[2] };

		swordOBB.pos = pos;
		swordOBB.matrix = fbxobj_miku->GetMatrix();
		swordOBB.length = obj_SwordBox->GetScale();
		m_obb = swordOBB;
	}

	std::vector<OBB> l_obbs;
	for (int i = 0; i < matRot.size(); i++)
	{
		obj_Box[i]->MultiMatrix(bones[i].second);
		obj_Box[i]->Update();

		OBB l_obb;
		l_obb.pos = DirectX::XMFLOAT3(
			bones[i].second.r[3].m128_f32[0],
			bones[i].second.r[3].m128_f32[1],
			bones[i].second.r[3].m128_f32[2]);
		l_obb.matrix = matRot[i];
		l_obb.length = obj_Box[i]->GetScale();
		l_obbs.push_back(l_obb);
	}
	m_obbs = l_obbs;
}

void Player::OtherUpdate()
{
	CalcAngle();

	//Update
	fbxobj_miku->SetAnimationIndex(m_animationType);
	fbxobj_miku->Update();

	//Current
	switch (m_animationType)
	{
	case STAND:
	{
		ImguiControl::Imgui_playerAniType = "STAND";
		break;
	}
	case SLOWRUN:
	{
		ImguiControl::Imgui_playerAniType = "SLOWRUN";
		break;
	}
	case RUN:
	{
		ImguiControl::Imgui_playerAniType = "RUN";
		break;
	}
	case BACK_RUN:
	{
		ImguiControl::Imgui_playerAniType = "BACK_RUN";
		break;
	}
	case R_RUN:
	{
		ImguiControl::Imgui_playerAniType = "R_RUN";
		break;
	}
	case L_RUN:
	{
		ImguiControl::Imgui_playerAniType = "L_RUN";
		break;
	}
	case NORMAL_ATTACK_1:
	{
		CalcAttack();

		ImguiControl::Imgui_playerAniType = "NORMAL_ATTACK_1";
		break;
	}
	case NORMAL_ATTACK_2:
	{
		CalcAttack();

		ImguiControl::Imgui_playerAniType = "NORMAL_ATTACK_2";
		break;
	}
	case NORMAL_ATTACK_3:
	{
		CalcAttack();

		ImguiControl::Imgui_playerAniType = "NORMAL_ATTACK_3";
		break;
	}
	case DAMAGED:
	{
		if (m_isAttack) { m_isAttack = false; }

		if (fbxobj_miku->IsAnimationEnd(m_animationType))
		{
			m_isAnimation = false;
			m_isInvincible = false;
		}

		ImguiControl::Imgui_playerAniType = "DAMAGED";
		break;
	}
	case ROLLING:
	{
		CalcRolling();

		ImguiControl::Imgui_playerAniType = "ROLLING";
		break;
	}
	case DIE:
	{
		if (fbxobj_miku->IsAnimationEnd(AnimationType::DIE))
		{
			fbxobj_miku->StopAnimation(AnimationType::DIE);
		}
		ImguiControl::Imgui_playerAniType = "DIE";
		break;
	}
	case HEAL:
	{
		if (!m_isEstus)
		{
			CalcHeal();
		}

		if (fbxobj_miku->IsAnimationEnd(m_animationType))
		{
			m_isAnimation = false;
		}

		ImguiControl::Imgui_playerAniType = "HEAL";
		break;
	}
	}

	//Imgui
	SetImgui();

	bones = fbxobj_miku->GetAffineTrans();
	matRot = fbxobj_miku->GetMatRots();
	int i = 0;
	int l_eraseCount = 0;
	while (1)
	{
		if (boxes.size() <= i)
		{
			break;
		}

		if (boxes[i] == 0)
		{
			bones.erase(bones.begin() + (i - l_eraseCount));
			matRot.erase(matRot.begin() + (i - l_eraseCount));
			l_eraseCount++;
		}

		i++;
	}

	//仮
	if (m_isHelmet != ImguiControl::isHel)
	{
		if (ImguiControl::isHel)
		{
			fbxobj_miku->SetDrawSkipNum(3);
			fbxobj_miku->SetDrawSkipNum(4);
			fbxobj_miku->SetDrawSkipNum(5);
			fbxobj_miku->SetDrawSkipNum(6);
			fbxobj_miku->SetDrawSkipNum(7);
			fbxobj_miku->SetDrawSkipNum(10);
			fbxobj_miku->SetDrawSkipNum(12);
			fbxobj_miku->SetDrawSkipNum(14);
		}
		else
		{
			fbxobj_miku->ResetDrawSkipNum();
		}
	}
	m_isHelmet = ImguiControl::isHel;

	obj_Helmet->MultiMatrix(bones[2].second);
	if (m_isSheathed)
	{
		obj_Sword->SetPosition(DirectX::XMFLOAT3(
			ImguiControl::SwordXt,
			ImguiControl::SwordYt,
			ImguiControl::SwordZt
		));
		obj_Sword->SetRotation(DirectX::XMFLOAT3(
			ImguiControl::SwordXr,
			ImguiControl::SwordYr,
			ImguiControl::SwordZr
		));

		obj_Sword->MultiMatrix(bones[0].second);
	}
	else
	{
		obj_Sword->SetPosition(XMFLOAT3(
			0.0f,
			0.0f,
			0.0f));
		obj_Sword->SetRotation(XMFLOAT3(
			310.0f,
			300.0f,
			310.0f));
		obj_Sword->MultiMatrix(fbxobj_miku->GetMatrix());
	}

	obj_Helmet->Update();
	obj_Sword->Update();
}

void Player::CalcBlendAnimation()
{
	//切り替わったら
	if (!m_isChange &&
		m_animationType != m_oldAnimationType)
	{
		m_blendTimer = 0.0f;
		m_isChange = true;
		m_keepAnimationType = m_animationType;
		fbxobj_miku->ResetAnimation(m_animationType);
		fbxobj_miku->SetAnimationSpeed(0, 0, false);
	}

	//補間計算
	else
	{
		//途中切り替え時
		if (m_animationType != m_keepAnimationType)
		{
			m_blendTimer = 0.0f;
			m_keepAnimationType = m_animationType;
			fbxobj_miku->ResetAnimation(m_animationType);
			fbxobj_miku->SetAnimationSpeed(0, 0, false);
		}

		//タイマー計算
		else if (m_blendTimer < 1.0f)
		{
			m_blendTimer += ImguiControl::P_MAX_BLEND_TIMER;
			if (m_blendTimer > 1.0f)
			{
				m_blendTimer = 1.0f;
				m_isChange = false;
				m_oldAnimationType = m_animationType;

				//納刀
				if (m_animationType < AnimationType::NORMAL_ATTACK_1 ||
					m_animationType>AnimationType::NORMAL_ATTACK_3)
				{
					m_isSheathed = true;
				}
			}
		}
		else
		{
			m_isChange = false;
			m_oldAnimationType = m_animationType;

			//納刀
			if (m_animationType < AnimationType::NORMAL_ATTACK_1 ||
				m_animationType>AnimationType::NORMAL_ATTACK_3)
			{
				m_isSheathed = true;
			}
		}
	}

	fbxobj_miku->
		BlendAnimation(
			m_oldAnimationType,
			m_animationType,
			OgaJEase::easeOutCubic(m_blendTimer),
			true);
}

void Player::CalcAttack()
{
	//最大までのフレーム数
	float l_frame =
		fbxobj_miku->GetEndTime(m_animationType) / fbxobj_miku->GetAddTime(m_animationType);

	fbxobj_miku->SetAnimationSpeed(
		m_animationType, OgaJEase::easeInSine(m_attackEase), true);

	if (m_attackEase < 1.0f)
	{
		m_attackEase += 1.0f / l_frame;
	}
	else
	{
		m_attackEase = 1.0f;
	}

	//移動処理
	if (m_isAttack)
	{
		DirectX::XMFLOAT3 l_pos = m_pos;
		l_pos.y = m_enemyPos.y;
		float l_dist = OgaJHelper::CalcDist(m_enemyPos, l_pos);

		const float C_DIST = 40.0f;
		if (l_dist > C_DIST)
		{
			const float l_speed = 0.8f;
			DirectX::XMFLOAT3 l_angle = GetAngle();
			float l_addPosX = l_angle.x * l_speed;
			float l_addPosZ = l_angle.z * l_speed;
			m_pos.x += l_addPosX;
			m_pos.z += l_addPosZ;

			DirectX::XMFLOAT3 l_target = Camera::GetTarget();
			DirectX::XMFLOAT3 l_eye = Camera::GetEye();
			l_target.x += l_addPosX;
			l_target.z += l_addPosZ;
			l_eye.x += l_addPosX;
			l_eye.z += l_addPosZ;
			Camera::SetTarget(l_target);
			Camera::SetEye(l_eye);
		}
	}

	//攻撃アニメーション中断
	if (!m_isAccept)
	{
		if (fbxobj_miku->GetNowTime(m_animationType) >=
			fbxobj_miku->GetAddTime(m_animationType) * ImguiControl::P_ATTACK_COLLISION_ENDTIMER)
		{
			m_isAttack = false;
			m_isAccept = true;
			m_isHeal = true;
		}
	}

	if (fbxobj_miku->IsAnimationEnd(m_animationType))
	{
		m_isAnimation = false;
		m_isAttack = false;
		m_isAccept = true;
		m_isHeal = true;
	}
}

void Player::CalcAttackTimer()
{
	if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_3)
	{
		//攻撃判定開始前
		int l_animationNum = m_animationType - NORMAL_ATTACK_1;
		if (m_animationTimer < m_attackCollisionTimer[l_animationNum])
		{
			m_animationTimer++;
		}
		//開始
		else
		{
			//攻撃力
			m_power = ImguiControl::P_MAX_POWER[m_animationType - 6];

			m_animationTimer = 0;
			m_isAttack = true;

			m_healTimer = ImguiControl::P_AUTOHEAL_STAMINA_TIMER;
			m_stamina -= ImguiControl::P_ATTACK_SUB_STAMINA;
			if (m_stamina < 0)
			{
				m_stamina = 0;
			}

			//攻撃時方向切り替え
			if (IsInputStick(ImguiControl::P_DOWN_POWER))
			{
				SetAngle(GetInputAngle());
			}
		}
	}
}

void Player::CheckAttackAnimationType()
{
	//連撃←無限に受け付けるかも
	if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_2)
	{
		//仮
		m_attackEase = 0.0f;

		int l_animType = m_animationType + 1;
		DoAttack(l_animType);
	}
	//1撃目
	else if (m_animationType != NORMAL_ATTACK_3)
	{
		//仮
		m_attackEase = 0.0f;

		DoAttack(AnimationType::NORMAL_ATTACK_1);
	}
}

void Player::DoAttack(const int animationType)
{
	m_isAnimation = true;
	m_isStickReleaseTrigger = false;
	m_isAccept = false;
	m_isHeal = false;
	m_padState = 0;
	m_animationTimer = 0;

	//仮
	m_isSheathed = false;

	m_oldAnimationType = m_animationType;
	m_animationType = animationType;
	fbxobj_miku->PlayAnimation(animationType);
}

void Player::CalcRolling()
{
	float l_addPosX = m_rollingAngle.x * ImguiControl::P_MAX_MOVE_SPEED;
	float l_addPosZ = m_rollingAngle.z * ImguiControl::P_MAX_MOVE_SPEED;
	m_pos.x += l_addPosX;
	m_pos.z += l_addPosZ;

	XMFLOAT3 l_cameraPos = Camera::GetEye();
	l_cameraPos.x += l_addPosX;
	l_cameraPos.z += l_addPosZ;
	Camera::SetEye(l_cameraPos);
	if (!m_isTarget)
	{
		Camera::SetTarget(m_cameraTarget);
	}

	fbxobj_miku->SetPosition(m_pos);

	if (fbxobj_miku->IsAnimationEnd(m_animationType))
	{
		m_isAnimation = false;
		m_isInvincible = false;
	}
}

void Player::CalcRollingAngle()
{
	m_padState = 0;

	m_healTimer = ImguiControl::P_AUTOHEAL_STAMINA_TIMER;
	m_stamina -= ImguiControl::P_ROLLING_SUB_STAMINA;
	if (m_stamina < 0)
	{
		m_stamina = 0;
	}

	m_oldAnimationType = m_animationType;
	m_animationType = ROLLING;
	fbxobj_miku->ResetAnimation(AnimationType::ROLLING);
	fbxobj_miku->PlayAnimation(AnimationType::ROLLING);
	m_isAnimation = true;
	m_isStickReleaseTrigger = false;
	m_isInvincible = true;

	//向きの計算
	if (IsInputStick(ImguiControl::P_DOWN_POWER))
	{
		m_rollingAngle = GetInputAngle();
		SetAngle(m_rollingAngle);
	}
	else
	{
		m_rollingAngle = GetAngle();
		SetAngle(m_rollingAngle);
	}
}

void Player::CalcHeal()
{
	if (m_estusTimer < ImguiControl::P_ESTUS_TIMER)
	{
		m_estusTimer++;
	}
	else
	{
		m_estus--;
		m_estusHeal = ImguiControl::P_MAX_ESTUS_HEAL;
		m_isEstus = true;
		pManagerEstus.SetIsCreateStop(false);
	}
}

void Player::CalcArea()
{
	//エリア外判定
	float l_dist = OgaJHelper::CalcDist(DirectX::XMFLOAT3(0, 0, 0), m_pos);
	if (l_dist > ImguiControl::Imgui_stageArea)
	{
		float l_sub = l_dist - ImguiControl::Imgui_stageArea;
		DirectX::XMFLOAT3 l_backVec = OgaJHelper::CalcDirectionVec3(
			m_pos, DirectX::XMFLOAT3(0, 0, 0));
		l_backVec = OgaJHelper::CalcNormalizeVec3(l_backVec);
		float l_backX = l_backVec.x * l_sub;
		float l_backZ = l_backVec.z * l_sub;
		m_pos.x += l_backX;
		m_pos.z += l_backZ;
		DirectX::XMFLOAT3 l_eye = Camera::GetEye();
		DirectX::XMFLOAT3 l_target = Camera::GetTarget();
		l_eye.x += l_backX;
		l_eye.z += l_backZ;
		l_target.x += l_backX;
		l_target.z += l_backZ;
		Camera::SetEye(l_eye);
		Camera::SetTarget(l_target);
	}
}

void Player::CalcExpulsion(bool isTackle)
{
	if (isTackle) { return; }
	if (m_animationType == ROLLING)
	{
		return;
	}

	DirectX::XMFLOAT3 l_pos = m_pos;
	l_pos.y = m_enemyPos.y;
	float l_dist = OgaJHelper::CalcDist(m_enemyPos, l_pos);

	if (l_dist < ImguiControl::P_EXPLUSION_RAD)
	{
		DirectX::XMFLOAT3 l_vec =
			OgaJHelper::CalcNormalizeVec3(OgaJHelper::CalcDirectionVec3(m_enemyPos, l_pos));
		float l_expulsX = l_vec.x * (ImguiControl::P_EXPLUSION_RAD - l_dist);
		float l_expulsZ = l_vec.z * (ImguiControl::P_EXPLUSION_RAD - l_dist);
		m_pos.x += l_expulsX;
		m_pos.z += l_expulsZ;

		DirectX::XMFLOAT3 l_eye = Camera::GetEye();
		DirectX::XMFLOAT3 l_target = Camera::GetTarget();
		l_eye.x += l_expulsX;
		l_eye.z += l_expulsZ;
		l_target.x += l_expulsX;
		l_target.z += l_expulsZ;
		Camera::SetEye(l_eye);
		Camera::SetTarget(l_target);
	}
}

void Player::SetImgui()
{
	//Old
	switch (m_oldAnimationType)
	{
	case STAND:
	{
		ImguiControl::Imgui_playerOldAniType = "STAND";
		break;
	}
	case SLOWRUN:
	{
		ImguiControl::Imgui_playerOldAniType = "SLOWRUN";
		break;
	}
	case RUN:
	{
		ImguiControl::Imgui_playerOldAniType = "RUN";
		break;
	}
	case BACK_RUN:
	{
		ImguiControl::Imgui_playerOldAniType = "BACK_RUN";
		break;
	}
	case R_RUN:
	{
		ImguiControl::Imgui_playerOldAniType = "R_RUN";
		break;
	}
	case L_RUN:
	{
		ImguiControl::Imgui_playerOldAniType = "L_RUN";
		break;
	}
	case NORMAL_ATTACK_1:
	{
		ImguiControl::Imgui_playerOldAniType = "NORMAL_ATTACK_1";
		break;
	}
	case NORMAL_ATTACK_2:
	{
		ImguiControl::Imgui_playerOldAniType = "NORMAL_ATTACK_2";
		break;
	}
	case NORMAL_ATTACK_3:
	{
		ImguiControl::Imgui_playerOldAniType = "NORMAL_ATTACK_3";
		break;
	}
	case DAMAGED:
	{
		ImguiControl::Imgui_playerOldAniType = "DAMAGED";
		break;
	}
	case ROLLING:
	{
		ImguiControl::Imgui_playerOldAniType = "ROLLING";
		break;
	}
	case HEAL:
	{
		ImguiControl::Imgui_playerOldAniType = "HEAL";
		break;
	}
	}

	if (m_isAccept)
	{
		ImguiControl::Imgui_playerIsAccept = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_playerIsAccept = "FALSE";
	}

	if (m_isChange)
	{
		ImguiControl::Imgui_playerIsChange = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_playerIsChange = "FALSE";
	}

	if (m_isAttack)
	{
		ImguiControl::Imgui_playerIsAttack = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_playerIsAttack = "FALSE";
	}

	m_cameraDist = ImguiControl::Imgui_cameraDist;

	ImguiControl::Imgui_playerCurrentAniTimer = fbxobj_miku->GetNowTime(m_animationType);
	ImguiControl::Imgui_playerOldAniTimer = fbxobj_miku->GetNowTime(m_oldAnimationType);
	ImguiControl::Imgui_playerBlendTimer = m_blendTimer;

	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetPosition(DirectX::XMFLOAT3(
			ImguiControl::Imgui_playerOBBPos[i][0],
			ImguiControl::Imgui_playerOBBPos[i][1],
			ImguiControl::Imgui_playerOBBPos[i][2]
		));
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(
			ImguiControl::Imgui_playerOBBScale[i][0],
			ImguiControl::Imgui_playerOBBScale[i][1],
			ImguiControl::Imgui_playerOBBScale[i][2]
		));
	}

	if (m_isInvincible)
	{
		ImguiControl::Imgui_playerIsInvincible = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_playerIsInvincible = "FALSE";
	}

	/*Camera::SetEye(DirectX::XMFLOAT3(
		ImguiControl::Imgui_eye_x, ImguiControl::Imgui_eye_y, ImguiControl::Imgui_eye_z));
	Camera::SetTarget(DirectX::XMFLOAT3(
		ImguiControl::Imgui_target_x, ImguiControl::Imgui_target_y, ImguiControl::Imgui_target_z));*/
}

bool Player::IsInputStick(float downPower)
{
	if (downPower < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
		downPower < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
	{
		return true;
	}
	return false;
}

void Player::CalcParticle()
{
	CalcSwordParticle();
	CalcEstusParticle();

	const int range = 21;

	for (int i = 0; i < pManagerLight0.GetMaxParticle(); i++)
	{
		if (!pManagerLight0.IsMove(i))
		{
			pManagerLight0.SetPosition(i, DirectX::XMFLOAT3(
				0 + rand() % range - range * 0.5f,
				100 + rand() % range - range * 0.5f,
				-500 + rand() % range - range * 0.5f
			));
			break;
		}
	}
	pManagerLight0.Update();

	for (int i = 0; i < pManagerLight1.GetMaxParticle(); i++)
	{
		if (!pManagerLight1.IsMove(i))
		{
			pManagerLight1.SetPosition(i, DirectX::XMFLOAT3(
				0 + rand() % range - range * 0.5f,
				100 + rand() % range - range * 0.5f,
				500 + rand() % range - range * 0.5f
			));
			break;
		}
	}
	pManagerLight1.Update();

	for (int i = 0; i < pManagerLight2.GetMaxParticle(); i++)
	{
		if (!pManagerLight2.IsMove(i))
		{
			pManagerLight2.SetPosition(i, DirectX::XMFLOAT3(
				-500 + rand() % range - range * 0.5f,
				100 + rand() % range - range * 0.5f,
				0 + rand() % range - range * 0.5f
			));
			break;
		}
	}
	pManagerLight2.Update();

	for (int i = 0; i < pManagerLight3.GetMaxParticle(); i++)
	{
		if (!pManagerLight3.IsMove(i))
		{
			pManagerLight3.SetPosition(i, DirectX::XMFLOAT3(
				500 + rand() % range - range * 0.5f,
				100 + rand() % range - range * 0.5f,
				0 + rand() % range - range * 0.5f
			));
			break;
		}
	}
	pManagerLight3.Update();
}

void Player::CalcSwordParticle()
{
	//パーティクル(剣)
	pManagerSword.SetIsCreateStop(true);
	if (m_isSheathed != m_isOldSheathed)
	{
		pManagerSword.SetIsCreateStop(false);
	}
	for (int i = 0; i < pManagerSword.GetMaxParticle(); i++)
	{
		//剣から微調整
		pManagerSword.SetPosition(i, DirectX::XMFLOAT3(
			0,
			static_cast<float>(rand() % 21) + 5.0f,
			0));

		//生成時のみ剣に付随
		if (!pManagerSword.IsMove(i))
		{
			pManagerSword.MultiMatrix(i, obj_Sword->GetMatrix());
		}
	}

	pManagerSword.Update();
}

void Player::CalcEstusParticle()
{
	const int l_randRange = 3;
	for (int i = 0; i < pManagerEstus.GetMaxParticle(); i++)
	{
		DirectX::XMFLOAT3 l_vec = pManagerEstus.GetVec(i);
		if (l_vec.x < 0.0f)
		{
			l_vec.x += 0.01f;
		}
		else if (l_vec.x > 0.0f)
		{
			l_vec.x -= 0.01f;
		}

		l_vec.y += 0.02f;

		if (l_vec.z < 0.0f)
		{
			l_vec.z += 0.01f;
		}
		else if (l_vec.z > 0.0f)
		{
			l_vec.z -= 0.01f;
		}
		pManagerEstus.SetVec(i, l_vec);

		//剣から微調整
		pManagerEstus.SetPosition(i, DirectX::XMFLOAT3(
			m_obbs[0].pos.x + static_cast<float>(rand() % l_randRange) - l_randRange * 0.5f,
			(m_obbs[0].pos.y + 3.0f) + static_cast<float>(rand() % l_randRange) - l_randRange * 0.5f,
			m_obbs[0].pos.z + static_cast<float>(rand() % l_randRange) - l_randRange * 0.5f
		));
	}
	pManagerEstus.Update();
	pManagerEstus.SetIsCreateStop(true);
}

void Player::CalcTrail()
{
	const float trailScale_z = 25.0f;
	const float trailBottomPos_y = 25.0f;

	if (m_trailCount > 99)
	{
		m_trailCount = 0;
	}

	DirectX::XMMATRIX l_bottom = DirectX::XMMatrixIdentity();
	l_bottom *= DirectX::XMMatrixTranslation(0, trailBottomPos_y, 0);
	l_bottom *= obj_Sword->GetMatrix();

	DirectX::XMMATRIX l_trans = DirectX::XMMatrixIdentity();
	l_trans.r[3].m128_f32[1] = trailScale_z;
	l_trans *= l_bottom;

	DirectX::XMFLOAT3 top = {
		l_trans.r[3].m128_f32[0],
	l_trans.r[3].m128_f32[1] ,
	l_trans.r[3].m128_f32[2] };

	DirectX::XMFLOAT3 bottom = {
		l_bottom.r[3].m128_f32[0],
	l_bottom.r[3].m128_f32[1],
	l_bottom.r[3].m128_f32[2] };

	if (!m_isTrailStart)
	{
		trail.SetCurrentPos(m_trailCount, top, bottom);
		m_isTrailStart = true;
	}
	else
	{
		int l_oldCount = m_trailCount - 1;
		if (l_oldCount < 0)
		{
			l_oldCount = 99;
		}

		trail.SetOldPos(m_trailCount,
			trail.GetCurrentPos_Top(l_oldCount),
			trail.GetCurrentPos_Bottom(l_oldCount));
		trail.SetCurrentPos(m_trailCount, top, bottom);
		if (!m_isSheathed)
		{
			trail.CreateTrail(m_trailCount);
		}
	}

	if (!m_isSheathed)
	{
		m_trailCount++;
	}
	else
	{
		m_isTrailStart = false;
	}
	trail.Update();
}

DirectX::XMFLOAT3 Player::GetInputAngle()
{
	XMFLOAT3 l_deg = { 0,0,0 };
	l_deg.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
	l_deg.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);
	l_deg = OgaJHelper::CalcNormalizeVec3(l_deg);

	DirectX::XMVECTOR l_matRotVec = DirectX::XMLoadFloat3(&l_deg);
	l_matRotVec = DirectX::XMVector3Transform(l_matRotVec, Camera::BillboardYMatrix());

	DirectX::XMFLOAT3 retAngle = { 0,0,0 };
	retAngle = { l_matRotVec.m128_f32[0],0,l_matRotVec.m128_f32[2] };
	return retAngle;
}

DirectX::XMFLOAT3 Player::GetAngle()
{
	XMFLOAT3 l_deg = fbxobj_miku->GetRotation();
	OgaJHelper::ConvertToRadian(l_deg.y);
	float l_radX = sinf(l_deg.y);
	float l_radZ = cosf(l_deg.y);
	return XMFLOAT3(l_radX, 0, l_radZ);
}

void Player::SetAngle(DirectX::XMFLOAT3 angle)
{
	float l_degY = atan2(angle.x, angle.z) * 180.0f / DirectX::XM_PI;
	if (l_degY < 0)
	{
		l_degY += 360.0f;
	}
	m_endAngle = l_degY;
}

void Player::CalcAngle()
{
	//どこかで補間する(ここはTriggerでしか通らない)←移動にも適応させる。
	float l_currentDegY = fbxobj_miku->GetRotation().y;
	if (l_currentDegY < 0)
	{
		l_currentDegY += 360.0f;
	}
	if (l_currentDegY > 360.0f)
	{
		l_currentDegY -= 360.0f;
	}

	float l_nearAngle = OgaJHelper::RotateEarliestArc(
		l_currentDegY, m_endAngle);
	if (l_nearAngle < 0)
	{
		float l_totalAngle = l_currentDegY - ImguiControl::P_ROTATE_ADD_ANGLE;
		if (fabsf(l_totalAngle - m_endAngle) < ImguiControl::P_ROTATE_ADD_ANGLE)
		{
			l_totalAngle = m_endAngle;
		}
		fbxobj_miku->SetRotation(DirectX::XMFLOAT3(0, l_totalAngle, 0));
	}
	else if (l_nearAngle > 0)
	{
		float l_totalAngle = l_currentDegY + ImguiControl::P_ROTATE_ADD_ANGLE;
		if (fabsf(l_totalAngle - m_endAngle) < ImguiControl::P_ROTATE_ADD_ANGLE)
		{
			l_totalAngle = m_endAngle;
		}
		fbxobj_miku->SetRotation(DirectX::XMFLOAT3(0, l_totalAngle, 0));
	}
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
	m_estusHeal = 0;
	m_isAnimation = true;
	m_isInvincible = true;
	m_isStickReleaseTrigger = false;
	m_isAccept = true;
	m_isHeal = true;
	m_oldAnimationType = m_animationType;
	m_animationType = DAMAGED;
	//fbxobj_miku->ResetAnimation(m_animationType);
	//fbxobj_miku->SetAnimationSpeed(0, 0, false);
	if (m_hp < 0) { m_hp = 0; }
	OutputDebugStringA("Hit!\n");
}
