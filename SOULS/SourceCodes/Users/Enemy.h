#pragma once
#include "../3D/FbxObjects.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

#include "ParticleManager.h"

class Enemy
{
	//���[�V�����������n������Ɨǂ��Ȃ肻���B�q�b�g�X�g�b�v�Ƃ��B
	//�o���b�g�^�C���Ƃ��B(�F��Ȋp�x����A�j���[�V����������Đ��I��)
	//�A������I

	ParticleManager pManager_Hit;
	ParticleManager pManager_Ex;

private:
	enum AnimationType
	{
		STAND,		//�ҋ@
		RUN,		//�ړ�
		KICK,		//�R��U��
		PUNCH,		//����U��

		DIE,

		//�ǉ�
		R_TURN,		//�E�U�����
		L_TURN,		//���U�����
		R_BACK,		//�E��]���U��
		L_BACK,		//����]���U��

		//������(���o�ƃt���[)
		EXPLOSION,	//����(����)
		RISE,		//�㏸
		SWING_DOWN,	//�U�艺�낵(�㏸�ƃZ�b�g)
		TACKLE		//�ːi
	};

private:
	enum TurnVec
	{
		RIGHT = 0,
		LEFT = 1
	};

private:	//�萔(����n)
	const int C_KICK_COLLISION_TIMER = 15;		//�U����������o���t���[��
	const int C_KICK_COLLISION_ENDTIMER = 30;	//�U�������A������������t���[��

	const int C_PUNCH_COLLISION_TIMER = 60;		//�U����������o���t���[��
	const int C_PUNCH_COLLISION_ENDTIMER = 20;	//�U�������A������������t���[��

	const int C_BACK_COLLISION_TIMER = 20;		//�U����������o���t���[��
	const int C_BACK_COLLISION_ENDTIMER = 90;	//�U�������A������������t���[��

	const int C_EXPLOSION_COLLISION_TIMER = 180;	//�����𗭂߂�t���[��
	const int C_EXPLOSION_COLLISION_DELAY = 65;		//�����Ȃ��t���[��
	const int C_EXPLOSION_COLLISION_ENDTIMER = 10;	//������锻����s���t���[��

private:	//�萔
	const float C_MAX_DIST = 35.0f;
	const float C_MAX_TURN_RAD = 45.0f;
	const float C_MAX_BACK_RAD = 150.0f;
	const float C_MAX_BLEND_TIMER = 0.02f;

private:	//�萔(�X�e�[�^�X�֌W)
	const int C_MAX_POWER = 300;
	const int C_MAX_EXPLOSION_POWER = 500;
	const int C_MAX_HP = 1000;

	const int C_RISE_TIMER = 110;
	const int C_SWING_DOWN_TIMER = 30;
	const float C_MAX_RISE_HEIGHT = 100.0f;
	const float C_MAX_MOVE_SPEED = 2.0f;		//�ړ����x
	const float C_MAX_TURN_TIMER = 0.02f;		//�U������C�[�W���O
	const float C_MAX_RISE_TIMER = 0.01f;		//�㏸�C�[�W���O
	const float C_MAX_SWING_DOWN_TIMER = 0.02f;	//�U�艺�낵�C�[�W���O

private:
	std::vector<OBB> m_obbs;
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_swingDownStartPos;
	DirectX::XMFLOAT3 m_swingDownEndPos;
	int m_animationTimer;
	int m_animationType;
	int m_oldAnimationType;
	//int m_boneCount;
	int m_hitOBBNum;
	float m_deg;
	float m_easeTimer;
	float m_turnStartAngle;
	float m_turnEndAngle;
	float m_dist;
	float m_blendTimer;
	float m_riseStartY;
	bool m_isInvincible;
	bool m_isAttack;
	bool m_isAttackTrigger;
	bool m_isCalc;
	bool m_isCalcEnd;
	bool m_isTurn;
	bool m_turnVec;
	bool m_isBackAttackLottery;
	bool m_isBackAttack;
	bool m_isRise;
	bool m_isSwing;
	bool m_isChange;

	DirectX::XMFLOAT3 m_tackleDirection = {};
	const int C_MAX_TACKLE_TIMER = 60;
	const int C_MAX_TACKLE_COUNT = 3;
	const float C_MAX_TACKLE_SPEED = 6.0f;
	const float C_MAX_TACKLE_RANGE = 200.0f;
	const float C_CALC_TACKLE_RANGE = 100.0f;
	int m_tackleCount = 0;
	bool m_isTackleRange = false;
	bool m_isTackleEnd = false;

	int m_pPowerCount = 0;
	int m_createCount = 1;
	int m_keepAnimationType = 0;
	bool m_isExplosion = false;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;

private:	//�I�u�W�F�N�g(Draw�p)
	std::array<Object*, 10> obj_Box = { nullptr };
	const int C_BOX_NUM = 10;

	std::array<bool, 37> boxes = {
		0,1,0,0,0,
		1,0,0,1,0,
		1,0,1,0,1,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,1,1,
		0,0,0,1,1,
		0,0
	};

	//std::array<FbxObjects*, 13> fbxobj_creature = { nullptr };
	FbxObjects* fbxobj_creature = nullptr;
	const int C_CREATURE_NUM = 13;

	Object* obj_circle = nullptr;

public:
	Enemy();
	~Enemy();

	void Init();
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();
	void LuminanceDraw();
	bool BeforeBattleScene();

private:
	void CalcOBB();
	void JudgAnimationType(float dist);
	void CalcAngleDiff(DirectX::XMFLOAT3& pPos);
	void CalcNearAngle(DirectX::XMFLOAT3& pPos, float myAngleY);				//�U���I�����ɋ���
	void CalcAttackCollisionTimer(const int startFrame, const int endFrame);	//�ڐG����̂�
	void CalcRise(DirectX::XMFLOAT3& pPos);
	void CalcSwingDown(DirectX::XMFLOAT3& pPos);
	void CalcTackle(DirectX::XMFLOAT3& playerPos);
	void CalcExplosion();

	void OtherUpdate(DirectX::XMFLOAT3& pPos);
	void SetImgui();
	void CalcBlendAnimation();

	float CalcDeg(DirectX::XMFLOAT3& pos);

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	//const int GetBoneCount() { return m_boneCount; }
	const int GetPower() { return C_MAX_POWER; }
	const int GetExplosionPower() { return C_MAX_EXPLOSION_POWER; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const bool IsInvincible() { return m_isInvincible; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsCalc() { return m_isCalc; }
	const bool IsExplosion() { return m_isExplosion; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }
	void DiscoverPlayer() { m_animationType = RUN; }
	void SetHitOBBNum(int hitNum) { m_hitOBBNum = hitNum; }

public:	//�ĂԂ��
	void HitAttack(int damage);
	std::vector<OBB> GetAttackOBBs();
	bool IsDead();
	bool IsFighting();
};