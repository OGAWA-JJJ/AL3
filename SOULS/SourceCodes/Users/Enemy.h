#pragma once
#include "../3D/FbxObjects.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"
#include "../../imgui/ImguiControl.h"

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

	//std::array<int, 3> m_attackPattern0 = { KICK,PUNCH,SWIP };

private:
	enum TurnVec
	{
		RIGHT = 0,
		LEFT = 1
	};

private:
	std::vector<OBB> m_obbs;
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_swingDownStartPos;
	DirectX::XMFLOAT3 m_swingDownEndPos;
	DirectX::XMFLOAT3 m_playerPos;
	DirectX::XMFLOAT3 m_tackleDirection;
	int m_animationTimer;
	int m_animationType;
	int m_oldAnimationType;
	int m_hitOBBNum;
	int m_tackleCount;
	int m_pPowerCount;
	int m_createCount;
	int m_keepAnimationType;
	float m_deg;
	float m_easeTimer;
	float m_turnStartAngle;
	float m_turnEndAngle;
	float m_dist;
	float m_blendTimer;
	float m_riseStartY;
	float m_beforeBattleEaseTimer;
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
	bool m_isTackleRange;
	bool m_isTackleEnd;
	bool m_isExplosion ;
	bool m_isDeadAnimationEnd ;
	bool m_beforeBattleEaseEndTrigger ;
	bool m_isRazerHit ;

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

	std::shared_ptr<FbxObjects> fbxobj_creature = nullptr;

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
	void ChangeAttackAnimation(int nextAnimationType);
	void EndAttackAnimation(int nextAnimationType = -1);

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int GetPower() { return ImguiControl::C_MAX_POWER; }
	const int GetExplosionPower() { return ImguiControl::C_MAX_EXPLOSION_POWER; }
	const inline float GetHpRate() {
		return static_cast<float>(m_hp) / static_cast<float>(ImguiControl::C_MAX_HP);
	}
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