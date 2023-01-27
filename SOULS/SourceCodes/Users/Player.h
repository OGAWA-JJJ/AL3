#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../XAudio2/Music.h"
#include "../Math/OBBCollision.h"
#include "../3D/FbxObjects.h"
#include "../../imgui/ImguiControl.h"

#include "ParticleManager.h"
#include "TrailManager.h"

class Player
{
	//����ϑu�����A��ʈÓ]�A�m�����������ʐԂƂ��A�u���[���ȊO�͈Â�������Ƃ�...�B���킹�Z�������肷��B
	//�H�v�_�Ƃ��A�v���[�`���n�b�L�������Ƃ����B
	ParticleManager pManagerSword;
	ParticleManager pManagerEstus;

	ParticleManager pManagerLight0;
	ParticleManager pManagerLight1;
	ParticleManager pManagerLight2;
	ParticleManager pManagerLight3;

	TrailManager trail;

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
		SLOWRUN,			//�ᑬ�ړ�
		RUN,				//�ړ�
		BACK_RUN,			//�^�[�Q�b�g��,���ړ�
		R_RUN,				//�^�[�Q�b�g��,�E�ړ�
		L_RUN,				//�^�[�Q�b�g��,���ړ�
		NORMAL_ATTACK_1,	//1����,�U�����ړ��Ƒ�������؂�ւ��܂�
		NORMAL_ATTACK_2,	//2����
		NORMAL_ATTACK_3,	//3����
		DAMAGED,			//��e(������тȂ�)
		ROLLING,			//���
		DIE,				//���S��
		HEAL				//�A�C�e��(��)�g�p

		//�ǉ�
		//����؂�ւ�
		//��Z
		//�o�b�N�X�e�b�v
	};

	const int C_MIKU_NUM = 13;	//fbx�������ύX

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
	XMFLOAT3 m_enemyPos;
	int m_animationTimer;			//�U�����n�߂Ă���̃t���[����
	int m_animationType;			//���݂̃A�j���[�V�����^�C�v
	int m_oldAnimationType;			//�ߋ��̃A�j���[�V�����^�C�v
	int m_healTimer;				//�X�^�~�i�񕜔���p�^�C�}�[
	int m_padState;					//Pad�̓��͏��
	int m_padRetentionTimer;		//Pad�̐�s���͕ۑ��p
	int m_attackCollisionTimer[3];	//�U��������n�߂�t���[��
	int m_estusHeal;				//HP�񕜎c��
	int m_estusTimer;				//HP�񕜔���J�n�܂ł̃t���[��
	int m_trailCount;
	int m_keepAnimationType;
	float m_cameraMoveEase;			//�J�����̃C�[�W���O�p�^�C�}�[
	float m_cameraY;				//�J������Y���ۑ��p(�߂荞�݉��)
	float m_cameraDist;				//���@�ƃJ�����̋���
	float m_blendTimer;				//��ԗp�^�C�}�[
	float m_endAngle;
	float m_attackEase = 0.0f;
	bool m_isTarget;				//�^�[�Q�b�g���[�h����
	bool m_isEase;					//�J�������C�[�W���O����
	bool m_isAttack;				//�U������
	bool m_isInvincible;			//���G��
	bool m_isAccept;				//�r�����͂��󂯕t���Ă邩
	bool m_isChange;				//�A�j���[�V�������؂�ւ������
	bool m_isAnimation;				//STAND��RUN�ȊOtrue
	bool m_isEstus;					//�񕜒���
	bool m_isHeal;
	bool m_isHelmet;
	bool m_isTrailStart;
	bool m_isStickReleaseTrigger;
	bool m_isSheathed;				//�[���p
	bool m_isOldSheathed;

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

	std::shared_ptr<FbxObjects> fbxobj_miku = nullptr;

public:
	Player();
	~Player();

	void Init();
	void Update(DirectX::XMFLOAT3 enemyPos, bool isInputRecept = true);
	void Draw();
	void LuminanceDraw();
	void ShadowDraw();
	void ResetCamera();
	bool BeforeBattleScene();

	void AddMenbers();
	void ExportJson();
	void InportJson();

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
	void CalcRollingAngle();
	void CalcHeal();
	void CalcArea();
	void SetImgui();
	bool IsInputStick(float downPower);
	void CalcParticle();
	void CalcSwordParticle();
	void CalcEstusParticle();
	void CalcTrail();

	DirectX::XMFLOAT3 GetInputAngle();
	DirectX::XMFLOAT3 GetAngle();
	void SetAngle(DirectX::XMFLOAT3 angle);
	void CalcAngle();


public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const OBB& GetSwordOBB() { return m_obb; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	const int GetPower() { return m_power; }
	const int GetEstus() { return m_estus; }
	const inline float GetHpRate() {
		return static_cast<float>(m_hp) / static_cast<float>(ImguiControl::P_MAX_HP);
	}
	const inline float GetMpRate() {
		return static_cast<float>(m_mp) / static_cast<float>(ImguiControl::P_MAX_MP);
	}
	const inline float GetStaminaRate() {
		return static_cast<float>(m_stamina) / static_cast<float>(ImguiControl::P_MAX_STAMINA);
	}
	const float GetCameraDist() { return m_cameraDist; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsInvincible() { return m_isInvincible; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }	//���G�t�^(���d�q�b�g���p)

public:	//�ĂԂ��
	bool IsDead();				//���S����
	void HitAttack(int damage);	//�U���󂯂���Ă�
	void CalcExpulsion(bool isTackle);
};
