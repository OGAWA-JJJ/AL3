#include "Player.h"
#include "../../imgui/ImguiControl.h"
#include "../Math/OgaJHelper.h"
#include "../Input/Input.h"
#include "../DirectX/Camera.h"
#include "PipelineManager.h"
#include "ModelManager.h"

Player::Player()
{
#pragma region Init

	m_obb = {};
	m_pos = { 0,0,0 };
	m_cameraAngle = { 0,0,0 };
	m_rollingAngle = { 0,0,0 };
	m_cameraToPlayer = { 0,0,0 };
	m_cameraTarget = { 0,0,0 };
	m_animationTimer = 0;
	m_animationType = STAND;
	m_oldAnimationType = m_animationType;
	m_healTimer = 0;
	m_padState = 0;
	m_padRetentionTimer = 0;

	for (int i = 0; i < 3; i++)
	{
		m_attackCollisionTimer[i] = C_ATTACK_COLLISION_TIMER[i];
	}

	m_cameraMoveEase = 0.0f;
	m_cameraY = 0.0f;
	m_cameraDist = C_MAX_CAMERA_NEAR_DISTANCE;
	m_blendTimer = 0.0f;
	m_isTarget = false;
	m_isEase = false;
	m_isAttack = false;
	m_isAccept = true;
	m_isChange = false;
	m_isAnimation = false;

#pragma endregion

#pragma region StatusInit

	m_hp = C_MAX_HP;
	m_mp = C_MAX_MP;
	m_stamina = C_MAX_STAMINA;
	m_power = C_MAX_POWER;

#pragma endregion

#pragma region ModelCreate

	//Obj
	obj_Sword = Object::Create(ModelManager::model_sword);
	obj_ShadowSword = Object::Create(ModelManager::model_sword);

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
	fbxobj_miku[STAND] = FbxObjects::Create(ModelManager::fbxmodel_standMiku);
	fbxobj_miku[STAND]->PlayAnimation();

	fbxobj_miku[SLOWRUN] = FbxObjects::Create(ModelManager::fbxmodel_slowRunMiku);
	fbxobj_miku[SLOWRUN]->PlayAnimation();

	fbxobj_miku[RUN] = FbxObjects::Create(ModelManager::fbxmodel_fastRunMiku);
	fbxobj_miku[RUN]->PlayAnimation();

	fbxobj_miku[NORMAL_ATTACK_1] = FbxObjects::Create(ModelManager::fbxmodel_oneSwrordAttack);
	fbxobj_miku[NORMAL_ATTACK_1]->PlayAnimation();
	fbxobj_miku[NORMAL_ATTACK_1]->StopAnimation();

	fbxobj_miku[NORMAL_ATTACK_2] = FbxObjects::Create(ModelManager::fbxmodel_oneSwrordAttack2);
	fbxobj_miku[NORMAL_ATTACK_2]->PlayAnimation();
	fbxobj_miku[NORMAL_ATTACK_2]->StopAnimation();
	fbxobj_miku[NORMAL_ATTACK_2]->SetLoopAnimation(false);

	fbxobj_miku[NORMAL_ATTACK_3] = FbxObjects::Create(ModelManager::fbxmodel_oneSwrordAttack3);
	fbxobj_miku[NORMAL_ATTACK_3]->PlayAnimation();
	fbxobj_miku[NORMAL_ATTACK_3]->StopAnimation();
	fbxobj_miku[NORMAL_ATTACK_3]->SetLoopAnimation(false);

	fbxobj_miku[DIE] = FbxObjects::Create(ModelManager::fbxmodel_dieMiku);
	fbxobj_miku[DIE]->PlayAnimation();

	fbxobj_miku[DAMAGED] = FbxObjects::Create(ModelManager::fbxmodel_impactMiku);
	fbxobj_miku[DAMAGED]->PlayAnimation();

	fbxobj_miku[ROLLING] = FbxObjects::Create(ModelManager::fbxmodel_rollingMiku);
	fbxobj_miku[ROLLING]->PlayAnimation();
	fbxobj_miku[ROLLING]->SetLoopAnimation(false);

	//Fbx(Shadow)
	fbxobj_shadowMiku[STAND] = FbxObjects::Create(ModelManager::fbxmodel_standMiku);
	fbxobj_shadowMiku[STAND]->PlayAnimation();

	fbxobj_shadowMiku[SLOWRUN] = FbxObjects::Create(ModelManager::fbxmodel_slowRunMiku);
	fbxobj_shadowMiku[SLOWRUN]->PlayAnimation();

	fbxobj_shadowMiku[RUN] = FbxObjects::Create(ModelManager::fbxmodel_fastRunMiku);
	fbxobj_shadowMiku[RUN]->PlayAnimation();

	fbxobj_shadowMiku[NORMAL_ATTACK_1] = FbxObjects::Create(ModelManager::fbxmodel_oneSwrordAttack);
	fbxobj_shadowMiku[NORMAL_ATTACK_1]->PlayAnimation();
	fbxobj_shadowMiku[NORMAL_ATTACK_1]->StopAnimation();

	fbxobj_shadowMiku[NORMAL_ATTACK_2] = FbxObjects::Create(ModelManager::fbxmodel_oneSwrordAttack2);
	fbxobj_shadowMiku[NORMAL_ATTACK_2]->PlayAnimation();
	fbxobj_shadowMiku[NORMAL_ATTACK_2]->StopAnimation();
	fbxobj_shadowMiku[NORMAL_ATTACK_2]->SetLoopAnimation(false);

	fbxobj_shadowMiku[NORMAL_ATTACK_3] = FbxObjects::Create(ModelManager::fbxmodel_oneSwrordAttack3);
	fbxobj_shadowMiku[NORMAL_ATTACK_3]->PlayAnimation();
	fbxobj_shadowMiku[NORMAL_ATTACK_3]->StopAnimation();
	fbxobj_shadowMiku[NORMAL_ATTACK_3]->SetLoopAnimation(false);

	fbxobj_shadowMiku[DIE] = FbxObjects::Create(ModelManager::fbxmodel_dieMiku);
	fbxobj_shadowMiku[DIE]->PlayAnimation();

	fbxobj_shadowMiku[DAMAGED] = FbxObjects::Create(ModelManager::fbxmodel_impactMiku);
	fbxobj_shadowMiku[DAMAGED]->PlayAnimation();

	fbxobj_shadowMiku[ROLLING] = FbxObjects::Create(ModelManager::fbxmodel_rollingMiku);
	fbxobj_shadowMiku[ROLLING]->PlayAnimation();
	fbxobj_shadowMiku[ROLLING]->SetLoopAnimation(false);

#pragma endregion

#pragma region ModelInit

	const float Sword_Scale = 2.0f;
	const float Miku_Scale = 0.02f;

	//Obj
	obj_Sword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));
	obj_ShadowSword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));

	//Fbx
	for (int i = 0; i < C_MIKU_NUM; i++)
	{
		fbxobj_miku[i]->SetPosition(XMFLOAT3(0, 0, 0));
		fbxobj_miku[i]->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
		fbxobj_miku[i]->SetRotation(XMFLOAT3(0, 0, 0));

		fbxobj_shadowMiku[i]->SetPosition(XMFLOAT3(0, 0, 0));
		fbxobj_shadowMiku[i]->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
		fbxobj_shadowMiku[i]->SetRotation(XMFLOAT3(0, 0, 0));

		fbxobj_miku[i]->SetDrawSkipNum(3);
		fbxobj_miku[i]->SetDrawSkipNum(4);
		fbxobj_miku[i]->SetDrawSkipNum(5);
		fbxobj_miku[i]->SetDrawSkipNum(6);
		fbxobj_miku[i]->SetDrawSkipNum(7);
		fbxobj_miku[i]->SetDrawSkipNum(10);
		fbxobj_miku[i]->SetDrawSkipNum(12);
		fbxobj_miku[i]->SetDrawSkipNum(14);

		fbxobj_shadowMiku[i]->SetDrawSkipNum(3);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(4);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(5);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(6);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(7);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(10);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(12);
		fbxobj_shadowMiku[i]->SetDrawSkipNum(14);
	}

#pragma endregion
}

Player::~Player()
{
	delete obj_Sword;
	delete obj_ShadowSword;
	delete obj_Helmet;

	for (int i = 0; i < 28; i++)
	{
		delete obj_Box[i];
	}

	delete obj_SwordBox;

	for (int i = 0; i < C_MIKU_NUM; i++)
	{
		delete fbxobj_miku[i];
		delete fbxobj_shadowMiku[i];
	}
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
	for (int i = 0; i < C_MIKU_NUM; i++)
	{
		fbxobj_miku[i]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}

	m_cameraY = 50.0f;

	//Test
	obj_SwordBox->SetPosition(DirectX::XMFLOAT3(
		0.0f,
		6.0f,
		0.0f
	));

	ImguiControl::Imgui_cameraDist = C_MAX_CAMERA_NEAR_DISTANCE;

	//パーティクル
	const float pScale = 0.3f;
	const float pPower = 0.03f;
	const float pColor = 0.9f;

	pManager.Init();
	Particle::ParticleData pData;
	pData.isRandVec = true;
	pData.isRandColor = false;
	pData.scale = { pScale, pScale, pScale };
	pData.power = { pPower, pPower, pPower };
	pData.color = { pColor, pColor, pColor, 1.0f };
	for (int i = 0; i < pManager.GetMaxParticle(); i++)
	{
		pManager.SetParticle(i, pData);
	}

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
	}
}

void Player::Update(DirectX::XMFLOAT3 enemyPos)
{
	if (IsDead())
	{
		if (fbxobj_miku[AnimationType::DIE]->IsAnimationEnd())
		{
			fbxobj_miku[AnimationType::DIE]->StopAnimation();
			fbxobj_shadowMiku[AnimationType::DIE]->StopAnimation();
		}

		fbxobj_miku[AnimationType::DIE]->SetPosition(m_pos);
		fbxobj_shadowMiku[AnimationType::DIE]->SetPosition(m_pos);
		fbxobj_miku[AnimationType::DIE]->Update();
		fbxobj_shadowMiku[AnimationType::DIE]->Update();
		return;
	}

#pragma region Game

	//変数
	XMFLOAT3 cameraPos = Camera::GetEye();
	XMFLOAT3 targetPos = Camera::GetTarget();
	XMFLOAT3 enemyToPlayer = {};
	m_cameraToPlayer = {};

	if (m_animationType == STAND)
	{
		enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_miku[AnimationType::STAND]->GetPosition());
		m_cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_miku[AnimationType::STAND]->GetPosition());
	}
	else if (m_animationType == SLOWRUN)
	{
		enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_miku[AnimationType::SLOWRUN]->GetPosition());
		m_cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_miku[AnimationType::SLOWRUN]->GetPosition());
	}
	else if (m_animationType == RUN)
	{
		enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_miku[AnimationType::RUN]->GetPosition());
		m_cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_miku[AnimationType::RUN]->GetPosition());
	}
	else if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_3)
	{
		enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_miku[m_animationType]->GetPosition());
		m_cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_miku[m_animationType]->GetPosition());
	}
	//2と3追加
	else if (m_animationType == ROLLING)
	{
		enemyToPlayer = OgaJHelper::CalcDirectionVec3(enemyPos, fbxobj_miku[AnimationType::ROLLING]->GetPosition());
		m_cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), fbxobj_miku[AnimationType::ROLLING]->GetPosition());
	}

	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);
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
			if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
				0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
			{
				if (!m_isAnimation)
				{
					m_oldAnimationType = m_animationType;
					m_animationType = RUN;
					m_isStickReleaseTrigger = false;

					XMFLOAT3 vec = { 0,0,0 };
					vec.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
					vec.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

					m_pos.x += vec.z * m_cameraToPlayer.x * C_MAX_MOVE_SPEED;
					m_pos.z += vec.z * m_cameraToPlayer.z * C_MAX_MOVE_SPEED;

					float rad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
					rad += DirectX::XM_PI / 2;
					m_pos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
					m_pos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

					cameraPos.x += vec.z * m_cameraToPlayer.x * C_MAX_MOVE_SPEED;
					cameraPos.z += vec.z * m_cameraToPlayer.z * C_MAX_MOVE_SPEED;
					cameraPos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
					cameraPos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

					//移動に完全追従←変更必要(本家参照)
					targetPos.x += vec.z * m_cameraToPlayer.x * C_MAX_MOVE_SPEED;
					targetPos.z += vec.z * m_cameraToPlayer.z * C_MAX_MOVE_SPEED;
					targetPos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
					targetPos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

					float deg = atan2(vec.x, vec.z);
					OgaJHelper::ConvertToDegree(deg);
					float cameraRad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
					OgaJHelper::ConvertToDegree(cameraRad);

					Camera::SetEye(cameraPos);
					Camera::SetTarget(targetPos);

					for (int i = 0; i < C_MIKU_NUM; i++)
					{
						fbxobj_miku[i]->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
						fbxobj_shadowMiku[i]->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
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
			DirectX::XMFLOAT3 pos = fbxobj_miku[STAND]->GetPosition();
			pos.y = Camera::GetEye().y;
			DirectX::XMFLOAT3 cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), pos);
			cameraToPlayer = OgaJHelper::CalcNormalizeVec3(cameraToPlayer);
			const float dist = 100.0f;
			cameraToPlayer.x *= dist;
			cameraToPlayer.y *= dist;
			cameraToPlayer.z *= dist;
			m_cameraTarget = fbxobj_miku[STAND]->GetPosition() + cameraToPlayer;
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
					fbxobj_miku[AnimationType::STAND]->GetPosition(),
					Camera::GetEye());
				playerToCamera = OgaJHelper::CalcNormalizeVec3(playerToCamera);

				float xz = atan2(playerToCamera.x, playerToCamera.z);
				OgaJHelper::ConvertToDegree(xz);

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
					m_cameraMoveEase += C_EASE_CAMERA_TIMER * 2;
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

			if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
				0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
			{
				if (!m_isAnimation)
				{
					m_oldAnimationType = m_animationType;
					m_animationType = RUN;
					m_isStickReleaseTrigger = false;

					XMFLOAT3 vec = { 0,0,0 };
					vec.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
					vec.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);

					m_pos.x += vec.z * m_cameraToPlayer.x * C_MAX_MOVE_SPEED;
					m_pos.z += vec.z * m_cameraToPlayer.z * C_MAX_MOVE_SPEED;

					float rad = atan2(m_cameraToPlayer.x, m_cameraToPlayer.z);
					rad += DirectX::XM_PI / 2;
					m_pos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
					m_pos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

					cameraPos.x += vec.z * m_cameraToPlayer.x * C_MAX_MOVE_SPEED;
					cameraPos.z += vec.z * m_cameraToPlayer.z * C_MAX_MOVE_SPEED;
					cameraPos.x += vec.x * sinf(rad) * C_MAX_MOVE_SPEED;
					cameraPos.z += vec.x * cosf(rad) * C_MAX_MOVE_SPEED;

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
		const XMFLOAT3 GoalCameraTarget = {
				enemyPos.x,
				enemyPos.y - 30.0f,
				enemyPos.z
		};
		const DirectX::XMFLOAT3 GoalCameraEye = {
				m_pos.x + m_cameraDist * enemyToPlayer.x,
				50.0f,
				m_pos.z + m_cameraDist * enemyToPlayer.z
		};
		if (!l_isThumb && m_cameraMoveEase < 1.0f)
		{
			//カメラ挙動管理用
			m_cameraMoveEase += C_EASE_CAMERA_TIMER;
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

		//向き
		float pRadian = atan2(
			cosf(fbxobj_miku[AnimationType::STAND]->GetRotation().z),
			sinf(fbxobj_miku[AnimationType::STAND]->GetRotation().x));
		OgaJHelper::ConvertToDegree(pRadian);
		float cRadian = atan2(m_cameraToPlayer.z, m_cameraToPlayer.x);
		OgaJHelper::ConvertToDegree(cRadian);
		float rot = OgaJHelper::RotateEarliestArc(pRadian, cRadian) * -1;
		float diff = 0;
		if (fbxobj_miku[AnimationType::STAND]->GetRotation().y - rot > 0) { diff = 0.2f; }
		else if (fbxobj_miku[AnimationType::STAND]->GetRotation().y - rot < 0) { diff = -0.2f; }
		fbxobj_miku[AnimationType::STAND]->SetRotation(XMFLOAT3(
			fbxobj_miku[AnimationType::STAND]->GetRotation().x,
			rot + diff,
			fbxobj_miku[AnimationType::STAND]->GetRotation().z
		));

		DirectX::XMFLOAT3 l_rot = fbxobj_miku[AnimationType::STAND]->GetRotation();
		fbxobj_miku[AnimationType::SLOWRUN]->SetRotation(l_rot);
		fbxobj_miku[AnimationType::RUN]->SetRotation(l_rot);
		fbxobj_miku[AnimationType::NORMAL_ATTACK_1]->SetRotation(l_rot);
		fbxobj_miku[AnimationType::NORMAL_ATTACK_2]->SetRotation(l_rot);
		fbxobj_miku[AnimationType::NORMAL_ATTACK_3]->SetRotation(l_rot);
		fbxobj_miku[AnimationType::DIE]->SetRotation(l_rot);
		fbxobj_miku[AnimationType::DAMAGED]->SetRotation(l_rot);

		fbxobj_shadowMiku[AnimationType::STAND]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::SLOWRUN]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::RUN]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::NORMAL_ATTACK_1]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::NORMAL_ATTACK_2]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::NORMAL_ATTACK_3]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::DIE]->SetRotation(l_rot);
		fbxobj_shadowMiku[AnimationType::DAMAGED]->SetRotation(l_rot);
	}

#pragma endregion

	//ゲージ管理
	if (m_isHeal)
	{
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
	}

	Setter();
	Input();

	CalcBlendAnimation();

	OtherUpdate();	//ここでm_animationTypeの変更を行うと挙動がおかしくなる可能性アリ
	CalcOBB();

	//確認用
	if (m_isAttack)
	{
		obj_Sword->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		obj_Sword->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_cameraDist = ImguiControl::Imgui_cameraDist;

	//パーティクル
	for (int i = 0; i < pManager.GetMaxParticle(); i++)
	{
		//剣から微調整
		pManager.SetPosition(i, DirectX::XMFLOAT3(
			0,
			static_cast<float>(rand() % 21) + 5.0f,
			0));

		//生成時のみ剣に付随
		if (!pManager.IsMove(i))
		{
			pManager.MultiMatrix(i, obj_Sword->GetMatrix());
		}
	}

	pManager.Update();
}

void Player::Draw()
{
	if (IsDead())
	{
		fbxobj_miku[AnimationType::DIE]->Draw(PipelineManager::fbx_normal);
		return;
	}

	if (ImguiControl::Imgui_isPlayerDraw)
	{
		fbxobj_miku[m_animationType]->Draw(PipelineManager::fbx_normal);
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());

	if (ImguiControl::isHel)
	{
		obj_Helmet->Draw(PipelineManager::obj_normal);
	}

	if (ImguiControl::Imgui_isWeaponDraw)
	{
		obj_Sword->Draw(PipelineManager::obj_normal);
	}

	if (ImguiControl::Imgui_isOBBDraw && m_animationType != DAMAGED)
	{
		for (int i = 0; i < C_BOX_NUM; i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_normal);
		}
		obj_SwordBox->Draw(PipelineManager::obj_normal);
	}

	Object::PostDraw();

	pManager.Draw();
}

void Player::LuminanceDraw()
{
	if (IsDead())
	{
		return;
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_Sword->Draw(PipelineManager::obj_normal);
	Object::PostDraw();

	pManager.Draw();
}

void Player::ShadowDraw()
{
	if (IsDead())
	{
		fbxobj_shadowMiku[AnimationType::DIE]->Draw(PipelineManager::fbx_shadow);
		return;
	}

	if (ImguiControl::Imgui_isPlayerDraw)
	{
		fbxobj_shadowMiku[m_animationType]->Draw(PipelineManager::fbx_shadow);
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	if (ImguiControl::Imgui_isWeaponDraw)
	{
		obj_ShadowSword->Draw(PipelineManager::obj_shadow);
	}
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
	if (m_stamina > 0 && !m_isTarget)
	{
		if (m_padState == XINPUT_GAMEPAD_B)
		{
			if (m_isAccept &&
				m_animationType != DAMAGED &&
				m_animationType != ROLLING)
			{
				m_padState = 0;

				m_healTimer = C_HEAL_TIMER;
				m_stamina -= C_ROLLING_SUB_STAMINA;
				if (m_stamina < 0)
				{
					m_stamina = 0;
				}

				m_oldAnimationType = m_animationType;
				m_animationType = ROLLING;
				fbxobj_miku[AnimationType::ROLLING]->ResetAnimation();
				fbxobj_miku[AnimationType::ROLLING]->ReplayAnimation();
				fbxobj_shadowMiku[AnimationType::ROLLING]->ResetAnimation();
				fbxobj_shadowMiku[AnimationType::ROLLING]->ReplayAnimation();
				m_isAnimation = true;
				m_isStickReleaseTrigger = false;
				m_isInvincible = true;

				//向きの計算
				if (0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTSIDE)) ||
					0.3 < fabs(Input::isPadThumb(XINPUT_THUMB_LEFTVERT)))
				{
					XMFLOAT3 l_deg = { 0,0,0 };
					l_deg.x = Input::isPadThumb(XINPUT_THUMB_LEFTSIDE);
					l_deg.z = Input::isPadThumb(XINPUT_THUMB_LEFTVERT);
					l_deg = OgaJHelper::CalcNormalizeVec3(l_deg);

					DirectX::XMVECTOR l_matRotVec = DirectX::XMLoadFloat3(&l_deg);
					l_matRotVec = DirectX::XMVector3Transform(l_matRotVec, Camera::BillboardYMatrix());

					m_rollingAngle = { l_matRotVec.m128_f32[0],0,l_matRotVec.m128_f32[2] };
					float l_degY = atan2(m_rollingAngle.x, m_rollingAngle.z) * 180.0f / DirectX::XM_PI;
					fbxobj_miku[AnimationType::ROLLING]->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
					fbxobj_shadowMiku[AnimationType::ROLLING]->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
				}
				else
				{
					XMFLOAT3 l_deg = fbxobj_miku[AnimationType::STAND]->GetRotation();
					OgaJHelper::ConvertToRadian(l_deg.y);
					float l_radX = sinf(l_deg.y);
					float l_radZ = cosf(l_deg.y);

					m_rollingAngle = { l_radX,0,l_radZ };
					float l_degY = atan2(m_rollingAngle.x, m_rollingAngle.z) * 180.0f / DirectX::XM_PI;
					fbxobj_miku[AnimationType::ROLLING]->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
					fbxobj_shadowMiku[AnimationType::ROLLING]->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
				}
			}
		}
	}
}

void Player::Setter()
{
	fbxobj_miku[AnimationType::STAND]->SetPosition(m_pos);
	fbxobj_miku[AnimationType::SLOWRUN]->SetPosition(m_pos);
	fbxobj_miku[AnimationType::RUN]->SetPosition(m_pos);
	fbxobj_miku[AnimationType::NORMAL_ATTACK_1]->SetPosition(m_pos);
	fbxobj_miku[AnimationType::NORMAL_ATTACK_2]->SetPosition(m_pos);
	fbxobj_miku[AnimationType::NORMAL_ATTACK_3]->SetPosition(m_pos);
	fbxobj_miku[AnimationType::DAMAGED]->SetPosition(m_pos);

	fbxobj_shadowMiku[AnimationType::STAND]->SetPosition(m_pos);
	fbxobj_shadowMiku[AnimationType::SLOWRUN]->SetPosition(m_pos);
	fbxobj_shadowMiku[AnimationType::RUN]->SetPosition(m_pos);
	fbxobj_shadowMiku[AnimationType::NORMAL_ATTACK_1]->SetPosition(m_pos);
	fbxobj_shadowMiku[AnimationType::NORMAL_ATTACK_2]->SetPosition(m_pos);
	fbxobj_shadowMiku[AnimationType::NORMAL_ATTACK_3]->SetPosition(m_pos);
	fbxobj_shadowMiku[AnimationType::DAMAGED]->SetPosition(m_pos);
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
		swordOBB.matrix = fbxobj_miku[m_animationType]->GetMatrix();
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
	//Current
	if (m_animationType == STAND)
	{
		fbxobj_miku[AnimationType::STAND]->Update();
		fbxobj_shadowMiku[AnimationType::STAND]->Update(true);

		ImguiControl::Imgui_playerAniType = "STAND";
	}
	else if (m_animationType == SLOWRUN)
	{
		fbxobj_miku[AnimationType::SLOWRUN]->Update();
		fbxobj_shadowMiku[AnimationType::SLOWRUN]->Update(true);

		ImguiControl::Imgui_playerAniType = "SLOWRUN";
	}
	else if (m_animationType == RUN)
	{
		fbxobj_miku[AnimationType::RUN]->Update();
		fbxobj_shadowMiku[AnimationType::RUN]->Update(true);

		ImguiControl::Imgui_playerAniType = "RUN";
	}
	else if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_3)
	{
		//最大までのフレーム数
		float l_frame = fbxobj_miku[m_animationType]->GetEndTime() / fbxobj_miku[m_animationType]->GetAddTime();
		fbxobj_miku[m_animationType]->SetAnimationSpeed(OgaJEase::easeInSine(m_attackEase), true);
		fbxobj_shadowMiku[m_animationType]->SetAnimationSpeed(OgaJEase::easeInSine(m_attackEase), true);
		if (m_attackEase < 1.0f)
		{
			m_attackEase += 1.0f / l_frame;
		}
		else
		{
			m_attackEase = 1.0f;
		}

		fbxobj_miku[m_animationType]->Update();
		fbxobj_shadowMiku[m_animationType]->Update(true);

		if (m_animationType == NORMAL_ATTACK_1)
		{
			ImguiControl::Imgui_playerAniType = "NORMAL_ATTACK_1";
		}
		else if (m_animationType == NORMAL_ATTACK_2)
		{
			ImguiControl::Imgui_playerAniType = "NORMAL_ATTACK_2";
		}
		else if (m_animationType == NORMAL_ATTACK_3)
		{
			ImguiControl::Imgui_playerAniType = "NORMAL_ATTACK_3";
		}


		//攻撃アニメーション中断ローリング
		if (!m_isAccept)
		{
			if (fbxobj_miku[m_animationType]->GetNowTime() >=
				fbxobj_miku[m_animationType]->GetAddTime() * C_ATTACK_COLLISION_ENDTIMER)
			{
				m_isAttack = false;
				m_isAccept = true;
				m_isHeal = true;
			}
		}

		if (fbxobj_miku[m_animationType]->IsAnimationEnd())
		{
			m_isAnimation = false;
			m_isAttack = false;
		}
	}
	else if (m_animationType == DAMAGED)
	{
		if (m_isAttack) { m_isAttack = false; }
		fbxobj_miku[AnimationType::DAMAGED]->Update();
		fbxobj_shadowMiku[AnimationType::DAMAGED]->Update(true);

		ImguiControl::Imgui_playerAniType = "DAMAGED";

		if (fbxobj_miku[AnimationType::DAMAGED]->IsAnimationEnd())
		{
			m_isAnimation = false;
			m_isInvincible = false;
		}
	}
	else if (m_animationType == ROLLING)
	{
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
			Camera::SetTarget(m_cameraTarget);
		}

		fbxobj_miku[AnimationType::ROLLING]->SetPosition(m_pos);
		fbxobj_shadowMiku[AnimationType::ROLLING]->SetPosition(m_pos);
		fbxobj_miku[AnimationType::ROLLING]->Update();
		fbxobj_shadowMiku[AnimationType::ROLLING]->Update(true);

		ImguiControl::Imgui_playerAniType = "ROLLING";

		if (fbxobj_miku[AnimationType::ROLLING]->IsAnimationEnd())
		{
			m_isAnimation = false;
			m_isInvincible = false;
		}
	}

	//Imgui
	SetImgui();

	bones = fbxobj_miku[m_animationType]->GetAffineTrans();
	matRot = fbxobj_miku[m_animationType]->GetMatRots();
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
			for (int i = 0; i < C_MIKU_NUM; i++)
			{
				fbxobj_miku[i]->SetDrawSkipNum(3);
				fbxobj_miku[i]->SetDrawSkipNum(4);
				fbxobj_miku[i]->SetDrawSkipNum(5);
				fbxobj_miku[i]->SetDrawSkipNum(6);
				fbxobj_miku[i]->SetDrawSkipNum(7);
				fbxobj_miku[i]->SetDrawSkipNum(10);
				fbxobj_miku[i]->SetDrawSkipNum(12);
				fbxobj_miku[i]->SetDrawSkipNum(14);

				fbxobj_shadowMiku[i]->SetDrawSkipNum(3);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(4);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(5);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(6);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(7);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(10);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(12);
				fbxobj_shadowMiku[i]->SetDrawSkipNum(14);
			}
		}
		else
		{
			for (int i = 0; i < C_MIKU_NUM; i++)
			{
				fbxobj_miku[i]->ResetDrawSkipNum();
				fbxobj_shadowMiku[i]->ResetDrawSkipNum();
			}
		}
	}
	m_isHelmet = ImguiControl::isHel;

	obj_Helmet->MultiMatrix(bones[2].second);
	obj_Sword->MultiMatrix(fbxobj_miku[m_animationType]->GetMatrix());
	obj_ShadowSword->MultiMatrix(fbxobj_miku[m_animationType]->GetMatrix());

	obj_Helmet->Update();
	obj_Sword->Update();
	obj_ShadowSword->SetRotation(obj_Sword->GetRotation());
	obj_ShadowSword->Update(true);
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
		fbxobj_miku[m_animationType]->ResetAnimation();
		fbxobj_shadowMiku[m_animationType]->ResetAnimation();
	}

	//補間計算
	else
	{
		//途中切り替え時
		if (m_animationType != m_keepAnimationType)
		{
			m_blendTimer = 0.0f;
			//m_oldAnimationType = m_animationType;
			m_keepAnimationType = m_animationType;
			fbxobj_miku[m_animationType]->ResetAnimation();
			fbxobj_shadowMiku[m_animationType]->ResetAnimation();
		}

		//タイマー計算
		else if (m_blendTimer < 1.0f)
		{
			m_blendTimer += C_MAX_BLEND_TIMER;
			//fbxobj_creature[m_oldAnimationType]->Update();
			if (m_blendTimer > 1.0f)
			{
				m_blendTimer = 1.0f;
				m_isChange = false;
				m_oldAnimationType = m_animationType;
			}
		}
		else
		{
			m_isChange = false;
			m_oldAnimationType = m_animationType;
		}
	}
	ImguiControl::Imgui_playerBlendTimer = m_blendTimer;

	//BindAnimation
	fbxobj_miku[m_animationType]->
		BlendAnimation(fbxobj_miku[m_oldAnimationType], OgaJEase::easeOutCubic(m_blendTimer), true);
	fbxobj_shadowMiku[m_animationType]->
		BlendAnimation(fbxobj_shadowMiku[m_oldAnimationType], OgaJEase::easeOutCubic(m_blendTimer), true);
}

void Player::CalcAttackTimer()
{
	if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_3)
	{
		int l_animationNum = m_animationType - 3;
		if (m_animationTimer < m_attackCollisionTimer[l_animationNum])
		{
			m_animationTimer++;
		}
		else
		{
			m_animationTimer = 0;
			m_isAttack = true;

			m_healTimer = C_HEAL_TIMER;
			m_stamina -= C_ATTACK_SUB_STAMINA;
			if (m_stamina < 0)
			{
				m_stamina = 0;
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

	m_oldAnimationType = m_animationType;
	m_animationType = animationType;
	fbxobj_miku[animationType]->ReplayAnimation();
	fbxobj_shadowMiku[animationType]->ReplayAnimation();
}

void Player::SetImgui()
{
	//Old
	if (m_oldAnimationType == STAND)
	{
		ImguiControl::Imgui_playerOldAniType = "STAND";
	}
	else if (m_oldAnimationType == SLOWRUN)
	{
		ImguiControl::Imgui_playerOldAniType = "SLOWRUN";
	}
	else if (m_oldAnimationType == RUN)
	{
		ImguiControl::Imgui_playerOldAniType = "RUN";
	}
	else if (m_oldAnimationType >= NORMAL_ATTACK_1 &&
		m_oldAnimationType <= NORMAL_ATTACK_3)
	{
		if (m_oldAnimationType == NORMAL_ATTACK_1)
		{
			ImguiControl::Imgui_playerOldAniType = "NORMAL_ATTACK_1";
		}
		else if (m_oldAnimationType == NORMAL_ATTACK_2)
		{
			ImguiControl::Imgui_playerOldAniType = "NORMAL_ATTACK_2";
		}
		else if (m_oldAnimationType == NORMAL_ATTACK_3)
		{
			ImguiControl::Imgui_playerOldAniType = "NORMAL_ATTACK_3";
		}
	}
	else if (m_oldAnimationType == DAMAGED)
	{
		ImguiControl::Imgui_playerOldAniType = "DAMAGED";
	}
	else if (m_oldAnimationType == ROLLING)
	{
		ImguiControl::Imgui_playerOldAniType = "ROLLING";
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

	ImguiControl::Imgui_playerCurrentAniTimer = fbxobj_miku[m_animationType]->GetNowTime();
	ImguiControl::Imgui_playerOldAniTimer = fbxobj_miku[m_oldAnimationType]->GetNowTime();

	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetPosition(DirectX::XMFLOAT3(
			ImguiControl::p[i][0],
			ImguiControl::p[i][1],
			ImguiControl::p[i][2]
		));
	}
	obj_Box[2]->SetPosition(DirectX::XMFLOAT3(0, 2.0f, 0));
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
	m_isAnimation = true;
	m_isInvincible = true;
	m_isAccept = true;
	m_oldAnimationType = m_animationType;
	m_animationType = DAMAGED;
	if (m_hp < 0) { m_hp = 0; }
	OutputDebugStringA("Hit!\n");
}
