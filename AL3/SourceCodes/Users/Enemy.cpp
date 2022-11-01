#include "Enemy.h"
#include "../3D/FbxLoader.h"
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
	m_animationTimer = 0;
	m_animationType = STAND;
	m_boneCount = 0;
	m_deg = 180.0f;
	m_turnEaseTimer = 0.0f;
	m_turnStartAngle = 0.0f;
	m_turnEndAngle = 0.0f;
	m_dist = 0.0f;
	m_blendTimer = 0.0f;
	m_easeBlendTimer = 0.0f;
	m_isInvincible = false;
	m_isAttack = false;
	m_isAttackTrigger = false;
	m_isCalc = false;
	m_isCalcEnd = false;
	m_isTurn = false;
	m_turnVec = false;
	m_isBackAttackLottery = false;
	m_isBackAttack = false;

	m_hp = C_MAX_HP;
#pragma endregion

#pragma region ModelCreate
	for (int i = 0; i < 37; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
	}

	fbxobj_idleCreature = FbxObjects::Create(ModelManager::fbxmodel_idleCreature);
	fbxobj_idleCreature->PlayAnimation();

	fbxobj_currentCreature = fbxobj_idleCreature;

	fbxobj_runCreature = FbxObjects::Create(ModelManager::fbxmodel_runCreature);
	fbxobj_runCreature->PlayAnimation();

	fbxobj_kickCreature = FbxObjects::Create(ModelManager::fbxmodel_kickCreature);
	fbxobj_kickCreature->PlayAnimation();

	fbxobj_punchCreature = FbxObjects::Create(ModelManager::fbxmodel_punchCreature);
	fbxobj_punchCreature->PlayAnimation();

	fbxobj_dieCreature = FbxObjects::Create(ModelManager::fbxmodel_dieCreature);
	fbxobj_dieCreature->PlayAnimation();

	fbxobj_RTurnCreature = FbxObjects::Create(ModelManager::fbxmodel_RTurnCreature);
	fbxobj_RTurnCreature->PlayAnimation();

	fbxobj_LTurnCreature = FbxObjects::Create(ModelManager::fbxmodel_LTurnCreature);
	fbxobj_LTurnCreature->PlayAnimation();

	fbxobj_RBackCreature = FbxObjects::Create(ModelManager::fbxmodel_RBackCreature);
	fbxobj_RBackCreature->PlayAnimation();

	fbxobj_LBackCreature = FbxObjects::Create(ModelManager::fbxmodel_LBackCreature);
	fbxobj_LBackCreature->PlayAnimation();

	fbxobj_explosionCreature = FbxObjects::Create(ModelManager::fbxmodel_explosionCreature);
	fbxobj_explosionCreature->PlayAnimation();

	fbxobj_riseCreature = FbxObjects::Create(ModelManager::fbxmodel_riseCreature);
	fbxobj_riseCreature->PlayAnimation();

	fbxobj_swingDownCreature = FbxObjects::Create(ModelManager::fbxmodel_swingDownCreature);
	fbxobj_swingDownCreature->PlayAnimation();
#pragma endregion

#pragma region ModelInit
	const float Creature_Scale = 0.4f;

	fbxobj_idleCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_runCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_kickCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_punchCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_dieCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_RTurnCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_LTurnCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_RBackCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_LBackCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_explosionCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_riseCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));

	fbxobj_swingDownCreature->SetScale(DirectX::XMFLOAT3(
		Creature_Scale,
		Creature_Scale,
		Creature_Scale));
#pragma endregion
}

Enemy::~Enemy()
{
	delete obj_Box;

	delete fbxobj_idleCreature;
	delete	fbxobj_runCreature;
	delete	fbxobj_kickCreature;
	delete	fbxobj_punchCreature;
	delete	fbxobj_dieCreature;

	delete	fbxobj_RTurnCreature;
	delete	fbxobj_LTurnCreature;
	delete	fbxobj_RBackCreature;
	delete	fbxobj_LBackCreature;
	delete	fbxobj_explosionCreature;
	delete	fbxobj_riseCreature;
	delete	fbxobj_swingDownCreature;
}

void Enemy::Init()
{
	//obj_Lich->SetPosition(XMFLOAT3(0.0f, 35.0f, -400.0f));
	//obj_Lich->SetRotation(XMFLOAT3(0, -100, 0));

	m_pos = DirectX::XMFLOAT3(0.0f, 0.0f, -300.0f);
}

void Enemy::Update(DirectX::XMFLOAT3 playerPos)
{
	if (IsDead())
	{
		if (fbxobj_dieCreature->IsAnimationEnd())
		{
			fbxobj_dieCreature->StopAnimation();
		}

		fbxobj_dieCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_dieCreature->SetPosition(m_pos);
		fbxobj_dieCreature->Update();
		return;
	}

	if (m_animationType != STAND)
	{
		//非攻撃中かつ振り向き中じゃない
		if (!m_isAttack)
		{
			float l_dist = OgaJHelper::CalcDist(m_pos, playerPos);
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

	//ボーン用配列
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> l_affine;
	std::vector<DirectX::XMMATRIX> l_matRot;

	//BindAnimation
	if (fbxobj_oldCreature != nullptr && m_isChange)
	{
		fbxobj_currentCreature->BlendAnimation(fbxobj_oldCreature, m_easeBlendTimer, m_isChange);
	}
	else
	{
		fbxobj_currentCreature->BlendAnimation(fbxobj_oldCreature, m_easeBlendTimer, false);
	}

	//過去のデータを保存
	if (!m_isChange)
	{
		fbxobj_oldCreature = fbxobj_currentCreature;
	}

	//補間計算
	else
	{
		if (m_blendTimer < 1.0f)
		{
			m_blendTimer += C_MAX_BLEND_TIMER;
			m_easeBlendTimer = OgaJEase::easeOutCubic(m_blendTimer);
			//fbxobj_oldCreature->Update();
			if (m_blendTimer > 1.0f)
			{
				m_blendTimer = 1.0f;
				m_isChange = false;
				fbxobj_oldCreature = fbxobj_currentCreature;
			}
		}
		if (m_blendTimer > 1.0f)
		{
			m_isChange = false;
			fbxobj_oldCreature = fbxobj_currentCreature;
		}
	}

	//FbxUpdate
	if (m_animationType == STAND)
	{
		fbxobj_idleCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_idleCreature->SetPosition(m_pos);
		fbxobj_idleCreature->Update();
		l_affine = fbxobj_idleCreature->GetAffineTrans();
		l_matRot = fbxobj_idleCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "STAND";
		fbxobj_currentCreature = fbxobj_idleCreature;
	}
	else if (m_animationType == RUN)
	{
		m_isAttack = false;
		fbxobj_runCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_runCreature->SetPosition(m_pos);
		fbxobj_runCreature->Update();
		l_affine = fbxobj_runCreature->GetAffineTrans();
		l_matRot = fbxobj_runCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "RUN";
		fbxobj_currentCreature = fbxobj_runCreature;
	}
	else if (m_animationType == KICK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_KICK_COLLISION_TIMER, C_KICK_COLLISION_ENDTIMER);
		}

		fbxobj_kickCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_kickCreature->SetPosition(m_pos);
		fbxobj_kickCreature->Update();
		l_affine = fbxobj_kickCreature->GetAffineTrans();
		l_matRot = fbxobj_kickCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "KICK";
		fbxobj_currentCreature = fbxobj_kickCreature;

		if (fbxobj_kickCreature->IsAnimationEnd())
		{
			fbxobj_runCreature->SetPosition(m_pos);
			fbxobj_runCreature->Update();

			m_isAttack = false;

			CalcNearAngle(playerPos, fbxobj_kickCreature->GetRotation().y);
		}
	}
	else if (m_animationType == PUNCH)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_PUNCH_COLLISION_TIMER, C_PUNCH_COLLISION_ENDTIMER);
		}

		fbxobj_punchCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_punchCreature->SetPosition(m_pos);
		fbxobj_punchCreature->Update();
		l_affine = fbxobj_punchCreature->GetAffineTrans();
		l_matRot = fbxobj_punchCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "PUNCH";
		fbxobj_currentCreature = fbxobj_punchCreature;

		if (fbxobj_punchCreature->IsAnimationEnd())
		{
			fbxobj_runCreature->SetPosition(m_pos);
			fbxobj_runCreature->Update();

			m_isAttack = false;

			CalcNearAngle(playerPos, fbxobj_punchCreature->GetRotation().y);
		}
	}
	else if (m_animationType == R_TURN)
	{
		l_affine = fbxobj_RTurnCreature->GetAffineTrans();
		l_matRot = fbxobj_RTurnCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "R_TURN";
		fbxobj_currentCreature = fbxobj_RTurnCreature;
	}
	else if (m_animationType == L_TURN)
	{
		l_affine = fbxobj_LTurnCreature->GetAffineTrans();
		l_matRot = fbxobj_LTurnCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "L_TURN";
		fbxobj_currentCreature = fbxobj_LTurnCreature;
	}
	else if (m_animationType == R_BACK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}

		l_affine = fbxobj_RBackCreature->GetAffineTrans();
		l_matRot = fbxobj_RBackCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "R_BACK";
		fbxobj_currentCreature = fbxobj_RBackCreature;
	}
	else if (m_animationType == L_BACK)
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}

		l_affine = fbxobj_LBackCreature->GetAffineTrans();
		l_matRot = fbxobj_LBackCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "L_BACK";
		fbxobj_currentCreature = fbxobj_LBackCreature;
	}
	else if (m_animationType == EXPLOSION)
	{
		fbxobj_explosionCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_explosionCreature->SetPosition(m_pos);
		fbxobj_explosionCreature->Update();

		l_affine = fbxobj_explosionCreature->GetAffineTrans();
		l_matRot = fbxobj_explosionCreature->GetMatRots();

		ImguiControl::Imgui_AniType = "EXPLOSION";
		fbxobj_currentCreature = fbxobj_explosionCreature;

		if (fbxobj_explosionCreature->IsAnimationEnd())
		{
			fbxobj_runCreature->SetPosition(m_pos);
			fbxobj_runCreature->Update();

			m_isAttack = false;

			CalcNearAngle(playerPos, fbxobj_explosionCreature->GetRotation().y);
		}
	}

	//切り替わったら
	if (!m_isChange && fbxobj_oldCreature != fbxobj_currentCreature)
	{
		m_blendTimer = 0.0f;
		m_isChange = true;
	}

	//箱サイズ
	std::vector<OBB> l_obbs;
	m_boneCount = l_affine.size();
	for (int i = 0; i < m_boneCount; i++)
	{
		//obj_Box[i]->SetScale(DirectX::XMFLOAT3(l_scale, l_scale, l_scale));
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

void Enemy::Draw()
{
	if (IsDead())
	{
		fbxobj_dieCreature->Draw(PipelineManager::fbx_normal);
		return;
	}

	if (ImguiControl::Imgui_isEnemyDraw)
	{
		if (m_animationType == STAND)
		{
			fbxobj_idleCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == RUN)
		{
			fbxobj_runCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == KICK)
		{
			fbxobj_kickCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == PUNCH)
		{
			fbxobj_punchCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == R_TURN)
		{
			fbxobj_RTurnCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == L_TURN)
		{
			fbxobj_LTurnCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == R_BACK)
		{
			fbxobj_RBackCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == L_BACK)
		{
			fbxobj_LBackCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == EXPLOSION)
		{
			fbxobj_explosionCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == RISE)
		{
			fbxobj_riseCreature->Draw(PipelineManager::fbx_normal);
		}
		else if (m_animationType == SWING_DOWN)
		{
			fbxobj_swingDownCreature->Draw(PipelineManager::fbx_normal);
		}
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	if (ImguiControl::Imgui_isOBBDraw)
	{
		for (int i = 0; i < m_boneCount; i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_normal);
		}
	}
	Object::PostDraw();
}

void Enemy::CalcOBB()
{

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
				fbxobj_RBackCreature->SetPosition(m_pos);
			}
			else
			{
				m_animationType = L_BACK;
				fbxobj_LBackCreature->SetPosition(m_pos);
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
		return;
	}
	else
	{
		m_isAttack = true;
		m_isCalc = false;
		m_isCalcEnd = false;

		l_rand = l_rand % l_div;
		if (l_rand == l_div - 1 &&
			dist < C_MAX_DIST)
		{
			m_animationType = EXPLOSION;
			fbxobj_explosionCreature->ResetAnimation();
		}

		else if (l_rand % 2 == 0)
		{
			m_animationType = KICK;
			fbxobj_kickCreature->ResetAnimation();
		}
		else
		{
			m_animationType = PUNCH;
			fbxobj_punchCreature->ResetAnimation();
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
			l_rotY = (m_turnEndAngle - m_turnStartAngle) * OgaJEase::easeOutSine(m_turnEaseTimer) + m_turnStartAngle;
			fbxobj_RTurnCreature->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
			fbxobj_RTurnCreature->Update();
		}
		else
		{
			l_rotY = (m_turnEndAngle - m_turnStartAngle) * OgaJEase::easeOutSine(m_turnEaseTimer) + m_turnStartAngle;
			fbxobj_LTurnCreature->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
			fbxobj_LTurnCreature->Update();
		}

		if (m_turnEaseTimer < 1.0f)
		{
			m_turnEaseTimer += C_MAX_TURN_TIMER;
		}
		else
		{
			m_deg = l_rotY - 180.0f;
			if (m_turnVec == RIGHT)
			{
				if (fbxobj_RTurnCreature->IsAnimationEnd())
				{
					fbxobj_RTurnCreature->ResetAnimation();
					m_turnEaseTimer = 0.0f;
					m_isTurn = false;

					if (OgaJHelper::CalcDist(m_pos, pPos) >= C_MAX_DIST)
					{
						fbxobj_runCreature->SetPosition(m_pos);
						fbxobj_runCreature->Update();
						m_animationType = RUN;
					}
				}
			}
			else
			{
				if (fbxobj_LTurnCreature->IsAnimationEnd())
				{
					fbxobj_LTurnCreature->ResetAnimation();
					m_turnEaseTimer = 0.0f;
					m_isTurn = false;

					if (OgaJHelper::CalcDist(m_pos, pPos) >= C_MAX_DIST)
					{
						fbxobj_runCreature->SetPosition(m_pos);
						fbxobj_runCreature->Update();
						m_animationType = RUN;
					}
				}
			}
		}
	}
	else if (m_isBackAttack)
	{
		if (m_turnVec == RIGHT)
		{
			if (m_turnEaseTimer < 1.0f)
			{
				float l_rotY = (m_turnEndAngle - m_turnStartAngle) *
					OgaJEase::easeOutSine(m_turnEaseTimer) + m_turnStartAngle;
				fbxobj_RBackCreature->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
				m_turnEaseTimer += C_MAX_TURN_TIMER;
			}

			fbxobj_RBackCreature->Update();
		}
		else
		{
			if (m_turnEaseTimer < 1.0f)
			{
				float l_rotY = (m_turnEndAngle - m_turnStartAngle) *
					OgaJEase::easeOutSine(m_turnEaseTimer) + m_turnStartAngle;
				fbxobj_LBackCreature->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
				m_turnEaseTimer += C_MAX_TURN_TIMER;
			}

			fbxobj_LBackCreature->Update();
		}

		if (m_turnEaseTimer >= 1.0f)
		{
			m_deg = m_turnEndAngle - 180.0f;
			if (m_turnVec == RIGHT)
			{
				if (fbxobj_RBackCreature->IsAnimationEnd())
				{
					fbxobj_RBackCreature->ResetAnimation();
					m_turnEaseTimer = 0.0f;
					m_isBackAttack = false;
					m_isAttack = false;

					CalcNearAngle(pPos, fbxobj_RBackCreature->GetRotation().y);
					m_isBackAttackLottery = false;
					float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
					JudgAnimationType(l_dist);

					if (l_dist > C_MAX_DIST)
					{
						fbxobj_runCreature->SetPosition(m_pos);
						fbxobj_runCreature->Update();
						m_animationType = RUN;
					}
				}
			}
			else
			{
				if (fbxobj_LBackCreature->IsAnimationEnd())
				{
					fbxobj_LBackCreature->ResetAnimation();
					m_turnEaseTimer = 0.0f;
					m_isBackAttack = false;
					m_isAttack = false;

					CalcNearAngle(pPos, fbxobj_LBackCreature->GetRotation().y);
					m_isBackAttackLottery = false;
					float l_dist = OgaJHelper::CalcDist(m_pos, pPos);
					JudgAnimationType(l_dist);

					if (l_dist > C_MAX_DIST)
					{
						fbxobj_runCreature->SetPosition(m_pos);
						fbxobj_runCreature->Update();
						m_animationType = RUN;
					}
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
			fbxobj_RTurnCreature->SetRotation(DirectX::XMFLOAT3(0, myAngleY, 0));
			fbxobj_RTurnCreature->SetPosition(m_pos);
			fbxobj_RTurnCreature->Update();
		}
		else
		{
			m_turnVec = TurnVec::LEFT;
			fbxobj_LTurnCreature->SetRotation(DirectX::XMFLOAT3(0, myAngleY, 0));
			fbxobj_LTurnCreature->SetPosition(m_pos);
			fbxobj_LTurnCreature->Update();
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

void Enemy::CalcAttackCollisionTimer(const float startFrame, const float endFrame)
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

/*----------呼ぶやつ----------*/
void Enemy::HitAttack(int damage)
{
	m_hp -= damage;
	m_isInvincible = true;
	if (m_hp <= 0) { m_hp = 0; }
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
