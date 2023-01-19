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

	ParticleManager pManagerHit;
	ParticleManager pManagerEx;
	ParticleManager pManagerRazer;

private:
	enum AnimationType
	{
		STAND,		//�ҋ@
		RUN,		//�ړ�
		KICK,		//�R��U��
		PUNCH,		//����U��
		DIE,
		R_TURN,		//�E�U�����
		L_TURN,		//���U�����
		R_BACK,		//�E��]���U��
		L_BACK,		//����]���U��
		EXPLOSION,	//����(����)
		RISE,		//�㏸
		SWING_DOWN,	//�U�艺�낵(�㏸�ƃZ�b�g)�����o�Ȃ�
		TACKLE,		//�ːi
		AWAKE,		//�퓬�O
		SWIP,		//�r�U��
		MAGIC,		//�������U��(�G�X�g���Ɏg��??)
		RAZER,		//���[�U�[(�ːi��)
		ULT_READY,	//������������
		ULT			//�n���o�[�h�o��
	};

private:
	enum TurnVec
	{
		RIGHT = 0,
		LEFT = 1
	};

private:	//�萔(����n)
	int C_KICK_COLLISION_TIMER = 15;		//�U����������o���t���[��
	int C_KICK_COLLISION_ENDTIMER = 30;	//�U�������A������������t���[��

	int C_PUNCH_COLLISION_TIMER = 60;		//�U����������o���t���[��
	int C_PUNCH_COLLISION_ENDTIMER = 20;	//�U�������A������������t���[��

	int C_BACK_COLLISION_TIMER = 20;		//�U����������o���t���[��
	int C_BACK_COLLISION_ENDTIMER = 90;	//�U�������A������������t���[��

	int C_EXPLOSION_COLLISION_TIMER = 180;	//�����𗭂߂�t���[��
	int C_EXPLOSION_COLLISION_DELAY = 65;		//���߂���̉����Ȃ��t���[��
	int C_EXPLOSION_COLLISION_ENDTIMER = 10;	//������锻����s���t���[��

	int C_SWIP_COLLISION_TIMER = 40;
	int C_SWIP_COLLISION_ENDTIMER = 10;

	int C_RAZER_COLLISION_STARTTIMER = 20;
	int C_RAZER_COLLISION_TIMER = 90;
	int C_RAZER_COLLISION_DELAY = 60;

	int C_SWINGDOWN_COLLISION_TIMER = 40;
	int C_SWINGDOWN_COLLISION_ENDTIMER = 10;

private:	//�萔
	float C_MAX_DIST = 35.0f;				//�U������J�n
	float C_MAX_TURN_RAD = 45.0f;			//�U�����������p�x
	float C_MAX_BACK_RAD = 150.0f;		//���U��������p�x
	float C_MAX_BLEND_TIMER = 0.02f;		//��ԑ��x

private:	//�萔(�X�e�[�^�X�֌W)
	int C_MAX_POWER = 300;
	int C_MAX_EXPLOSION_POWER = 500;
	int C_MAX_HP = 1000;

	int C_RISE_TIMER = 110;				//�㏸
	int C_SWING_DOWN_TIMER = 30;			//�U�艺�낵
	float C_MAX_RISE_HEIGHT = 100.0f;		//�㏸����
	float C_MAX_MOVE_SPEED = 2.0f;		//�ړ����x
	float C_MAX_TURN_TIMER = 0.02f;		//�U������C�[�W���O
	float C_MAX_RISE_TIMER = 0.01f;		//�㏸�C�[�W���O
	float C_MAX_SWING_DOWN_TIMER = 0.02f;	//�U�艺�낵�C�[�W���O

private:
	std::vector<OBB> m_obbs;
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_swingDownStartPos;
	DirectX::XMFLOAT3 m_swingDownEndPos;
	DirectX::XMFLOAT3 m_playerPos;
	int m_animationTimer;
	int m_animationType;
	int m_oldAnimationType;
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
	const int C_MAX_TACKLE_TIMER = 50;
	const int C_MAX_TACKLE_COUNT = 3;
	const float C_MAX_TACKLE_SPEED = 7.0f;
	const float C_MAX_TACKLE_RANGE = 200.0f;
	const float C_CALC_TACKLE_RANGE = 100.0f;
	int m_tackleCount = 0;
	bool m_isTackleRange = false;
	bool m_isTackleEnd = false;

	int m_pPowerCount = 0;
	int m_createCount = 1;
	int m_keepAnimationType = 0;
	bool m_isExplosion = false;

	bool m_isDeadAnimationEnd = false;

	bool m_beforeBattleEaseEndTrigger = false;
	float m_beforeBattleEaseTimer = 0.0f;
	const float c_beforeBattleAddTimer = 0.004f;

	bool m_isRazerHit = false;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;

private:	//�I�u�W�F�N�g(Draw�p)
	std::array<std::shared_ptr<Object>, 10> obj_Box = { nullptr };
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
	std::shared_ptr<FbxObjects> fbxobj_creature = nullptr;
	const int C_CREATURE_NUM = 13;

public:
	Enemy();
	~Enemy();

	void Init();
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();
	void LuminanceDraw();
	void ShadowDraw();
	bool BeforeBattleScene();

	void AddMenbers();
	void ExportJson();
	void InportJson();

private:
	void CalcOBB();
	void JudgAnimationType(float dist);
	void CalcAngleDiff();
	void CalcNearAngle(float myAngleY);				//�U���I�����ɋ���
	void CalcAttackCollisionTimer(const int startFrame, const int endFrame);	//�ڐG����̂�
	void CalcRise();
	void CalcSwingDown();
	void CalcTackle();
	void CalcExplosion();
	void CalcSwip();
	void CalcRazer();

	void OtherUpdate();
	void SetImgui();
	void CalcBlendAnimation();

	float CalcDeg();
	void CalcArea();

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int GetPower() { return C_MAX_POWER; }
	const int GetExplosionPower() { return C_MAX_EXPLOSION_POWER; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const bool IsInvincible() { return m_isInvincible; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsCalc() { return m_isCalc; }
	const bool IsExplosion() { return m_isExplosion; }
	const bool IsDeadAnimationEnd() { return m_isDeadAnimationEnd; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }
	void DiscoverPlayer() { m_animationType = RUN; }
	void SetHitOBBNum(int hitNum) { m_hitOBBNum = hitNum; }
	void SelfKill() { m_hp = 0; }

public:	//�ĂԂ��
	void HitAttack(int damage);
	std::vector<OBB> GetAttackOBBs();
	bool IsDead();
	bool IsFighting();
	bool IsTackle()
	{
		if (m_animationType == TACKLE)
		{
			return true;
		}
		return false;
	}

	bool IsRazerHit()
	{
		return m_isRazerHit;
	}
};