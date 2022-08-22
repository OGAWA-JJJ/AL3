#include "Enemy.h"
#include "../3D/FbxLoader.h"
#include "../DirectX/DirectXImportant.h"
#include "../Users/PipelineManager.h"
#include "../Math/OgaJHelper.h"
#include "ModelManager.h"
#include <random>

Enemy::Enemy()
{
	m_pos = { 0,0,0 };
	m_animationTimer = 0;
	m_animationType = STAND;
	m_boneCount = 0;
	m_deg = 180.0f;
	m_isInvincible = false;
	m_isAttack = false;
	m_isAttackTrigger = false;
	m_isCalc = false;

	m_hp = C_MAX_HP;

#pragma region ModelCreate
	for (int i = 0; i < 37; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
	}

	fbxobj_idleCreature = new FbxObject3D();
	fbxobj_idleCreature->Init();
	fbxobj_idleCreature->SetModel(ModelManager::fbxmodel_idleCreature);
	fbxobj_idleCreature->PlayAnimation();

	fbxobj_runCreature = new FbxObject3D();
	fbxobj_runCreature->Init();
	fbxobj_runCreature->SetModel(ModelManager::fbxmodel_runCreature);
	fbxobj_runCreature->PlayAnimation();

	fbxobj_kickCreature = new FbxObject3D();
	fbxobj_kickCreature->Init();
	fbxobj_kickCreature->SetModel(ModelManager::fbxmodel_kickCreature);
	fbxobj_kickCreature->PlayAnimation();

	fbxobj_punchCreature = new FbxObject3D();
	fbxobj_punchCreature->Init();
	fbxobj_punchCreature->SetModel(ModelManager::fbxmodel_punchCreature);
	fbxobj_punchCreature->PlayAnimation();

	fbxobj_dieCreature = new FbxObject3D();
	fbxobj_dieCreature->Init();
	fbxobj_dieCreature->SetModel(ModelManager::fbxmodel_dieCreature);
	fbxobj_dieCreature->PlayAnimation();
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
#pragma endregion
}

Enemy::~Enemy()
{
	delete fbxobj_runCreature;
	delete fbxobj_kickCreature;
	delete fbxobj_punchCreature;
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
		if (fbxobj_dieCreature->GetNowTime() == fbxobj_dieCreature->GetEndTime())
		{
			fbxobj_dieCreature->StopAnimation();
		}

		fbxobj_dieCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_dieCreature->SetPosition(m_pos);
		fbxobj_dieCreature->Update();
		return;
	}

	if (!m_animationType == STAND)
	{
		//攻撃中じゃない
		if (!m_isAttack)
		{
			//完全追従になってる
			DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(playerPos, m_pos);
			eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
			m_deg = atan2(eDirection.x, eDirection.z);
			OgaJHelper::ConvertToDegree(m_deg);

			if (OgaJHelper::CalcDist(m_pos, playerPos) >= C_MAX_DIST)
			{
				m_pos.x += -eDirection.x * C_MAX_MOVE_SPEED;
				m_pos.z += -eDirection.z * C_MAX_MOVE_SPEED;
			}
			else
			{
				JudgAnimationType();
				m_isAttack = true;
				m_isAttackTrigger = true;

				m_isCalc = false;
			}
		}
	}

	//ボーン用配列
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> l_affine;
	std::vector<DirectX::XMMATRIX> l_matRot;

	//待機中にしろ！
	if (m_animationType == STAND)
	{
		fbxobj_idleCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_idleCreature->SetPosition(m_pos);
		fbxobj_idleCreature->Update();
		l_affine = fbxobj_idleCreature->GetAffineTrans();
		l_matRot = fbxobj_idleCreature->GetMatRots();
	}
	else if (m_animationType == RUN)
	{
		fbxobj_runCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_runCreature->SetPosition(m_pos);
		fbxobj_runCreature->Update();
		l_affine = fbxobj_runCreature->GetAffineTrans();
		l_matRot = fbxobj_runCreature->GetMatRots();
	}
	else if (m_animationType == KICK)
	{
		if (m_isAttackTrigger)
		{
			fbxobj_kickCreature->ResetAnimation();
			m_isAttackTrigger = false;
		}

		//仮
		if (m_animationTimer < C_ATTACK_COLLISION_TIMER)
		{
			m_animationTimer++;
		}
		else
		{
			m_isCalc = true;
		}

		fbxobj_kickCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_kickCreature->SetPosition(m_pos);
		fbxobj_kickCreature->Update();
		l_affine = fbxobj_kickCreature->GetAffineTrans();
		l_matRot = fbxobj_kickCreature->GetMatRots();

		if (fbxobj_kickCreature->GetNowTime() == fbxobj_kickCreature->GetEndTime())
		{
			fbxobj_runCreature->SetPosition(m_pos);
			fbxobj_runCreature->Update();

			//m_animationType = RUN;
			m_endKick = true;
			m_isAttack = false;

			m_isCalc = false;
		}
	}
	else if (m_animationType == PUNCH)
	{
		if (m_isAttackTrigger)
		{
			fbxobj_punchCreature->ResetAnimation();
			m_isAttackTrigger = false;
		}

		//仮
		if (m_animationTimer < C_ATTACK_COLLISION_TIMER)
		{
			m_animationTimer++;
		}
		else
		{
			m_isCalc = true;
		}

		fbxobj_punchCreature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_punchCreature->SetPosition(m_pos);
		fbxobj_punchCreature->Update();
		l_affine = fbxobj_punchCreature->GetAffineTrans();
		l_matRot = fbxobj_punchCreature->GetMatRots();

		if (fbxobj_punchCreature->GetNowTime() == fbxobj_punchCreature->GetEndTime())
		{
			fbxobj_runCreature->SetPosition(m_pos);
			fbxobj_runCreature->Update();

			//m_animationType = RUN;
			m_endPunch = true;
			m_isAttack = false;

			m_isCalc = false;
		}
	}

	m_boneCount = l_affine.size();

	//箱サイズ
	const float l_scale = 12.0f;
	std::vector<OBB> l_obbs;
	for (int i = 0; i < m_boneCount; i++)
	{
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(l_scale, l_scale, l_scale));
		obj_Box[i]->MultiMatrix(l_affine[i].second);
		obj_Box[i]->Update();

		OBB l_obb;
		l_obb.pos = DirectX::XMFLOAT3(
			l_affine[i].second.r[3].m128_f32[0],
			l_affine[i].second.r[3].m128_f32[1],
			l_affine[i].second.r[3].m128_f32[2]);
		l_obb.matRot = l_matRot[i];
		l_obb.length = DirectX::XMFLOAT3(l_scale, l_scale, l_scale);

		l_obbs.push_back(l_obb);
	}
	m_obbs = l_obbs;
}

void Enemy::Draw()
{
	if (IsDead())
	{
		fbxobj_dieCreature->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		return;
	}

	if (m_animationType == STAND)
	{
		fbxobj_idleCreature->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == RUN)
	{
		fbxobj_runCreature->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
	}
	else if (m_animationType == KICK)
	{
		fbxobj_kickCreature->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		if (m_endKick)
		{
			m_animationType = RUN;
			m_endKick = false;
		}
	}
	else if (m_animationType == PUNCH)
	{
		fbxobj_punchCreature->Draw(DirectXImportant::cmdList.Get(), PipelineManager::fbx_normal);
		if (m_endPunch)
		{
			m_animationType = RUN;
			m_endPunch = false;
		}
	}

	Object::PreDraw(DirectXImportant::cmdList.Get());
	for (int i = 0; i < m_boneCount; i++)
	{
		obj_Box[i]->Draw(PipelineManager::obj_normal);
	}
	Object::PostDraw();
}

void Enemy::CalcOBB()
{

}

void Enemy::JudgAnimationType()
{
	if (std::rand() % 2 == 0) { m_animationType = KICK; }
	else { m_animationType = PUNCH; }
}
