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

	//const float boxSize = 12.0f;
	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
		//obj_Box[i]->SetScale(DirectX::XMFLOAT3(boxSize, boxSize, boxSize));
	}

	obj_circle = Object::Create(ModelManager::model_circle);

	fbxobj_creature[AnimationType::STAND] = FbxObjects::Create(ModelManager::fbxmodel_idleCreature);
	fbxobj_creature[AnimationType::STAND]->PlayAnimation();

	fbxobj_creature[AnimationType::RUN] = FbxObjects::Create(ModelManager::fbxmodel_runCreature);
	fbxobj_creature[AnimationType::RUN]->PlayAnimation();

	fbxobj_creature[AnimationType::KICK] = FbxObjects::Create(ModelManager::fbxmodel_kickCreature);
	fbxobj_creature[AnimationType::KICK]->PlayAnimation();
	fbxobj_creature[AnimationType::KICK]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::PUNCH] = FbxObjects::Create(ModelManager::fbxmodel_punchCreature);
	fbxobj_creature[AnimationType::PUNCH]->PlayAnimation();
	fbxobj_creature[AnimationType::PUNCH]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::DIE] = FbxObjects::Create(ModelManager::fbxmodel_dieCreature);
	fbxobj_creature[AnimationType::DIE]->PlayAnimation();

	fbxobj_creature[AnimationType::R_TURN] = FbxObjects::Create(ModelManager::fbxmodel_RTurnCreature);
	fbxobj_creature[AnimationType::R_TURN]->PlayAnimation();
	fbxobj_creature[AnimationType::R_TURN]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::L_TURN] = FbxObjects::Create(ModelManager::fbxmodel_LTurnCreature);
	fbxobj_creature[AnimationType::L_TURN]->PlayAnimation();
	fbxobj_creature[AnimationType::L_TURN]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::R_BACK] = FbxObjects::Create(ModelManager::fbxmodel_RBackCreature);
	fbxobj_creature[AnimationType::R_BACK]->PlayAnimation();
	fbxobj_creature[AnimationType::R_BACK]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::L_BACK] = FbxObjects::Create(ModelManager::fbxmodel_LBackCreature);
	fbxobj_creature[AnimationType::L_BACK]->PlayAnimation();
	fbxobj_creature[AnimationType::L_BACK]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::EXPLOSION] = FbxObjects::Create(ModelManager::fbxmodel_explosionCreature);
	fbxobj_creature[AnimationType::EXPLOSION]->PlayAnimation();
	fbxobj_creature[AnimationType::EXPLOSION]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::RISE] = FbxObjects::Create(ModelManager::fbxmodel_riseCreature);
	fbxobj_creature[AnimationType::RISE]->PlayAnimation();
	fbxobj_creature[AnimationType::RISE]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::SWING_DOWN] = FbxObjects::Create(ModelManager::fbxmodel_swingDownCreature);
	fbxobj_creature[AnimationType::SWING_DOWN]->PlayAnimation();
	fbxobj_creature[AnimationType::SWING_DOWN]->SetLoopAnimation(false);

	fbxobj_creature[AnimationType::TACKLE] = FbxObjects::Create(ModelManager::fbxmodel_tackleCreature);
	fbxobj_creature[AnimationType::TACKLE]->PlayAnimation();

#pragma endregion

#pragma region ModelInit

	const float Creature_Scale = 0.6f;
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

	pManager_Hit.Init();
	Particle::ParticleData pData;
	pData.isRandVec = true;
	pData.isRandColor = false;
	pData.life = 30;
	pData.scale = { pScale, pScale, pScale };
	pData.power = { pPower, pPower, pPower };
	pData.color = { 0.9f, 0.2f, 0.2f, 1.0f };
	for (int i = 0; i < pManager_Hit.GetMaxParticle(); i++)
	{
		pManager_Hit.SetParticle(i, pData);
	}
	pManager_Hit.SetCreateNum(pManager_Hit.GetMaxParticle());
	pManager_Hit.SetIsCreateStop(true);

	const float pScale2 = 2.0f;
	const float pPower2 = 1.0f;
	pManager_Ex.Init();
	Particle::ParticleData pData2;
	pData2.isRandVec = false;
	pData2.life = 30;
	pData2.scale = { pScale2, pScale2, pScale2 };
	pData2.power = { pPower2, pPower2, pPower2 };
	pData2.color = { 0.9f, 0.9f, 0.2f, 1.0f };
	for (int i = 0; i < pManager_Ex.GetMaxParticle(); i++)
	{
		pManager_Ex.SetParticle(i, pData2);
	}
	pManager_Ex.SetIsCreateStop(true);

	//Box(enemyScale=0.4fがベース)
	std::array<float, 10> l_x = {
		15,
		6,
		5,
		6.6f,
		6,
		6,
		5,
		6,
		5,
		6
	};
	std::array<float, 10> l_y = {
		10,
		6,
		7,
		15,
		8,
		10,
		7,
		7,
		7,
		7
	};
	std::array<float, 10> l_z = {
		10,
		6,
		5,
		6.6f,
		6,
		6,
		5,
		6,
		5,
		6
	};
	float addScale = fbxobj_creature[AnimationType::STAND]->GetScale().x / 0.4f;
	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(
			l_x[i] * addScale,
			l_y[i] * addScale,
			l_z[i] * addScale
		));
		ImguiControl::Imgui_enemyOBBScale[i][0] = l_x[i] * addScale;
		ImguiControl::Imgui_enemyOBBScale[i][1] = l_y[i] * addScale;
		ImguiControl::Imgui_enemyOBBScale[i][2] = l_z[i] * addScale;
	}

	//丸影
	const float circleScale = 30.0f;
	obj_circle->SetScale(DirectX::XMFLOAT3(circleScale, circleScale, circleScale));
	obj_circle->SetColor(DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
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
					m_pos.x += -eDirection.x * C_MAX_MOVE_SPEED;
					m_pos.z += -eDirection.z * C_MAX_MOVE_SPEED;
				}
			}

			JudgAnimationType(l_dist);
		}

		CalcAngleDiff(playerPos);
	}

	CalcBlendAnimation();
	OtherUpdate(playerPos);
	CalcOBB();
	SetImgui();

	//パーティクル
	if (pManager_Hit.IsArrivalCreateNum())
	{
		pManager_Hit.SetIsCreateStop(true);
	}
	pManager_Hit.Update();

	DirectX::XMFLOAT3 l_pos = m_pos;
	l_pos.y += 1.0f;
	obj_circle->SetPosition(l_pos);
	obj_circle->Update();
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

	obj_circle->Draw(PipelineManager::obj_texColorReceice);
	if (ImguiControl::Imgui_isOBBDraw)
	{
		for (int i = 0; i < obj_Box.size(); i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_normal);
		}
	}

	Object::PostDraw();

	pManager_Hit.Draw();
	pManager_Ex.Draw();
}

void Enemy::LuminanceDraw()
{
	pManager_Ex.Draw();
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
			ImguiControl::Imgui_enemyOBBPos[i][0],
			ImguiControl::Imgui_enemyOBBPos[i][1],
			ImguiControl::Imgui_enemyOBBPos[i][2]
		));
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(
			ImguiControl::Imgui_enemyOBBScale[i][0],
			ImguiControl::Imgui_enemyOBBScale[i][1],
			ImguiControl::Imgui_enemyOBBScale[i][2]
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
				fbxobj_creature[AnimationType::R_BACK]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
				fbxobj_creature[AnimationType::R_BACK]->ReplayAnimation();
			}
			else
			{
				m_animationType = L_BACK;
				fbxobj_creature[AnimationType::L_BACK]->SetPosition(m_pos);
				fbxobj_creature[AnimationType::L_BACK]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
				fbxobj_creature[AnimationType::L_BACK]->ReplayAnimation();
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
				fbxobj_creature[AnimationType::R_TURN]->ReplayAnimation();
			}
			else
			{
				m_animationType = L_TURN;
				fbxobj_creature[AnimationType::L_TURN]->ReplayAnimation();
			}
		}
	}
	else if (dist > C_MAX_DIST)
	{
		l_rand = l_rand % l_div;
		if (dist < C_MAX_TACKLE_RANGE)
		{
			if (l_rand == 0)
			{
				m_isAttack = true;

				m_isTackleEnd = false;
				m_tackleCount = 0;
				m_animationType = TACKLE;
				fbxobj_creature[TACKLE]->ReplayAnimation();
				return;
			}
		}
		m_animationType = RUN;
	}
	else
	{
		m_isAttack = true;
		m_isCalc = false;
		m_isCalcEnd = false;

		l_rand = l_rand % l_div;
		if (l_rand % 7 == 0 &&
			m_oldAnimationType != EXPLOSION &&
			GetHpRate() < 0.75f)
		{
			m_pPowerCount = 20;
			pManager_Ex.SetCreateNum(m_createCount);
			pManager_Ex.SetIsCreateStop(false);
			m_animationType = EXPLOSION;
			fbxobj_creature[AnimationType::EXPLOSION]->ReplayAnimation();
		}

		//else if (l_rand % 10 == 0)
		//{
		//	//ここの管理方法を考える←仮
		//	m_riseStartY = 0.0f;
		//	m_animationType = RISE;
		//	fbxobj_creature[AnimationType::RISE]->SetPosition(m_pos);
		//	fbxobj_creature[AnimationType::RISE]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		//	fbxobj_creature[AnimationType::SWING_DOWN]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		//	fbxobj_creature[AnimationType::RISE]->ResetAnimation();
		//	fbxobj_creature[AnimationType::SWING_DOWN]->ResetAnimation();
		//}

		else if (m_oldAnimationType != KICK)
		{
			m_animationType = KICK;
			fbxobj_creature[AnimationType::KICK]->ReplayAnimation();
		}
		else
		{
			m_animationType = PUNCH;
			fbxobj_creature[AnimationType::PUNCH]->ReplayAnimation();
		}
	}
}

void Enemy::CalcAngleDiff(DirectX::XMFLOAT3& pPos)
{
	if (m_isTurn)
	{
		float l_rotY = 0.0f;
		m_deg = CalcDeg(pPos);
		if (fabsf(m_turnStartAngle - (m_deg + 180.0f)) >= 180.0f)
		{
			if (m_turnStartAngle < m_deg + 180.0f)
			{
				m_turnStartAngle += 359.9f;
			}
			else
			{
				m_deg += 359.9f;
			}
		}

		if (m_turnVec == RIGHT)
		{
			l_rotY = ((m_deg + 180.0f) - m_turnStartAngle) * OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
			fbxobj_creature[AnimationType::R_TURN]->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
		}
		else
		{
			l_rotY = ((m_deg + 180.0f) - m_turnStartAngle) * OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
			fbxobj_creature[AnimationType::L_TURN]->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
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
					m_easeTimer = 0.0f;
					m_isTurn = false;
				}
			}
			else
			{
				if (fbxobj_creature[AnimationType::L_TURN]->IsAnimationEnd())
				{
					m_easeTimer = 0.0f;
					m_isTurn = false;
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
		}

		if (m_easeTimer >= 1.0f)
		{
			m_deg = m_turnEndAngle - 180.0f;
			if (m_turnVec == RIGHT)
			{
				if (fbxobj_creature[AnimationType::R_BACK]->IsAnimationEnd())
				{
					m_easeTimer = 0.0f;
					m_isBackAttack = false;
					m_isAttack = false;

					CalcNearAngle(pPos, fbxobj_creature[AnimationType::R_BACK]->GetRotation().y);
					m_isBackAttackLottery = false;
					float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
				}
			}
			else
			{
				if (fbxobj_creature[AnimationType::L_BACK]->IsAnimationEnd())
				{
					m_easeTimer = 0.0f;
					m_isBackAttack = false;
					m_isAttack = false;

					CalcNearAngle(pPos, fbxobj_creature[AnimationType::L_BACK]->GetRotation().y);
					m_isBackAttackLottery = false;
					float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
				}
			}
		}

		//fbxobj_creature[m_animationType]->Update();
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
			//fbxobj_creature[AnimationType::R_TURN]->Update();
		}
		else
		{
			m_turnVec = TurnVec::LEFT;
			fbxobj_creature[AnimationType::L_TURN]->SetRotation(DirectX::XMFLOAT3(0, myAngleY, 0));
			fbxobj_creature[AnimationType::L_TURN]->SetPosition(m_pos);
			//fbxobj_creature[AnimationType::L_TURN]->Update();
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
			fbxobj_creature[AnimationType::SWING_DOWN]->SetRotation(fbxobj_creature[RISE]->GetRotation());
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
				//JudgAnimationType(l_dist);
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

void Enemy::CalcTackle(DirectX::XMFLOAT3& pPos)
{
	//違うとこでも結構求めてるから修正必要アリ(共有化)
	DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(pPos, m_pos);
	eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);

	//突進前
	if (m_animationTimer < C_MAX_TACKLE_TIMER)
	{
		m_deg = atan2(eDirection.x, eDirection.z);
		OgaJHelper::ConvertToDegree(m_deg);
		m_animationTimer++;
		if (m_animationTimer >= C_MAX_TACKLE_TIMER)
		{
			m_tackleDirection = eDirection;
		}
	}
	else
	{
		m_pos.x += -m_tackleDirection.x * C_MAX_TACKLE_SPEED;
		m_pos.z += -m_tackleDirection.z * C_MAX_TACKLE_SPEED;

		float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
		//突進中
		if (!m_isTackleRange)
		{
			//範囲外(追従)
			if (l_dist > C_CALC_TACKLE_RANGE)
			{
				m_deg = atan2(eDirection.x, eDirection.z);
				OgaJHelper::ConvertToDegree(m_deg);
				m_tackleDirection = eDirection;
			}
			//範囲内
			else
			{
				m_isCalc = true;

				m_isTackleRange = true;
				m_tackleDirection = eDirection;
			}
		}

		//突進後,範囲外
		if (m_isTackleRange && l_dist > C_MAX_TACKLE_RANGE)
		{
			m_tackleCount++;
			m_animationTimer = 0;
			m_isTackleRange = false;

			//何回突進するか
			if (m_tackleCount >= C_MAX_TACKLE_COUNT)
			{
				m_isTackleEnd = true;
			}
		}

		//判定外
		if (l_dist > C_CALC_TACKLE_RANGE)
		{
			m_isCalc = false;
		}
	}
}

void Enemy::CalcExplosion()
{
	//パーティクル(解放前)
	const int l_randPosRange = 501;
	const float l_halfRandPosRange = l_randPosRange * 0.5f;	//-この値から+この値までが範囲になる。

	const int l_createAddFrame = 15;						//何フレーム毎に生成される個数が増えるか。
	const float l_emitPosY = 100.0f;						//吸収される高さ
	const float l_power = 1.0f;
	const float l_addPower = 1.1f;
	const float l_scale = 2.0f;
	const float l_colorR = 0.9f;
	const float l_colorG = 0.9f;
	const float l_colorB = 0.2f;

	//パーティクル(解放後)
	const int l_randPowerRange = 81;						//baseに加算される値の範囲,/10.0fされて加算
	const int l_randScaleRange = 101;						//baseに加算される値の範囲,/10.0fされて加算
	const int l_randColorRangeR = 21;						//baseに加算される値の範囲,/100.0fされて加算
	const int l_randColorRangeG = 51;						//baseに加算される値の範囲,/100.0fされて加算
	const float l_basePower = 3.0f;							//最低値
	const float l_baseScale = 5.0f;							//最低値
	const float l_baseColorR = 0.7f;						//最低値
	const float l_baseColorG = 0.4f;						//最低値
	const float l_baseColorB = 0.2f;						//最低値

	for (int i = 0; i < pManager_Ex.GetMaxParticle(); i++)
	{
		if (!m_isExplosion)
		{
			if (!pManager_Ex.IsMove(i))
			{
				DirectX::XMFLOAT3 l_pos = m_pos + DirectX::XMFLOAT3(
					rand() % l_randPosRange - l_halfRandPosRange,
					rand() % l_randPosRange - l_halfRandPosRange,
					rand() % l_randPosRange - l_halfRandPosRange);
				DirectX::XMFLOAT3 l_pos2 = m_pos;
				l_pos2.y = l_emitPosY;
				DirectX::XMFLOAT3 l_vec =
					OgaJHelper::CalcNormalizeVec3(OgaJHelper::CalcDirectionVec3(l_pos, l_pos2));

				pManager_Ex.SetPosition(i, l_pos);
				pManager_Ex.SetVec(i, l_vec);
				pManager_Ex.SetPower(i, DirectX::XMFLOAT3(l_power, l_power, l_power));
				pManager_Ex.SetScale(i, DirectX::XMFLOAT3(l_scale, l_scale, l_scale));
				pManager_Ex.SetColor(i, DirectX::XMFLOAT4(l_colorR, l_colorG, l_colorB, 1.0f));
			}

			else
			{
				DirectX::XMFLOAT3 l_power = pManager_Ex.GetPower(i);
				l_power.x *= l_addPower;
				l_power.y *= l_addPower;
				l_power.z *= l_addPower;
				pManager_Ex.SetPower(i, l_power);
			}
		}
		else
		{
			pManager_Ex.SetIsCreateStop(true);

			if (pManager_Ex.IsMove(i))
			{
				if (pManager_Ex.GetNowPosition(i).y < 0)
				{
					pManager_Ex.SetNowScale(i, DirectX::XMFLOAT3(0, 0, 0));
				}
			}
		}
	}

	if (!m_isExplosion &&
		m_pPowerCount < C_EXPLOSION_COLLISION_TIMER + C_EXPLOSION_COLLISION_DELAY + C_EXPLOSION_COLLISION_ENDTIMER)
	{
		if (m_pPowerCount % l_createAddFrame == 0)
		{
			m_createCount++;
			pManager_Ex.SetCreateNum(m_createCount);
		}
		if (m_pPowerCount == C_EXPLOSION_COLLISION_TIMER)
		{
			pManager_Ex.SetIsCreateStop(true);
		}
		if (m_pPowerCount >= C_EXPLOSION_COLLISION_TIMER + C_EXPLOSION_COLLISION_DELAY)
		{
			m_isExplosion = true;
			pManager_Ex.SetCreateNum(pManager_Ex.GetMaxParticle());
			pManager_Ex.SetIsCreateStop(false);
			pManager_Ex.ResetLifeParticle();

			for (int i = 0; i < pManager_Ex.GetMaxParticle(); i++)
			{
				DirectX::XMFLOAT3 l_pos = m_pos + DirectX::XMFLOAT3(
					rand() % l_randPosRange - l_halfRandPosRange,
					rand() % l_randPosRange - l_halfRandPosRange + l_emitPosY,
					rand() % l_randPosRange - l_halfRandPosRange);
				DirectX::XMFLOAT3 l_pos2 = m_pos;
				l_pos2.y = l_emitPosY;
				DirectX::XMFLOAT3 l_vec =
					OgaJHelper::CalcNormalizeVec3(OgaJHelper::CalcDirectionVec3(l_pos2, l_pos));

				pManager_Ex.SetPosition(i, l_pos2);
				pManager_Ex.SetVec(i, l_vec);
				pManager_Ex.SetPower(i, DirectX::XMFLOAT3(
					l_basePower + (rand() % l_randPowerRange / 10.0f),
					l_basePower + (rand() % l_randPowerRange / 10.0f),
					l_basePower + (rand() % l_randPowerRange / 10.0f)));
				pManager_Ex.SetScale(i, DirectX::XMFLOAT3(
					l_baseScale + (rand() % l_randScaleRange / 10.0f),
					l_baseScale + (rand() % l_randScaleRange / 10.0f),
					l_baseScale + (rand() % l_randScaleRange / 10.0f)));
				pManager_Ex.SetColor(i, DirectX::XMFLOAT4(
					l_baseColorR + (rand() % l_randColorRangeR / 100.0f),
					l_baseColorG + (rand() % l_randColorRangeG / 100.0f),
					l_baseColorB, 1.0f));
			}
		}
	}
	else if (m_isExplosion)
	{
		if (m_pPowerCount >
			C_EXPLOSION_COLLISION_TIMER + C_EXPLOSION_COLLISION_DELAY + C_EXPLOSION_COLLISION_ENDTIMER)
		{
			m_isExplosion = false;
			pManager_Ex.SetIsCreateStop(true);
		}
	}
	m_pPowerCount++;
}

void Enemy::OtherUpdate(DirectX::XMFLOAT3& pPos)
{
	//FbxUpdate
	if (m_animationType == STAND)
	{
		fbxobj_creature[AnimationType::STAND]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::STAND]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::STAND]->Update();
	}
	else if (m_animationType == RUN)
	{
		m_isAttack = false;
		fbxobj_creature[AnimationType::RUN]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::RUN]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::RUN]->Update();
	}
	else if (m_animationType == KICK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_KICK_COLLISION_TIMER, C_KICK_COLLISION_ENDTIMER);
		}
		if (!m_isCalc && !m_isCalcEnd)
		{
			m_deg = CalcDeg(pPos);
		}

		fbxobj_creature[AnimationType::KICK]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::KICK]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::KICK]->Update();

		if (m_isCalc)
		{
			obj_Box[9]->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			obj_Box[9]->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (fbxobj_creature[AnimationType::KICK]->IsAnimationEnd())
		{
			m_isAttack = false;

			CalcNearAngle(pPos, fbxobj_creature[AnimationType::KICK]->GetRotation().y);
		}
	}
	else if (m_animationType == PUNCH)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_PUNCH_COLLISION_TIMER, C_PUNCH_COLLISION_ENDTIMER);
		}
		if (!m_isCalc && !m_isCalcEnd)
		{
			m_deg = CalcDeg(pPos);
		}

		fbxobj_creature[AnimationType::PUNCH]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::PUNCH]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::PUNCH]->Update();

		if (m_isCalc)
		{
			obj_Box[5]->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			obj_Box[5]->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (fbxobj_creature[AnimationType::PUNCH]->IsAnimationEnd())
		{
			m_isAttack = false;

			CalcNearAngle(pPos, fbxobj_creature[AnimationType::PUNCH]->GetRotation().y);
		}
	}
	else if (m_animationType == R_TURN)
	{
		fbxobj_creature[AnimationType::R_TURN]->Update();
	}
	else if (m_animationType == L_TURN)
	{
		fbxobj_creature[AnimationType::L_TURN]->Update();
	}
	else if (m_animationType == R_BACK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}
		fbxobj_creature[AnimationType::R_BACK]->Update();
	}
	else if (m_animationType == L_BACK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}
		fbxobj_creature[AnimationType::L_BACK]->Update();
	}
	else if (m_animationType == EXPLOSION)
	{
		CalcExplosion();

		fbxobj_creature[AnimationType::EXPLOSION]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::EXPLOSION]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::EXPLOSION]->Update();

		if (fbxobj_creature[AnimationType::EXPLOSION]->IsAnimationEnd())
		{
			m_isAttack = false;

			CalcNearAngle(pPos, fbxobj_creature[AnimationType::EXPLOSION]->GetRotation().y);

			m_pPowerCount = 0;
			m_createCount = 1;
			m_isExplosion = false;
			pManager_Ex.SetIsCreateStop(true);
		}
	}
	else if (m_animationType == RISE)
	{
		CalcRise(pPos);
	}
	else if (m_animationType == SWING_DOWN)
	{
		CalcSwingDown(pPos);
	}
	else if (m_animationType == TACKLE)
	{
		CalcTackle(pPos);

		fbxobj_creature[AnimationType::TACKLE]->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature[AnimationType::TACKLE]->SetPosition(m_pos);
		fbxobj_creature[AnimationType::TACKLE]->Update();

		if (m_isTackleEnd)
		{
			m_isAttack = false;
			CalcNearAngle(pPos, fbxobj_creature[AnimationType::TACKLE]->GetRotation().y);
		}
	}

	pManager_Ex.Update();
}

void Enemy::SetImgui()
{
	//各アニメーションタイプ
	if (m_animationType == STAND)
	{
		ImguiControl::Imgui_enemyAniType = "STAND";
	}
	else if (m_animationType == RUN)
	{
		ImguiControl::Imgui_enemyAniType = "RUN";
	}
	else if (m_animationType == KICK)
	{
		ImguiControl::Imgui_enemyAniType = "KICK";
	}
	else if (m_animationType == PUNCH)
	{
		ImguiControl::Imgui_enemyAniType = "PUNCH";
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
		ImguiControl::Imgui_enemyAniType = "R_BACK";
	}
	else if (m_animationType == L_BACK)
	{
		ImguiControl::Imgui_enemyAniType = "L_BACK";
	}
	else if (m_animationType == EXPLOSION)
	{
		ImguiControl::Imgui_enemyAniType = "EXPLOSION";
	}
	else if (m_animationType == RISE)
	{
		ImguiControl::Imgui_enemyAniType = "RISE";
	}
	else if (m_animationType == SWING_DOWN)
	{
		ImguiControl::Imgui_enemyAniType = "SWING_DOWN";
	}
	else if (m_animationType == TACKLE)
	{
		ImguiControl::Imgui_enemyAniType = "TACKLE";
	}

	if (m_oldAnimationType == STAND)
	{
		ImguiControl::Imgui_enemyOldAniType = "STAND";
	}
	else if (m_oldAnimationType == RUN)
	{
		ImguiControl::Imgui_enemyOldAniType = "RUN";
	}
	else if (m_oldAnimationType == KICK)
	{
		ImguiControl::Imgui_enemyOldAniType = "KICK";
	}
	else if (m_oldAnimationType == PUNCH)
	{
		ImguiControl::Imgui_enemyOldAniType = "PUNCH";
	}
	else if (m_oldAnimationType == R_TURN)
	{
		ImguiControl::Imgui_enemyOldAniType = "R_TURN";
	}
	else if (m_oldAnimationType == L_TURN)
	{
		ImguiControl::Imgui_enemyOldAniType = "L_TURN";
	}
	else if (m_oldAnimationType == R_BACK)
	{
		ImguiControl::Imgui_enemyOldAniType = "R_BACK";
	}
	else if (m_oldAnimationType == L_BACK)
	{
		ImguiControl::Imgui_enemyOldAniType = "L_BACK";
	}
	else if (m_oldAnimationType == EXPLOSION)
	{
		ImguiControl::Imgui_enemyOldAniType = "EXPLOSION";
	}
	else if (m_oldAnimationType == RISE)
	{
		ImguiControl::Imgui_enemyOldAniType = "RISE";
	}
	else if (m_oldAnimationType == SWING_DOWN)
	{
		ImguiControl::Imgui_enemyOldAniType = "SWING_DOWN";
	}
	else if (m_oldAnimationType == TACKLE)
	{
		ImguiControl::Imgui_enemyOldAniType = "TACKLE";
	}

	//無敵かどうか
	if (m_isInvincible)
	{
		ImguiControl::Imgui_enemyInvi = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_enemyInvi = "FALSE";
	}

	ImguiControl::Imgui_enemyBlendTimer = m_blendTimer;
	ImguiControl::Imgui_enemyCurrentAniTimer = fbxobj_creature[m_animationType]->GetNowTime();
	ImguiControl::Imgui_enemyOldAniTimer = fbxobj_creature[m_oldAnimationType]->GetNowTime();
}

void Enemy::CalcBlendAnimation()
{
	//切り替わったら
	if (!m_isChange &&
		fbxobj_creature[m_animationType] != fbxobj_creature[m_oldAnimationType])
	{
		m_blendTimer = 0.0f;
		m_isChange = true;
		m_keepAnimationType = m_animationType;
		fbxobj_creature[m_animationType]->ResetAnimation();
	}

	//補間計算
	else
	{
		//途中切り替え時
		if (m_animationType != m_keepAnimationType)
		{
			m_blendTimer = 0.0f;
			m_keepAnimationType = m_animationType;
			fbxobj_creature[m_animationType]->ResetAnimation();
		}

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

	//BlendAnimation
	fbxobj_creature[m_animationType]->
		BlendAnimation(fbxobj_creature[m_oldAnimationType], OgaJEase::easeOutCubic(m_blendTimer), true);
}

float Enemy::CalcDeg(DirectX::XMFLOAT3& pos)
{
	DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(pos, m_pos);
	eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
	float l_deg = atan2(eDirection.x, eDirection.z);
	OgaJHelper::ConvertToDegree(l_deg);
	return l_deg;
}

/*----------呼ぶやつ----------*/
void Enemy::HitAttack(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0) { m_hp = 0; }
	m_isInvincible = true;

	DirectX::XMFLOAT3 l_pos = m_obbs[m_hitOBBNum].pos;
	for (int i = 0; i < pManager_Hit.GetMaxParticle(); i++)
	{
		//生成時のみ剣に付随
		if (!pManager_Hit.IsMove(i))
		{
			pManager_Hit.SetPosition(i, l_pos);
		}
	}
	pManager_Hit.SetIsCreateStop(false);

	OutputDebugStringA("Hit!\n");
}

std::vector<OBB> Enemy::GetAttackOBBs()
{
	std::vector<OBB> l_obb;
	if (m_animationType == KICK)
	{
		l_obb = { m_obbs[9] };
		return l_obb;
	}
	else if (m_animationType == PUNCH)
	{
		l_obb = { m_obbs[3] };
		return l_obb;
	}
	else if (m_animationType == R_BACK ||
		m_animationType == L_BACK)
	{
		l_obb = { m_obbs[3],m_obbs[5] };
		return l_obb;
	}
	else
	{
		return m_obbs;
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
