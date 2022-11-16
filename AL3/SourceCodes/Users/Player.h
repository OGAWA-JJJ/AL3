#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../XAudio2/Music.h"
#include "../3D/FbxObject3D.h"
#include "../Math/OBBCollision.h"
#include "../3D/FbxObjects.h"

class Player
{
	//����ϑu�����A��ʈÓ]�A�m�����������ʐԂƂ��A�u���[���ȊO�͈Â�������Ƃ�...�B���킹�Z�������肷��B
	//�H�v�_�Ƃ��A�v���[�`���n�b�L�������Ƃ����B

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
		NORMAL_ATTACK_1,	//�U��(�A����Ή� �� ���)
		NORMAL_ATTACK_2,
		NORMAL_ATTACK_3,
		DAMAGED,			//��e(������т��Ȃ�)
		ROLLING,			//���
		DIE					//���S��

		//�V�K
		//�A�C�e���g�p
		//����؂�ւ�

		//��Z
		//�o�b�N�X�e�b�v
	};

private:		//�萔
	const float C_MAX_CAMERA_NEAR_DISTANCE = 125.0f;	//�J�����Ǝ��@�̋����̍ő�
	const float C_MAX_CAMERA_FAR_DISTANCE = 200.0f;		//�J�����Ǝ��@�̋����̍ő�

private:
	const int C_ATTACK_COLLISION_TIMER = 20;		//�U����������o���t���[��
	const int C_ATTACK_COLLISION_ENDTIMER = 60;		//�U�������A������������t���[��(�U���ɂ��C������)������������t���[��
	const int C_HEAL_TIMER = 90;					//�񕜂��n�߂�܂ł̃t���[��
	const int C_ATTACK_SUB_STAMINA = 100;			//�����X�^�~�i(�U��)
	const int C_ROLLING_SUB_STAMINA = 100;			//�����X�^�~�i(���)
	const int C_HEAL_VOL = 10;						//1�t���[���̃X�^�~�i�񕜗�
	const int C_MAX_PAD_RETENTION = 60;				//PAD�ێ�����
	const float C_MAX_MOVE_SPEED = 2.0f;			//���@�̍ő呬�x
	const float C_MAX_CAMERA_MOVE_SPEED = 2.0f;		//�J�����̍ő呬�x
	const float C_EASE_CAMERA_TIMER = 0.006f;		//Target���[�h���؂�ւ�����ۂ̑��x
	const float C_MAX_BLEND_TIMER = 0.02f;

private:	//�萔(�X�e�[�^�X�֌W)
	const int C_MAX_HP = 1000;
	const int C_MAX_MP = 100;
	const int C_MAX_STAMINA = 1000;
	const int C_MAX_POWER = 100;

private:	//�i�[�p
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> bones;
	std::vector<DirectX::XMMATRIX> matRot;

private:	//�ϐ�
	std::vector<OBB> m_obbs;
	OBB m_obb;
	XMFLOAT3 m_pos;
	XMFLOAT3 m_cameraAngle;
	XMFLOAT3 m_rollingAngle;
	XMFLOAT3 m_cameraToPlayer;
	int m_animationTimer;
	int m_animationType;
	int m_oldAnimationType;
	int m_healTimer;
	int m_padState;
	int m_padRetentionTimer;
	float m_cameraMoveEase;
	float m_cameraY;
	float m_cameraDist;
	float m_blendTimer;
	bool m_isTarget;
	bool m_isEase;
	bool m_isAttack;
	bool m_isInvincible;
	bool m_isAccept;
	bool m_isChange;
	bool m_isAnimation;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;
	int m_mp;
	int m_stamina;
	int m_power;

private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_Sword = nullptr;
	Object* obj_ShadowSword = nullptr;

	Object* obj_Box[28] = { nullptr };
	Object* obj_SwordBox = nullptr;
	Object* obj_HitBox = nullptr;

	FbxObjects* fbxobj_miku[9] = { nullptr };
	FbxObjects* fbxobj_shadowMiku[9] = { nullptr };	//shadow�p�C��
	const int C_MIKU_NUM = 9;						//fbx�������ύX

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

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const OBB& GetSwordOBB() { return m_obb; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int GetPower() { return m_power; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const inline float GetMpRate() { return static_cast<float>(m_mp) / static_cast<float>(C_MAX_MP); }
	const inline float GetStaminaRate() { return static_cast<float>(m_stamina) / static_cast<float>(C_MAX_STAMINA); }
	const float GetCameraDist() { return m_cameraDist; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsInvincible() { return m_isInvincible; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }

public:	//�ĂԂ��
	bool IsDead();
	void HitAttack(int damage);
};
