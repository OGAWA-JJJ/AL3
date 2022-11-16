#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../XAudio2/Music.h"
#include "../3D/FbxObject3D.h"
#include "../Math/OBBCollision.h"
#include "../3D/FbxObjects.h"

class Player
{
	//やっぱ爽快感、画面暗転、瀕死だったら画面赤とか、ブルーム以外は暗くしたりとか...。合わせ技だったりする。
	//工夫点とかアプローチをハッキリ喋れるといい。

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:	//自機のパターン
	enum AnimationType
	{
		STAND,				//待機
		SLOWRUN,			//低速移動(未使用)
		RUN,				//移動
		NORMAL_ATTACK_1,	//攻撃(連撃非対応 ← やれ)
		NORMAL_ATTACK_2,
		NORMAL_ATTACK_3,
		DAMAGED,			//被弾(吹っ飛びがない)
		ROLLING,			//回避
		DIE					//死亡時

		//新規
		//アイテム使用
		//武器切り替え

		//戦技
		//バックステップ
	};

private:		//定数
	const float C_MAX_CAMERA_NEAR_DISTANCE = 125.0f;	//カメラと自機の距離の最大
	const float C_MAX_CAMERA_FAR_DISTANCE = 200.0f;		//カメラと自機の距離の最大

private:
	const int C_ATTACK_COLLISION_TIMER = 20;		//攻撃判定を取り出すフレーム
	const int C_ATTACK_COLLISION_ENDTIMER = 60;		//攻撃判定後、判定を取り消すフレーム(攻撃による気もする)←回避を入れれるフレーム
	const int C_HEAL_TIMER = 90;					//回復し始めるまでのフレーム
	const int C_ATTACK_SUB_STAMINA = 100;			//減少スタミナ(攻撃)
	const int C_ROLLING_SUB_STAMINA = 100;			//減少スタミナ(回避)
	const int C_HEAL_VOL = 10;						//1フレームのスタミナ回復量
	const int C_MAX_PAD_RETENTION = 60;				//PAD保持時間
	const float C_MAX_MOVE_SPEED = 2.0f;			//自機の最大速度
	const float C_MAX_CAMERA_MOVE_SPEED = 2.0f;		//カメラの最大速度
	const float C_EASE_CAMERA_TIMER = 0.006f;		//Targetモードが切り替わった際の速度
	const float C_MAX_BLEND_TIMER = 0.02f;

private:	//定数(ステータス関係)
	const int C_MAX_HP = 1000;
	const int C_MAX_MP = 100;
	const int C_MAX_STAMINA = 1000;
	const int C_MAX_POWER = 100;

private:	//格納用
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> bones;
	std::vector<DirectX::XMMATRIX> matRot;

private:	//変数
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

private:	//変数(ステータス関係)
	int m_hp;
	int m_mp;
	int m_stamina;
	int m_power;

private:	//オブジェクト(Draw用)
	Object* obj_Sword = nullptr;
	Object* obj_ShadowSword = nullptr;

	Object* obj_Box[28] = { nullptr };
	Object* obj_SwordBox = nullptr;
	Object* obj_HitBox = nullptr;

	FbxObjects* fbxobj_miku[9] = { nullptr };
	FbxObjects* fbxobj_shadowMiku[9] = { nullptr };	//shadow用修正
	const int C_MIKU_NUM = 9;						//fbx増減時変更

public:
	Player();
	~Player();

	void Init();
	void Update(DirectX::XMFLOAT3 enemyPos);
	void Draw();
	void LuminanceDraw();
	void ShadowDraw();

private:
	void Input();		//本当にInputなのか分からん
	void Setter();		//今のところ、いる??
	void CalcOBB();		//剣のOBB算出
	void OtherUpdate();	//基本的にUpdateと剣の追従処理
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

public:	//呼ぶやつ
	bool IsDead();
	void HitAttack(int damage);
};
