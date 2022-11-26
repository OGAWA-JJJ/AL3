#include "Enemy.h"
#include "../DirectX/DirectXImportant.h"
#include "../Users/PipelineManager.h"
#include "../Math/OgaJHelper.h"
#include "ModelManager.h"
#include "../../imgui/ImguiControl.h"
#include <random>

Enemy::Enemy()
{
#pragma region Init

	m_pos = { 0,0,0 };
	m_swingDownStartPos = { 0,0,0 };
	m_swingDownEndPos = { 0,0,0 };
	m_animationTimer = 0;
	m_animationType = STAND;
	m_oldAnimationType = m_animationType;
	//m_boneCount = 0;
	m_hitOBBNum = 0;
	m_deg = 180.0f;
	m_easeTimer = 0.0f;
	m_turnStartAngle = 0.0f;
	m_turnEndAngle = 0.0f;
	m_dist = 0.0f;
	m_blendTimer = 0.0f;
	m_riseStartY = 0;
	m_isInvincible = false;
	m_isAttack = false;
	m_isAttackTrigger = false;
	m_isCalc = false;
	m_isCalcEnd = false;
	m_isTurn = false;
	m_turnVec = false;
	m_isBackAttackLottery = false;
	m_isBackAttack = false;
	m_isRise = false;
	m_isSwing = false;
	m_isChange = false;

#pragma endregion

#pragma region StatusInit

	m_hp = C_MAX_HP;

#pragma endregion

#pragma region ModelCreate

	const float boxSize = 6.0f;
	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(boxSize, boxSize, boxSize));
	}

	fbxobj_creature[AnimationType::STAND] = FbxObjects::Create(ModelManager::fbxmodel_idleCreature);
	fbxobj_creature[AnimationType::STAND]->PlayAnimation();

	fbxobj_creature[AnimationType::RUN] = FbxObjects::Create(ModelManager::fbxmodel_runCreature);
	fbxobj_creature[AnimationType::RUN]->PlayAnimation();

	fbxobj_creature[AnimationType::KICK] = FbxObjects::Create(ModelManager::fbxmodel_kickCreature);
	fbxobj_creature[AnimationType::KICK]->PlayAnimation();

	fbxobj_creature[AnimationType::PUNCH] = FbxObjects::Create(ModelManager::fbxmodel_punchCreature);
	fbxobj_creature[AnimationType::PUNCH]->PlayAnimation();

	fbxobj_creature[AnimationType::DIE] = FbxObjects::Create(ModelManager::fbxmodel_dieCreature);
	fbxobj_creature[AnimationType::DIE]->PlayAnimation();

	fbxobj_creature[AnimationType::R_TURN] = FbxObjects::Create(ModelManager::fbxmodel_RTurnCreature);
	fbxobj_creature[AnimationType::R_TURN]->PlayAnimation();

	fbxobj_creature[AnimationType::L_TURN] = FbxObjects::Create(ModelManager::fbxmodel_LTurnCreature);
	fbxobj_creature[AnimationType::L_TURN]->PlayAnimation();

	fbxobj_creature[AnimationType::R_BACK] = FbxObjects::Create(ModelManager::fbxmodel_RBackCreature);
	fbxobj_creature[AnimationType::R_BACK]->PlayAnimation();

	fbxobj_creature[AnimationType::L_BACK] = FbxObjects::Create(ModelManager::fbxmodel_LBackCreature);
	fbxobj_creature[AnimationType::L_BACK]->PlayAnimation();

	fbxobj_creature[AnimationType::EXPLOSION] = FbxObjects::Create(ModelManager::fbxmodel_explosionCreature);
	fbxobj_creature[AnimationType::EXPLOSION]->PlayAnimation();

	fbxobj_creature[AnimationType::RISE] = FbxObjects::Create(ModelManager::fbxmodel_riseCreature);
	fbxobj_creature[AnimationType::RISE]->PlayAnimation();

	fbxobj_creature[AnimationType::SWING_DOWN] = FbxObjects::Create(ModelManager::fbxmodel_swingDownCreature);
	fbxobj_creature[AnimationType::SWING_DOWN]->PlayAnimation();

#pragma endregion

#pragma region ModelInit

	const float Creature_Scale = 0.4f;
	for (int i = 0; i < C_CREATURE_NUM; i++)
	{
		fbxobj_creature[i]->SetScale(DirectX::XMFLOAT3(Creature_Scale, Creature_Scale, Creature_Scale));
	}

#pragma endregion
}

Enemy::~Enemy()
{
	for (int i = 0; i < 37; i++)
	{
		delete obj_Box[i];
	}

	for (int i = 0; i < 12; i++)
	{
		delete fbxobj_creature[i];
	}
}

void Enemy::Init()
{
	m_pos = DirectX::XMFLOAT3(0.0f, 0.0f, -300.0f);

	//パーティクル
	const float pScale = 1.0f;
	const float pPower = 3.0f;

	pManager.Init();
	Particle::ParticleData pData;
	pData.isRandVec = true;
	pData.isRandColor = false;
	pData.life = 30;
	pData.scale = { pScale, pScale, pScale };
	pData.power = { pPower, pPower, pPower };
	pData.color = { 0.9f, 0.2f, 0.2f, 1.0f };
	for (int i = 0; i < pManager.GetMaxParticle(); i++)
	{
		pManager.SetParticle(i, pData);
	}
	pManager.SetCreateNum(pManager.GetMaxParticle());
	pManager.SetIsCreateStop(true);

	//Box
	std::array<float, 12> l_x = {
		15,
		6,
		5,
		6.6f,
		6,
		6,
		5,
		5,
		6,
		5,
		5,
		6
	};
	std::array<float, 12> l_y = {
		10,
		6,
		7,
		15,
		8,
		10,
		7,
		7,
		7,
		7,
		7,
		7
	};
	std::array<float, 12> l_z = {
		10,
		6,
		5,
		6.6f,
		6,
		6,
		5,
		5,
		6,
		5,
		5,
		6
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

void Enemy::Update(DirectX::XMFLOAT3 playerPos)
{
	if (IsDead())
	{
		if (fbxobj_creature[AnimationType::DIE]->IsAnimationEnd())
		{
			fbxobj_creature[AnimationType::DIE]->StopAnimation();
		}

		fbxobj_creature[AnimationType::DIE]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::DIE]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::DIE]->Update();
		return;
	}

	if (m_animationType != STAND)
	{
		//非攻撃中かつ振り向き中じゃない
		if (!m_isAttack)
		{
			DirectX::XMFLOAT3 l_pos = { m_pos.x,0,m_pos.z };
			float l_dist = OgaJHelper::CalcDist(l_pos, playerPos);
			if (!m_isTurn)
			{
				//完全追従←ゆっくり振り向け(定数足す)
				DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(playerPos, m_pos);
				eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
				m_deg = atan2(eDirection.x, eDirection.z);
				OgaJHelper::ConvertToDegree(m_deg);

				if (l_dist >= C_MAX_DIST)
				{
					m_animationType = RUN;
					m_pos.x += -eDirection.x * C_MAX_MOVE_SPEED;
					m_pos.z += -eDirection.z * C_MAX_MOVE_SPEED;
				}
			}

			JudgAnimationType(l_dist);
		}

		CalcAngleDiff(playerPos);
	}

	//切り替わったら
	if (!m_isChange &&
		fbxobj_creature[m_animationType] != fbxobj_creature[m_oldAnimationType])
	{
		m_blendTimer = 0.0f;
		m_isChange = true;
		fbxobj_creature[m_animationType]->ResetAnimation();
	}

	//補間計算
	else
	{
		if (m_blendTimer < 1.0f)
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
	ImguiControl::Imgui_enemyBlendTimer = m_blendTimer;

	//BindAnimation
	fbxobj_creature[m_animationType]->
		BlendAnimation(fbxobj_creature[m_oldAnimationType], OgaJEase::easeOutCubic(m_blendTimer), true);

	//FbxUpdate
	if (m_animationType == STAND)
	{
		fbxobj_creature[AnimationType::STAND]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::STAND]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::STAND]->Update();

		ImguiControl::Imgui_enemyAniType = "STAND";
	}
	else if (m_animationType == RUN)
	{
		m_isAttack = false;
		fbxobj_creature[AnimationType::RUN]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::RUN]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::RUN]->Update();

		ImguiControl::Imgui_enemyAniType = "RUN";
	}
	else if (m_animationType == KICK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_KICK_COLLISION_TIMER, C_KICK_COLLISION_ENDTIMER);
		}

		fbxobj_creature[AnimationType::KICK]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::KICK]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::KICK]->Update();

		ImguiControl::Imgui_enemyAniType = "KICK";

		if (fbxobj_creature[AnimationType::KICK]->IsAnimationEnd())
		{
			fbxobj_creature[AnimationType::RUN]->SetPosition(m_pos);
			fbxobj_creature[AnimationType::RUN]->Update();

			m_isAttack = false;

			CalcNearAngle(playerPos, fbxobj_creature[AnimationType::KICK]->GetRotation().y);
		}
	}
	else if (m_animationType == PUNCH)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_PUNCH_COLLISION_TIMER, C_PUNCH_COLLISION_ENDTIMER);
		}

		fbxobj_creature[AnimationType::PUNCH]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::PUNCH]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::PUNCH]->Update();

		ImguiControl::Imgui_enemyAniType = "PUNCH";

		if (fbxobj_creature[AnimationType::PUNCH]->IsAnimationEnd())
		{
			fbxobj_creature[AnimationType::PUNCH]->SetPosition(m_pos);
			fbxobj_creature[AnimationType::PUNCH]->Update();

			m_isAttack = false;

			CalcNearAngle(playerPos, fbxobj_creature[AnimationType::PUNCH]->GetRotation().y);
		}
	}
	else if (m_animationType == R_TURN)
	{
		ImguiControl::Imgui_enemyAniType = "R_TURN";
	}
	else if (m_animationType == L_TURN)
	{
		ImguiControl::Imgui_enemyAniType = "L_TURN";
	}
	else if (m_animationType == R_BACK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}

		ImguiControl::Imgui_enemyAniType = "R_BACK";
	}
	else if (m_animationType == L_BACK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}

		ImguiControl::Imgui_enemyAniType = "L_BACK";
	}
	else if (m_animationType == EXPLOSION)
	{
		fbxobj_creature[AnimationType::EXPLOSION]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::EXPLOSION]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::EXPLOSION]->Update();

		ImguiControl::Imgui_enemyAniType = "EXPLOSION";

		if (fbxobj_creature[AnimationType::EXPLOSION]->IsAnimationEnd())
		{
			fbxobj_creature[AnimationType::EXPLOSION]->SetPosition(m_pos);
			fbxobj_creature[AnimationType::EXPLOSION]->Update();

			m_isAttack = false;

			CalcNearAngle(playerPos, fbxobj_creature[AnimationType::EXPLOSION]->GetRotation().y);
		}
	}
	else if (m_animationType == RISE)
	{
		CalcRise(playerPos);
		ImguiControl::Imgui_enemyAniType = "RISE";
	}
	else if (m_animationType == SWING_DOWN)
	{
		CalcSwingDown(playerPos);
		ImguiControl::Imgui_enemyAniType = "SWING_DOWN";
	}

	ImguiControl::Imgui_enemyCurrentAniTimer = fbxobj_creature[m_animationType]->GetNowTime();
	ImguiControl::Imgui_enemyOldAniTimer = fbxobj_creature[m_oldAnimationType]->GetNowTime();

	CalcOBB();

	if (m_isInvincible)
	{
		ImguiControl::Imgui_enemyInvi = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_enemyInvi = "FALSE";
	}


	if (pManager.IsArrivalCreateNum())
	{
		pManager.SetIsCreateStop(true);
	}
	pManager.Update();
}

void Enemy::Draw()
{
	if (IsDead())
	{
		fbxobj_creature[AnimationType::DIE]->Draw(PipelineManager::fbx_normal);
		return;
	}

	if (ImguiControl::Imgui_isEnemyDraw)
	{
		fbxobj_creature[m_animationType]->Draw(PipelineManager::fbx_normal);
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());

	if (ImguiControl::Imgui_isOBBDraw)
	{
		for (int i = 0; i < obj_Box.size(); i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_normal);
		}
	}

	Object::PostDraw();

	pManager.Draw();
}

void Enemy::CalcOBB()
{
	//OBB計算
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> l_affine =
		fbxobj_creature[m_animationType]->GetAffineTrans();
	std::vector<DirectX::XMMATRIX> l_matRot =
		fbxobj_creature[m_animationType]->GetMatRots();

	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetPosition(DirectX::XMFLOAT3(
			ImguiControl::e[i][0],
			ImguiControl::e[i][1],
			ImguiControl::e[i][2]
		));
	}

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
			l_affine.erase(l_affine.begin() + (i - l_eraseCount));
			l_matRot.erase(l_matRot.begin() + (i - l_eraseCount));
			l_eraseCount++;
		}

		i++;
	}

	std::vector<OBB> l_obbs;
	for (int i = 0; i < l_affine.size(); i++)
	{
		obj_Box[i]->MultiMatrix(l_affine[i].second);
		obj_Box[i]->Update();

		OBB l_obb;
		l_obb.pos = DirectX::XMFLOAT3(
			l_affine[i].second.r[3].m128_f32[0],
			l_affine[i].second.r[3].m128_f32[1],
			l_affine[i].second.r[3].m128_f32[2]);
		l_obb.matrix = l_matRot[i];
		l_obb.length = obj_Box[i]->GetScale();

		l_obbs.push_back(l_obb);
	}
	m_obbs = l_obbs;
}

void Enemy::JudgAnimationType(float dist)
{
	int l_div = 200;
	int l_rand = std::rand();
	if (m_isTurn)
	{
		l_div = 1;
		l_rand = l_rand % l_div;

		if (l_rand == l_div - 1 &&
			m_animationType != RUN &&
			m_animationType != R_BACK &&
			m_animationType != L_BACK &&
			m_isBackAttackLottery)
		{
			if (m_turnVec == RIGHT)
			{
				m_animationType = R_BACK;
				fbxobj_creature[AnimationType::R_BACK]->SetPosition(m_pos);
			}
			else
			{
				m_animationType = L_BACK;
				fbxobj_creature[AnimationType::L_BACK]->SetPosition(m_pos);
			}

			m_isAttack = true;
			m_isCalcEnd = false;
			m_isBackAttackLottery = false;
			m_isBackAttack = true;
			m_isTurn = false;

			return;
		}
		else
		{
			if (m_turnVec == RIGHT)
			{
				m_animationType = R_TURN;
			}
			else
			{
				m_animationType = L_TURN;
			}
		}

		return;
	}
	else if (dist > C_MAX_DIST)
	{
		//return;
		m_animationType = RUN;
	}
	else
	{
		//ここの管理方法を考える
		/*m_riseStartY = 0.0f;
		m_isAttack = true;
		m_animationType = RISE;
		fbxobj_creature[AnimationType::RISE]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::RISE]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::SWING_DOWN]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));

		fbxobj_creature[AnimationType::RISE]->ResetAnimation();
		fbxobj_creature[AnimationType::SWING_DOWN]->ResetAnimation();
		return;*/

		m_isAttack = true;
		m_isCalc = false;
		m_isCalcEnd = false;

		l_rand = l_rand % l_div;
		if (l_rand == l_div - 1 &&
			dist < C_MAX_DIST)
		{
			m_animationType = EXPLOSION;
			//fbxobj_explosionCreature->ResetAnimation();
		}

		else if (l_rand % 2 == 0)
		{
			m_animationType = KICK;
			//fbxobj_kickCreature->ResetAnimation();
		}
		else
		{
			m_animationType = PUNCH;
			//fbxobj_punchCreature->ResetAnimation();
		}
	}
}

void Enemy::CalcAngleDiff(DirectX::XMFLOAT3& pPos)
{
	if (m_isTurn)
	{
		float l_rotY = 0.0f;
		if (m_turnVec == RIGHT)
		{
			l_rotY = (m_turnEndAngle - m_turnStartAngle) * OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
			fbxobj_creature[AnimationType::R_TURN]->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
			fbxobj_creature[AnimationType::R_TURN]->Update();
		}
		else
		{
			l_rotY = (m_turnEndAngle - m_turnStartAngle) * OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
			fbxobj_creature[AnimationType::L_TURN]->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
			fbxobj_creature[AnimationType::L_TURN]->Update();
		}

		if (m_easeTimer < 1.0f)
		{
			m_easeTimer += C_MAX_TURN_TIMER;
		}
		else
		{
			m_deg = l_rotY - 180.0f;
			if (m_turnVec == RIGHT)
			{
				if (fbxobj_creature[AnimationType::R_TURN]->IsAnimationEnd())
				{
					//fbxobj_RTurnCreature->ResetAnimation();
					m_easeTimer = 0.0f;
					m_isTurn = false;
					JudgAnimationType(OgaJHelper::CalcDist(m_pos, pPos));
				}
			}
			else
			{
				if (fbxobj_creature[AnimationType::L_TURN]->IsAnimationEnd())
				{
					//fbxobj_LTurnCreature->ResetAnimation();
					m_easeTimer = 0.0f;
					m_isTurn = false;
					JudgAnimationType(OgaJHelper::CalcDist(m_pos, pPos));
				}
			}
		}
	}
	else if (m_isBackAttack)
	{
		if (m_turnVec == RIGHT)
		{
			if (m_easeTimer < 1.0f)
			{
				float l_rotY = (m_turnEndAngle - m_turnStartAngle) *
					OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
				fbxobj_creature[AnimationType::R_BACK]->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
				m_easeTimer += C_MAX_TURN_TIMER;
			}

			fbxobj_creature[AnimationType::R_BACK]->Update();
		}
		else
		{
			if (m_easeTimer < 1.0f)
			{
				float l_rotY = (m_turnEndAngle - m_turnStartAngle) *
					OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
				fbxobj_creature[AnimationType::L_BACK]->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
				m_easeTimer += C_MAX_TURN_TIMER;
			}

			fbxobj_creature[AnimationType::L_BACK]->Update();
		}

		if (m_easeTimer >= 1.0f)
		{
			m_deg = m_turnEndAngle - 180.0f;
			if (m_turnVec == RIGHT)
			{
				if (fbxobj_creature[AnimationType::R_BACK]->IsAnimationEnd())
				{
					//fbxobj_creature[AnimationType::R_BACK]->ResetAnimation();
					m_easeTimer = 0.0f;
					m_isBackAttack = false;
					m_isAttack = false;

					CalcNearAngle(pPos, fbxobj_creature[AnimationType::R_BACK]->GetRotation().y);
					m_isBackAttackLottery = false;
					float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
					JudgAnimationType(l_dist);
				}
			}
			else
			{
				if (fbxobj_creature[AnimationType::L_BACK]->IsAnimationEnd())
				{
					//fbxobj_LBackCreature->ResetAnimation();
					m_easeTimer = 0.0f;
					m_isBackAttack = false;
					m_isAttack = false;

					CalcNearAngle(pPos, fbxobj_creature[AnimationType::L_BACK]->GetRotation().y);
					m_isBackAttackLottery = false;
					float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
					JudgAnimationType(l_dist);
				}
			}
		}
	}
}

void Enemy::CalcNearAngle(DirectX::XMFLOAT3& pPos, float myAngleY)
{
	m_turnStartAngle = myAngleY;

	//正面ベクトルを角度に
	float rotY = myAngleY - 180.0f;
	OgaJHelper::ConvertToRadian(rotY);
	float l_x = sinf(rotY);
	float l_z = cosf(rotY);
	float l_enemyDeg = atan2(l_x, l_z);
	OgaJHelper::ConvertToDegree(l_enemyDeg);
	if (l_enemyDeg < 0)
	{
		l_enemyDeg += 359.9f;
	}

	//プレイヤー方向のベクトルを角度に
	DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(pPos, m_pos);
	eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
	float l_frontDeg = atan2(eDirection.x, eDirection.z);
	OgaJHelper::ConvertToDegree(l_frontDeg);
	if (l_frontDeg < 0)
	{
		l_frontDeg += 359.9f;
	}

	float l_nearSub = OgaJHelper::RotateEarliestArc(l_enemyDeg, l_frontDeg);
	//自機が範囲外なら振り向きアニメーション抽選をする
	float l_nearSubAbs = fabsf(l_nearSub);
	if (l_nearSubAbs > C_MAX_TURN_RAD)
	{
		m_isTurn = true;

		if (l_nearSubAbs >= C_MAX_BACK_RAD)
		{
			if (!m_isBackAttackLottery && OgaJHelper::CalcDist(m_pos, pPos) < C_MAX_DIST)
			{
				m_isBackAttackLottery = true;
			}
		}

		if (l_nearSub < 0)
		{
			m_turnVec = TurnVec::RIGHT;
			fbxobj_creature[AnimationType::R_TURN]->SetRotation(DirectX::XMFLOAT3(0, myAngleY, 0));
			fbxobj_creature[AnimationType::R_TURN]->SetPosition(m_pos);
			fbxobj_creature[AnimationType::R_TURN]->Update();
		}
		else
		{
			m_turnVec = TurnVec::LEFT;
			fbxobj_creature[AnimationType::L_TURN]->SetRotation(DirectX::XMFLOAT3(0, myAngleY, 0));
			fbxobj_creature[AnimationType::L_TURN]->SetPosition(m_pos);
			fbxobj_creature[AnimationType::L_TURN]->Update();
		}
	}
	else
	{
		m_isTurn = false;
	}

	m_turnEndAngle = myAngleY + l_nearSub;
	if (m_turnEndAngle < 0.0f)
	{
		m_turnEndAngle += 359.9f;
	}
	if (fabsf(m_turnStartAngle - m_turnEndAngle) >= 180.0f)
	{
		if (m_turnStartAngle < m_turnEndAngle)
		{
			m_turnStartAngle += 359.9f;
		}
		else
		{
			m_turnEndAngle += 359.9f;
		}
	}
}

void Enemy::CalcAttackCollisionTimer(const int startFrame, const int endFrame)
{
	if (!m_isCalc)
	{
		if (m_animationTimer < startFrame)
		{
			m_animationTimer++;
		}
		else
		{
			m_animationTimer = 0;
			m_isCalc = true;
		}
	}
	else
	{
		if (m_animationTimer < endFrame)
		{
			m_animationTimer++;
		}
		else
		{
			m_animationTimer = 0;
			m_isCalc = false;
			m_isCalcEnd = true;
		}
	}
}

void Enemy::CalcRise(DirectX::XMFLOAT3& pPos)
{
	if (m_animationTimer < C_RISE_TIMER)
	{
		m_animationTimer++;
	}
	else
	{
		m_pos.y = ((m_riseStartY + C_MAX_RISE_HEIGHT) - m_riseStartY) * OgaJEase::easeOutCubic(m_easeTimer) + m_riseStartY;
		if (m_easeTimer < 1.0f)
		{
			m_easeTimer += C_MAX_RISE_TIMER;
			if (m_easeTimer > 1.0f)
			{
				m_easeTimer = 1.0f;
			}
		}
		else
		{
			m_animationTimer = 0;
			m_easeTimer = 0.0f;
			m_swingDownStartPos = m_pos;
			m_swingDownEndPos = pPos;
			m_animationType = AnimationType::SWING_DOWN;
			fbxobj_creature[AnimationType::SWING_DOWN]->SetPosition(m_pos);
			fbxobj_creature[AnimationType::SWING_DOWN]->StopAnimation();
			fbxobj_creature[AnimationType::SWING_DOWN]->Update();
		}
		fbxobj_creature[AnimationType::RISE]->SetPosition(m_pos);
	}
	fbxobj_creature[AnimationType::RISE]->Update();
}

void Enemy::CalcSwingDown(DirectX::XMFLOAT3& pPos)
{
	bool isUpdate = true;
	if (m_animationTimer < C_SWING_DOWN_TIMER)
	{
		m_animationTimer++;
	}
	else
	{
		m_pos = OgaJEase::easeInCircXMFLOAT3(m_swingDownStartPos, m_swingDownEndPos, m_easeTimer);
		if (m_easeTimer < 1.0f)
		{
			m_easeTimer += C_MAX_SWING_DOWN_TIMER;
			if (m_easeTimer > 1.0f)
			{
				m_easeTimer = 1.0f;
			}
		}
		else
		{
			fbxobj_creature[AnimationType::SWING_DOWN]->ReplayAnimation();
			if (fbxobj_creature[AnimationType::SWING_DOWN]->IsAnimationEnd())
			{
				isUpdate = false;
				m_animationTimer = 0;
				m_easeTimer = 0.0f;
				float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
				JudgAnimationType(l_dist);
				fbxobj_creature[AnimationType::RUN]->SetPosition(m_pos);
				fbxobj_creature[AnimationType::RUN]->Update();
				fbxobj_creature[AnimationType::SWING_DOWN]->SetAnimationTimerMax();
				m_isAttack = false;
			}
		}
		fbxobj_creature[AnimationType::SWING_DOWN]->SetPosition(m_pos);
	}
	if (isUpdate)
	{
		fbxobj_creature[AnimationType::SWING_DOWN]->Update();
	}
}

/*----------呼ぶやつ----------*/
void Enemy::HitAttack(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0) { m_hp = 0; }
	m_isInvincible = true;

	DirectX::XMFLOAT3 l_pos = m_obbs[m_hitOBBNum].pos;
	for (int i = 0; i < pManager.GetMaxParticle(); i++)
	{
		//生成時のみ剣に付随
		if (!pManager.IsMove(i))
		{
			pManager.SetPosition(i, l_pos);
		}
	}
	pManager.SetIsCreateStop(false);

	OutputDebugStringA("Hit!\n");
}

OBB& Enemy::GetAttackOBB()
{
	if (m_animationType == KICK)
	{
		return m_obbs[36];
	}
	else
	{
		return m_obbs[26];
	}
}

bool Enemy::IsDead()
{
	if (m_hp <= 0) { return true; }
	return false;
}

bool Enemy::IsFighting()
{
	if (m_animationType == STAND) { return false; }
	return true;
}
