#pragma once
#include "../3D/FbxObject3D.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

class Enemy
{
private:	//���[�V����
	enum AnimationType
	{
		STAND, RUN, KICK, PUNCH
	};

private:	//�萔(����n)
	const int C_KICK_COLLISION_TIMER = 30;		//�U����������o���t���[��
	const int C_KICK_COLLISION_ENDTIMER = 20;	//�U�������A������������t���[��(�U���ɂ��C������)

	const int C_PUNCH_COLLISION_TIMER = 15;		//�U����������o���t���[��
	const int C_PUNCH_COLLISION_ENDTIMER = 30;	//�U�������A������������t���[��

private:	//�萔
	const float C_MAX_DIST = 35.0f;

private:	//�萔(�X�e�[�^�X�֌W)
	const int C_MAX_POWER = 100;
	const int C_MAX_HP = 1000;
	const float C_MAX_MOVE_SPEED = 2.0f;		//�ړ����x

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

	//��
	bool m_endKick = false;
	bool m_endPunch = false;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;

private:	//�I�u�W�F�N�g(Draw�p)
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

public:	//�ĂԂ��
	void HitAttack(int damage);
	OBB& GetAttackOBB();
	bool IsDead();
	bool IsFighting();
};

