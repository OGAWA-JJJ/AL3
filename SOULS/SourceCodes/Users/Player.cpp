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
	m_estusHeal = 0;

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
	m_isEstus = false;

#pragma endregion

#pragma region StatusInit

	m_hp = C_MAX_HP;
	m_mp = C_MAX_MP;
	m_stamina = C_MAX_STAMINA;
	m_power = C_MAX_POWER[0];
	m_estus = C_MAX_ESTUS;

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

	fbxobj_miku->StopAnimation(DIE);
	fbxobj_miku->PlayAnimation(DAMAGED);

	fbxobj_miku->StopAnimation(ROLLING);
	fbxobj_miku->SetLoopAnimation(ROLLING, false);

	fbxobj_miku->PlayAnimation(HEAL);

	//Fbx(Shadow)
	fbxobj_shadowMiku = FbxObjects::Create(ModelManager::fbxmodel_mikus);

	fbxobj_shadowMiku->PlayAnimation(STAND);
	fbxobj_shadowMiku->PlayAnimation(SLOWRUN);
	fbxobj_shadowMiku->PlayAnimation(RUN);
	fbxobj_shadowMiku->PlayAnimation(BACK_RUN);
	fbxobj_shadowMiku->PlayAnimation(R_RUN);
	fbxobj_shadowMiku->PlayAnimation(L_RUN);

	fbxobj_shadowMiku->StopAnimation(NORMAL_ATTACK_1);
	fbxobj_shadowMiku->SetLoopAnimation(NORMAL_ATTACK_1, false);

	fbxobj_shadowMiku->StopAnimation(NORMAL_ATTACK_2);
	fbxobj_shadowMiku->SetLoopAnimation(NORMAL_ATTACK_2, false);

	fbxobj_shadowMiku->StopAnimation(NORMAL_ATTACK_3);
	fbxobj_shadowMiku->SetLoopAnimation(NORMAL_ATTACK_3, false);

	fbxobj_shadowMiku->StopAnimation(DIE);
	fbxobj_shadowMiku->StopAnimation(DAMAGED);

	fbxobj_shadowMiku->StopAnimation(ROLLING);
	fbxobj_shadowMiku->SetLoopAnimation(ROLLING, false);

	fbxobj_shadowMiku->PlayAnimation(HEAL);

#pragma endregion

#pragma region ModelInit

	const float Sword_Scale = 2.0f;
	const float Miku_Scale = 0.02f;

	//Obj
	obj_Sword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));
	obj_ShadowSword->SetScale(XMFLOAT3(Sword_Scale, Sword_Scale, Sword_Scale));

	//Fbx
	fbxobj_miku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_miku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_miku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_shadowMiku->SetPosition(XMFLOAT3(0, 0, 0));
	fbxobj_shadowMiku->SetScale(XMFLOAT3(Miku_Scale, Miku_Scale, Miku_Scale));
	fbxobj_shadowMiku->SetRotation(XMFLOAT3(0, 0, 0));

	fbxobj_miku->SetDrawSkipNum(3);
	fbxobj_miku->SetDrawSkipNum(4);
	fbxobj_miku->SetDrawSkipNum(5);
	fbxobj_miku->SetDrawSkipNum(6);
	fbxobj_miku->SetDrawSkipNum(7);
	fbxobj_miku->SetDrawSkipNum(10);
	fbxobj_miku->SetDrawSkipNum(12);
	fbxobj_miku->SetDrawSkipNum(14);

	fbxobj_shadowMiku->SetDrawSkipNum(3);
	fbxobj_shadowMiku->SetDrawSkipNum(4);
	fbxobj_shadowMiku->SetDrawSkipNum(5);
	fbxobj_shadowMiku->SetDrawSkipNum(6);
	fbxobj_shadowMiku->SetDrawSkipNum(7);
	fbxobj_shadowMiku->SetDrawSkipNum(10);
	fbxobj_shadowMiku->SetDrawSkipNum(12);
	fbxobj_shadowMiku->SetDrawSkipNum(14);

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

	delete fbxobj_miku;
	delete fbxobj_shadowMiku;
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

	m_cameraY = 50.0f;

	//Test
	obj_SwordBox->SetPosition(DirectX::XMFLOAT3(
		0.0f,
		6.0f,
		0.0f
	));

	ImguiControl::Imgui_cameraDist = C_MAX_CAMERA_NEAR_DISTANCE;

	//パーティクル
	const float pScale = 0.5f;
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
	pManager.SetCreateNum(1);

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
}

void Player::Update(DirectX::XMFLOAT3 enemyPos)
{
	if (IsDead())
	{
		if (fbxobj_miku->IsAnimationEnd(AnimationType::DIE))
		{
			fbxobj_miku->StopAnimation(AnimationType::DIE);
			fbxobj_shadowMiku->StopAnimation(AnimationType::DIE);
		}

		fbxobj_miku->SetPosition(m_pos);
		fbxobj_shadowMiku->SetPosition(m_pos);
		fbxobj_miku->Update();
		fbxobj_shadowMiku->Update();
		return;
	}

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

					fbxobj_miku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
					fbxobj_shadowMiku->SetRotation(XMFLOAT3(0, deg + cameraRad, 0));
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
			DirectX::XMFLOAT3 cameraToPlayer = OgaJHelper::CalcDirectionVec3(Camera::GetEye(), pos);
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
			fbxobj_shadowMiku->SetRotation(l_rot);
		}
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

	//エスト管理
	if (m_estusHeal > 0)
	{
		//誤差が発生する可能性アリ
		m_estusHeal -= C_MAX_ESTUS_HEAL_SPEED;
		if (m_hp < C_MAX_HP)
		{
			m_hp += C_MAX_ESTUS_HEAL_SPEED;
			if (m_hp >= C_MAX_HP)
			{
				m_hp = C_MAX_HP;
				m_estusHeal = 0;
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
	if (ImguiControl::Imgui_isPlayerDraw)
	{
		fbxobj_miku->Draw(PipelineManager::fbx_normal);
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
	if (ImguiControl::Imgui_isPlayerDraw)
	{
		fbxobj_shadowMiku->Draw(PipelineManager::fbx_shadow);
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
				CalcRolling();
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
	fbxobj_shadowMiku->SetPosition(m_pos);
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
	//Update
	fbxobj_miku->SetAnimationIndex(m_animationType);
	fbxobj_shadowMiku->SetAnimationIndex(m_animationType);

	fbxobj_miku->Update();
	fbxobj_shadowMiku->Update(true);

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

		fbxobj_miku->SetPosition(m_pos);
		fbxobj_shadowMiku->SetPosition(m_pos);

		if (fbxobj_miku->IsAnimationEnd(m_animationType))
		{
			m_isAnimation = false;
			m_isInvincible = false;
		}

		ImguiControl::Imgui_playerAniType = "ROLLING";
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

			fbxobj_shadowMiku->SetDrawSkipNum(3);
			fbxobj_shadowMiku->SetDrawSkipNum(4);
			fbxobj_shadowMiku->SetDrawSkipNum(5);
			fbxobj_shadowMiku->SetDrawSkipNum(6);
			fbxobj_shadowMiku->SetDrawSkipNum(7);
			fbxobj_shadowMiku->SetDrawSkipNum(10);
			fbxobj_shadowMiku->SetDrawSkipNum(12);
			fbxobj_shadowMiku->SetDrawSkipNum(14);
		}
		else
		{

			fbxobj_miku->ResetDrawSkipNum();
			fbxobj_shadowMiku->ResetDrawSkipNum();

		}
	}
	m_isHelmet = ImguiControl::isHel;

	obj_Helmet->MultiMatrix(bones[2].second);
	obj_Sword->MultiMatrix(fbxobj_miku->GetMatrix());
	obj_ShadowSword->MultiMatrix(fbxobj_miku->GetMatrix());

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
		fbxobj_miku->ResetAnimation(m_animationType);
		fbxobj_shadowMiku->ResetAnimation(m_animationType);
		fbxobj_miku->SetAnimationSpeed(0, 0, false);
		fbxobj_shadowMiku->SetAnimationSpeed(0, 0, false);
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
			fbxobj_shadowMiku->ResetAnimation(m_animationType);
		}

		//タイマー計算
		else if (m_blendTimer < 1.0f)
		{
			m_blendTimer += C_MAX_BLEND_TIMER;
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

	fbxobj_miku->
		BlendAnimation(m_oldAnimationType, m_animationType, OgaJEase::easeOutCubic(m_blendTimer), true);
	fbxobj_shadowMiku->
		BlendAnimation(m_oldAnimationType, m_animationType, OgaJEase::easeOutCubic(m_blendTimer), true);
}

void Player::CalcAttack()
{
	//攻撃力
	m_power = C_MAX_POWER[m_animationType - 6];

	//最大までのフレーム数
	float l_frame = fbxobj_miku->GetEndTime(m_animationType) / fbxobj_miku->GetAddTime(m_animationType);
	fbxobj_miku->SetAnimationSpeed(m_animationType, OgaJEase::easeInSine(m_attackEase), true);
	fbxobj_shadowMiku->SetAnimationSpeed(m_animationType, OgaJEase::easeInSine(m_attackEase), true);
	if (m_attackEase < 1.0f)
	{
		m_attackEase += 1.0f / l_frame;
	}
	else
	{
		m_attackEase = 1.0f;
	}

	//攻撃アニメーション中断ローリング
	if (!m_isAccept)
	{
		if (fbxobj_miku->GetNowTime(m_animationType) >=
			fbxobj_miku->GetAddTime(m_animationType) * C_ATTACK_COLLISION_ENDTIMER)
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
	}
}

void Player::CalcAttackTimer()
{
	if (m_animationType >= NORMAL_ATTACK_1 &&
		m_animationType <= NORMAL_ATTACK_3)
	{
		int l_animationNum = m_animationType - NORMAL_ATTACK_1;
		if (m_animationTimer < m_attackCollisionTimer[l_animationNum])
		{
			m_animationTimer++;
		}
		else
		{
			m_animationTimer = 0;
			m_isAttack = true;

			m_healTimer = C_AUTOHEAL_STAMINA_TIMER;
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
	fbxobj_miku->PlayAnimation(animationType);
	fbxobj_shadowMiku->PlayAnimation(animationType);
}

void Player::CalcRolling()
{
	m_padState = 0;

	m_healTimer = C_AUTOHEAL_STAMINA_TIMER;
	m_stamina -= C_ROLLING_SUB_STAMINA;
	if (m_stamina < 0)
	{
		m_stamina = 0;
	}

	m_oldAnimationType = m_animationType;
	m_animationType = ROLLING;
	fbxobj_miku->ResetAnimation(AnimationType::ROLLING);
	fbxobj_miku->PlayAnimation(AnimationType::ROLLING);
	fbxobj_shadowMiku->ResetAnimation(AnimationType::ROLLING);
	fbxobj_shadowMiku->PlayAnimation(AnimationType::ROLLING);
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
		fbxobj_miku->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
		fbxobj_shadowMiku->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
	}
	else
	{
		XMFLOAT3 l_deg = fbxobj_miku->GetRotation();
		OgaJHelper::ConvertToRadian(l_deg.y);
		float l_radX = sinf(l_deg.y);
		float l_radZ = cosf(l_deg.y);

		m_rollingAngle = { l_radX,0,l_radZ };
		float l_degY = atan2(m_rollingAngle.x, m_rollingAngle.z) * 180.0f / DirectX::XM_PI;
		fbxobj_miku->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
		fbxobj_shadowMiku->SetRotation(DirectX::XMFLOAT3(0, l_degY, 0));
	}
}

void Player::CalcHeal()
{
	if (m_estusTimer < C_ESTUS_TIMER)
	{
		m_estusTimer++;
	}
	else
	{
		m_estus--;
		m_estusHeal = C_MAX_ESTUS_HEAL;
		m_isEstus = true;
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
	if (m_hp < 0) { m_hp = 0; }
	OutputDebugStringA("Hit!\n");
}
