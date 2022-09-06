#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../XAudio2/Music.h"
#include "../3D/FbxObject3D.h"
#include "../Math/OBBCollision.h"

class Player
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:	//���@�̃p�^�[��
	enum AnimationType
	{
		STAND, SLOWRUN, RUN, ATTACK, DAMAGED, ROLLING
	};

public:		//�萔
	const float C_MAX_DISTANCE = 125.0f;			//�J�����Ǝ��@�̋���(�����ς�)

private:
	const int C_ATTACK_COLLISION_TIMER = 20;		//�U����������o���t���[��
	const int C_ATTACK_COLLISION_ENDTIMER = 30;		//�U�������A������������t���[��(�U���ɂ��C������)
	const int C_HEAL_TIMER = 90;					//�񕜂��n�߂�܂ł̃t���[��
	const int C_ATTACK_SUB_STAMINA = 100;			//�����X�^�~�i(�U��)
	const int C_ROLLING_SUB_STAMINA = 100;			//�����X�^�~�i(���)
	const int C_HEAL_VOL = 10;						//1�t���[���̃X�^�~�i�񕜗�
	const float C_MAX_MOVE_SPEED = 2.0f;			//���@�̍ő呬�x
	const float C_MAX_CAMERA_MOVE_SPEED = 2.0f;		//�J�����̍ő呬�x
	const float C_EASE_CAMERA_TIMER = 0.006f;		//Target���[�h���؂�ւ�����ۂ̑��x

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
	int m_animationTimer;
	int m_animationType;
	int m_healTimer;
	float m_cameraMoveEase;
	float m_cameraY;
	bool m_isTarget;
	bool m_isEase;
	bool m_isAttack;
	bool m_isInvincible;

private:	//�ϐ�(�X�e�[�^�X�֌W)
	int m_hp;
	int m_mp;
	int m_stamina;

	int m_power;

private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_Sword = nullptr;
	Object* obj_ShadowSword = nullptr;
	//std::weak_ptr<Object> obj_Sword;
	//std::weak_ptr<Object> obj_ShadowSword;
	Object* obj_Box[28] = { nullptr };
	Object* obj_SwordBox = nullptr;
	Object* obj_HitBox = nullptr;

	FbxObject3D* fbxobj_StandMiku = nullptr;
	FbxObject3D* fbxobj_SlowRunMiku = nullptr;
	FbxObject3D* fbxobj_FastRunMiku = nullptr;
	FbxObject3D* fbxobj_StandShadowMiku = nullptr;
	FbxObject3D* fbxobj_SlowRunShadowMiku = nullptr;
	FbxObject3D* fbxobj_FastRunShadowMiku = nullptr;

	FbxObject3D* fbxobj_OneSwordAttack = nullptr;
	FbxObject3D* fbxobj_OneSwordAttackShadow = nullptr;

	FbxObject3D* fbxobj_dieMiku = nullptr;
	FbxObject3D* fbxobj_impactMiku = nullptr;
	FbxObject3D* fbxobj_rollingMiku = nullptr;

	FbxObject3D* fbxobj_dieShadowMiku = nullptr;
	FbxObject3D* fbxobj_impactShadowMiku = nullptr;
	FbxObject3D* fbxobj_rollingShadowMiku = nullptr;
	//std::weak_ptr<FbxObject> fbxobj_StandMiku;
	//std::weak_ptr<FbxObject> fbxobj_SlowRunMiku;
	//std::weak_ptr<FbxObject> fbxobj_FastRunMiku;
	//std::weak_ptr<FbxObject> fbxobj_StandShadowMiku;
	//std::weak_ptr<FbxObject> fbxobj_SlowRunShadowMiku;
	//std::weak_ptr<FbxObject> fbxobj_FastRunShadowMiku;

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

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const OBB& GetSwordOBB() { return m_obb; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int& GetPower() { return m_power; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const inline float GetMpRate() { return static_cast<float>(m_mp) / static_cast<float>(C_MAX_MP); }
	const inline float GetStaminaRate() { return static_cast<float>(m_stamina) / static_cast<float>(C_MAX_STAMINA); }
	const bool IsAttack() { return m_isAttack; }
	const bool IsInvincible() { return m_isInvincible; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }

public:	//�ĂԂ��
	bool IsDead();
	void HitAttack(int damage);
};
