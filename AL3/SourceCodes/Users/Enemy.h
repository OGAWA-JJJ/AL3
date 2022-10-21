#pragma once
#include "../3D/FbxObject3D.h"
#include "../3D/FbxObjects.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

class Enemy
{
private:	//モーション
	enum AnimationType
	{
		STAND, RUN, KICK, PUNCH
	};

private:	//定数(判定系)
	const int C_KICK_COLLISION_TIMER = 30;		//攻撃判定を取り出すフレーム
	const int C_KICK_COLLISION_ENDTIMER = 20;	//攻撃判定後、判定を取り消すフレーム(攻撃による気もする)

	const int C_PUNCH_COLLISION_TIMER = 15;		//攻撃判定を取り出すフレーム
	const int C_PUNCH_COLLISION_ENDTIMER = 30;	//攻撃判定後、判定を取り消すフレーム

private:	//定数
	const float C_MAX_DIST = 35.0f;

private:	//定数(ステータス関係)
	const int C_MAX_POWER = 100;
	const int C_MAX_HP = 1000;
	const float C_MAX_MOVE_SPEED = 2.0f;		//移動速度

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
	bool m_isCalcEnd;

	//仮
	bool m_endKick = false;
	bool m_endPunch = false;

private:	//変数(ステータス関係)
	int m_hp;

private:	//オブジェクト(Draw用)
	Object* obj_Box[37] = { nullptr };

	FbxObjects* fbxobj_idleCreature = nullptr;
	FbxObjects* fbxobj_runCreature = nullptr;
	FbxObjects* fbxobj_kickCreature = nullptr;
	FbxObjects* fbxobj_punchCreature = nullptr;
	FbxObjects* fbxobj_dieCreature = nullptr;

public:
	Enemy();
	~Enemy();

	void Init();
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();

private:
	void CalcOBB();
	void JudgAnimationType();

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int& GetBoneCount() { return m_boneCount; }
	const int& GetPower() { return C_MAX_POWER; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const bool IsInvincible() { return m_isInvincible; }
	const bool IsAttack() { return m_isAttack; }
	const bool GetIsCalc() { return m_isCalc; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }
	void DiscoverPlayer() { m_animationType = RUN; }

public:	//呼ぶやつ
	void HitAttack(int damage);
	OBB& GetAttackOBB();
	bool IsDead();
	bool IsFighting();
};

