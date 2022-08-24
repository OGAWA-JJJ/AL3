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
		STAND, SLOWRUN, RUN, ATTACK, DAMAGED
	};

public:		//�萔
	const float C_MAX_DISTANCE = 125.0f;			//�J�����Ǝ��@�̋���(�����ς�)

private:
	const int C_ATTACK_COLLISION_TIMER = 20;		//�U����������o���t���[��
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
	int m_animationTimer;
	int m_animationType;
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
	void Input();
	void Setter();
	void CalcOBB();
	void OtherUpdate();

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const OBB& GetSwordOBB() { return m_obb; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int& GetPower() { return m_power; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsInvincible() { return m_isInvincible; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }

public:	//�ĂԂ��
	const bool IsDead()
	{
		if (m_hp <= 0) { return true; }
		return false;
	}
	void HitAttack(int damage)
	{
		m_hp -= damage;
		m_isInvincible = true;
		m_animationType = DAMAGED;
		fbxobj_OneSwordAttack->ResetAnimation();
		if (m_hp < 0) { m_hp = 0; }
		OutputDebugStringA("Hit!\n");
	}
};
