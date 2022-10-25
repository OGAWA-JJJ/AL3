#pragma once
#include "../3D/FbxObject3D.h"
#include "../3D/FbxObjects.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

class Enemy
{
private:	//���[�V����
	enum AnimationType
	{
		STAND,		//�ҋ@
		RUN,		//�ړ�
		KICK,		//�R��U��
		PUNCH,		//����U��

		//�ǉ�
		R_TURN,		//�E�U�����
		L_TURN,		//���U�����
		R_BACK,		//�E��]���U��
		L_BACK,		//����]���U��

		//������
		EXPLOSION,	//����(����)
		RISE,		//�㏸
		SWING_DOWN,	//�U�艺�낵(�㏸�ƃZ�b�g)
	};

private:
	enum TurnVec
	{
		RIGHT = 0,
		LEFT = 1
	};

private:	//�萔(����n)
	const int C_KICK_COLLISION_TIMER = 30;		//�U����������o���t���[��
	const int C_KICK_COLLISION_ENDTIMER = 20;	//�U�������A������������t���[��

	const int C_PUNCH_COLLISION_TIMER = 15;		//�U����������o���t���[��
	const int C_PUNCH_COLLISION_ENDTIMER = 30;	//�U�������A������������t���[��

	const int C_BACK_COLLISION_TIMER = 20;		//�U����������o���t���[��
	const int C_BACK_COLLISION_ENDTIMER = 90;	//�U�������A������������t���[��

private:	//�萔
	const float C_MAX_DIST = 35.0f;
	const float C_MAX_TURN_RAD = 15.0f;
	const float C_MAX_BACK_RAD = 150.0f;

private:	//�萔(�X�e�[�^�X�֌W)
	const int C_MAX_POWER = 100;
	const int C_MAX_HP = 1000;
	const float C_MAX_MOVE_SPEED = 2.0f;		//�ړ����x
	const float C_MAX_TURN_TIMER = 0.01f;		//�U������C�[�W���O

private:
	std::vector<OBB> m_obbs;
	DirectX::XMFLOAT3 m_pos;
	int m_animationTimer;
	int m_animationType;
	int m_boneCount;
	float m_deg;
	float m_turnEaseTimer;
	float m_turnStartAngle;
	float m_turnEndAngle;
	float m_dist;
	bool m_isInvincible;
	bool m_isAttack;
	bool m_isAttackTrigger;
	bool m_isCalc;
	bool m_isCalcEnd;
	bool m_isTurn;
	bool m_turnVec;
	bool m_isBackAttackLottery;
	bool m_isBackAttack;

	//��
	bool m_endKick = false;
	bool m_endPunch = false;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;

private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_Box[37] = { nullptr };

	FbxObjects* fbxobj_idleCreature = nullptr;
	FbxObjects* fbxobj_runCreature = nullptr;
	FbxObjects* fbxobj_kickCreature = nullptr;
	FbxObjects* fbxobj_punchCreature = nullptr;
	FbxObjects* fbxobj_dieCreature = nullptr;

	FbxObjects* fbxobj_RTurnCreature = nullptr;
	FbxObjects* fbxobj_LTurnCreature = nullptr;
	FbxObjects* fbxobj_RBackCreature = nullptr;
	FbxObjects* fbxobj_LBackCreature = nullptr;
	FbxObjects* fbxobj_explosionCreature = nullptr;
	FbxObjects* fbxobj_riseCreature = nullptr;
	FbxObjects* fbxobj_swingDownCreature = nullptr;

public:
	Enemy();
	~Enemy();

	void Init();
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();

private:
	void CalcOBB();
	void JudgAnimationType(float dist);
	void CalcAngleDiff(DirectX::XMFLOAT3& pPos);
	void CalcNearAngle(DirectX::XMFLOAT3 pPos, float myAngleY);	//�U���I�����ɋ���
	void CalcAttackCollisionTimer(const float startFrame, const float endFrame);

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int& GetBoneCount() { return m_boneCount; }
	const int& GetPower() { return C_MAX_POWER; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const bool IsInvincible() { return m_isInvincible; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsCalc() { return m_isCalc; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }
	void DiscoverPlayer() { m_animationType = RUN; }

public:	//�ĂԂ��
	void HitAttack(int damage);
	OBB& GetAttackOBB();
	bool IsDead();
	bool IsFighting();
};

