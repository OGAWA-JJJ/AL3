#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../XAudio2/Music.h"
#include "../Math/OBBCollision.h"
#include "../3D/FbxObjects.h"

#include "ParticleManager.h"
#include "TrailManager.h"

class Player
{
	//����ϑu�����A��ʈÓ]�A�m�����������ʐԂƂ��A�u���[���ȊO�͈Â�������Ƃ�...�B���킹�Z�������肷��B
	//�H�v�_�Ƃ��A�v���[�`���n�b�L�������Ƃ����B
	ParticleManager pManager;
	//TrailManager trail;

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:	//���@�̃p�^�[��
	enum AnimationType
	{
		STAND,				//�ҋ@
		SLOWRUN,			//�ᑬ�ړ�(���g�p)
		RUN,				//�ړ�
		BACK_RUN,			//�^�[�Q�b�g��
		R_RUN,
		L_RUN,
		NORMAL_ATTACK_1,	//�U��(�A����Ή�����ꁩ����)
		NORMAL_ATTACK_2,
		NORMAL_ATTACK_3,
		DAMAGED,			//��e(������т��Ȃ�)
		ROLLING,			//���
		DIE,				//���S��
		HEAL				//�A�C�e��(��)�g�p

		//�ǉ�
		//����؂�ւ�
		//��Z
		//�o�b�N�X�e�b�v
	};

private:		//�萔
	const float C_MAX_CAMERA_NEAR_DISTANCE = 75.0f;			//�J�����Ǝ��@�̋����̍ő�
	const float C_MAX_CAMERA_FAR_DISTANCE = 200.0f;			//�J�����Ǝ��@�̋����̍ő�

private:
	const int C_ATTACK_COLLISION_TIMER[3] = { 40,35,50 };	//�U������
	const int C_ATTACK_COLLISION_ENDTIMER = 55;				//�U�������A������������t���[��(�U���ɂ��C������)������������t���[���ɕύX

	const int C_AUTOHEAL_STAMINA_TIMER = 60;		//�񕜂��n�߂�܂ł̃t���[��
	const int C_ATTACK_SUB_STAMINA = 220;			//�����X�^�~�i(�U��)
	const int C_ROLLING_SUB_STAMINA = 200;			//�����X�^�~�i(���)
	const int C_HEAL_VOL = 5;						//1�t���[���̃X�^�~�i�񕜗�
	const int C_MAX_PAD_RETENTION = 60;				//PAD�ێ�����
	const float C_MAX_MOVE_SPEED = 1.5f;			//���@�̍ő呬�x
	const float C_MAX_CAMERA_MOVE_SPEED = 2.0f;		//�J�����̍ő呬�x
	const float C_EASE_CAMERA_TIMER = 0.01f;		//Target���[�h���؂�ւ�����ۂ̑��x
	const float C_MAX_BLEND_TIMER = 0.02f;

private:	//�萔(�X�e�[�^�X�֌W)
	const int C_MAX_HP = 1000;
	const int C_MAX_MP = 100;
	const int C_MAX_STAMINA = 1000;
	const int C_MAX_POWER[3] = { 40,25,70 };	//�ύX�K�v
	const int C_MAX_ESTUS = 5;					//������
	const int C_ESTUS_TIMER = 60;				//�G�X�g����ł���񕜂܂ł̃t���[����
	const int C_MAX_ESTUS_HEAL = 350;			//�񕜗�
	const int C_MAX_ESTUS_HEAL_SPEED = 15;		//1�t���[���̉񕜗�

private:	//�i�[�p
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> bones;	//�{�[�����
	std::vector<DirectX::XMMATRIX> matRot;							//��]�s����

private:	//�ϐ�
	std::vector<OBB> m_obbs;		//�S�g��OBB
	OBB m_obb;						//����OBB
	XMFLOAT3 m_pos;					//�|�W�V����
	XMFLOAT3 m_cameraAngle;			//�J�����̃A���O��
	XMFLOAT3 m_rollingAngle;		//�����������̃A���O��
	XMFLOAT3 m_cameraToPlayer;		//�J�������玩�@�̕����x�N�g��
	XMFLOAT3 m_cameraTarget;		//�J�����̃^�[�Q�b�g
	int m_animationTimer;			//�U�����n�߂Ă���̃t���[����
	int m_animationType;			//���݂̃A�j���[�V�����^�C�v
	int m_oldAnimationType;			//�ߋ��̃A�j���[�V�����^�C�v
	int m_healTimer;				//�X�^�~�i�񕜔���p�^�C�}�[
	int m_padState;					//Pad�̓��͏��
	int m_padRetentionTimer;		//Pad�̐�s���͕ۑ��p
	int m_attackCollisionTimer[3];	//�U��������n�߂�t���[��
	int m_estusHeal;				//HP�񕜎c��
	int m_estusTimer;				//HP�񕜔���J�n�܂ł̃t���[��
	float m_cameraMoveEase;			//�J�����̃C�[�W���O�p�^�C�}�[
	float m_cameraY;				//�J������Y���ۑ��p(�߂荞�݉��)
	float m_cameraDist;				//���@�ƃJ�����̋���
	float m_blendTimer;				//��ԗp�^�C�}�[
	bool m_isTarget;				//�^�[�Q�b�g���[�h����
	bool m_isEase;					//�J�������C�[�W���O����
	bool m_isAttack;				//�U������
	bool m_isInvincible;			//���G��
	bool m_isAccept;				//�r�����͂��󂯕t���Ă邩
	bool m_isChange;				//�A�j���[�V�������؂�ւ������
	bool m_isAnimation;				//STAND��RUN�ȊOtrue
	bool m_isEstus;					//�񕜒���

	//��
	int m_trailCount = 0;
	int m_keepAnimationType = 0;
	float m_attackEase = 0.0f;
	bool m_isHeal = true;
	bool m_isHelmet = true;
	bool m_isTrailStart = false;
	bool m_isStickReleaseTrigger = true;
	bool m_isSheathed = false;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;
	int m_mp;
	int m_stamina;
	int m_power;
	int m_estus;

private:	//�I�u�W�F�N�g(Draw�p)
	std::shared_ptr<Object> obj_Sword = nullptr;

	std::array<std::shared_ptr<Object>, 10> obj_Box = { nullptr };
	const int C_BOX_NUM = 10;
	std::array<bool, 28> boxes = {
		1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,1,1,0,1
	};

	std::shared_ptr<Object> obj_SwordBox = nullptr;
	std::shared_ptr<Object> obj_Helmet = nullptr;

	//std::array<FbxObjects*, 13> fbxobj_miku = { nullptr };
	//std::array<FbxObjects*, 13> fbxobj_shadowMiku = { nullptr };	//shadow�p�C��
	std::shared_ptr<FbxObjects> fbxobj_miku = nullptr;

	const int C_MIKU_NUM = 13;										//fbx�������ύX

public:
	Player();
	~Player();

	void Init();
	void Update(DirectX::XMFLOAT3 enemyPos);
	void Draw();
	void LuminanceDraw();
	void ShadowDraw();

private:
	void Input();		//�{����Input�Ȃ̂��������
	void Setter();		//���̂Ƃ���A����??
	void CalcOBB();		//����OBB�Z�o
	void OtherUpdate();	//��{�I��Update�ƌ��̒Ǐ]����
	void CalcBlendAnimation();
	void CalcAttack();
	void CalcAttackTimer();
	void CheckAttackAnimationType();
	void DoAttack(const int animationType);
	void CalcRolling();
	void CalcHeal();
	void CalcArea();
	void SetImgui();

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const OBB& GetSwordOBB() { return m_obb; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int GetPower() { return m_power; }
	const int GetEstus() { return m_estus; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const inline float GetMpRate() { return static_cast<float>(m_mp) / static_cast<float>(C_MAX_MP); }
	const inline float GetStaminaRate() { return static_cast<float>(m_stamina) / static_cast<float>(C_MAX_STAMINA); }
	const float GetCameraDist() { return m_cameraDist; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsInvincible() { return m_isInvincible; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }	//���G�t�^(���d�q�b�g���p)

public:	//�ĂԂ��
	bool IsDead();				//���S����
	void HitAttack(int damage);	//�U���󂯂���Ă�
};
