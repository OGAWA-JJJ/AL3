#pragma once
#include "../3D/FbxObject3D.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

class Enemy
{
private:	//モーション
	enum AnimationType
	{
		STAND, RUN, KICK, PUNCH
	};

private:	//定数
	const int C_ATTACK_COLLISION_TIMER = 20;		//攻撃判定を取り出すフレーム
	const int C_MAX_POWER = 100;
	const float C_MAX_MOVE_SPEED = 2.0f;	//移動速度
	const float C_MAX_DIST = 35.0f;

private:	//定数(ステータス関係)
	const int C_MAX_HP = 1000;

private:
	std::vector<OBB> m_obbs;
	DirectX::XMFLOAT3 m_pos;
	int m_animationTimer;
	int m_animationType;
	int m_boneCount;
	float m_deg;
	bool m_isInvincible;
	bool m_isAttack;
	bool m_isAttackTrigger;
	bool m_isCalc;

	//仮
	bool m_endKick = false;
	bool m_endPunch = false;

private:	//変数(ステータス関係)
	int m_hp;

private:	//オブジェクト(Draw用)
	Object* obj_Box[37] = { nullptr };

	FbxObject3D* fbxobj_idleCreature = nullptr;
	FbxObject3D* fbxobj_runCreature = nullptr;
	FbxObject3D* fbxobj_kickCreature = nullptr;
	FbxObject3D* fbxobj_punchCreature = nullptr;
	FbxObject3D* fbxobj_dieCreature = nullptr;

public:
	Enemy();
	~Enemy();
	void Init();
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();

private:
	void CalcOBB();
	void JudgAnimationType();

public:	//Getter,Setter
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int& GetBoneCount() { return m_boneCount; }
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	void UnInvincible() { m_isInvincible = false; }
	bool IsInvincible() { return m_isInvincible; }
	inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	void DiscoverPlayer() { m_animationType = RUN; }
	bool IsAttack() { return m_isAttack; }
	const int& GetPower() { return C_MAX_POWER; }
	const bool GetIsCalc() { return m_isCalc; }

public:
	void HitAttack(int damage)
	{
		m_hp -= damage;
		m_isInvincible = true;
		if (m_hp < 0) { m_hp = 0; }
		OutputDebugStringA("Hit!\n");
	}
	const bool IsDead()
	{
		if (m_hp <= 0) { return true; }
		return false;
	}
	const OBB& GetAttackOBB()
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
	const bool IsFighting() 
	{
		if (m_animationType == STAND) { return false; }
		return true;
	}
};

