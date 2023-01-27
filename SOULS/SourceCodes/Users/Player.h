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
	//やっぱ爽快感、画面暗転、瀕死だったら画面赤とか、ブルーム以外は暗くしたりとか...。合わせ技だったりする。
	//工夫点とかアプローチをハッキリ喋れるといい。
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

private:	//自機のパターン
	enum AnimationType
	{
		STAND,				//待機
		SLOWRUN,			//低速移動
		RUN,				//移動
		BACK_RUN,			//ターゲット中,後ろ移動
		R_RUN,				//ターゲット中,右移動
		L_RUN,				//ターゲット中,左移動
		NORMAL_ATTACK_1,	//1撃目,攻撃中移動と操作方向切り替えまだ
		NORMAL_ATTACK_2,	//2撃目
		NORMAL_ATTACK_3,	//3撃目
		DAMAGED,			//被弾(吹っ飛びなし)
		ROLLING,			//回避
		DIE,				//死亡時
		HEAL				//アイテム(回復)使用

		//追加
		//武器切り替え
		//戦技
		//バックステップ
	};

	const int C_MIKU_NUM = 13;	//fbx増減時変更

private:	//格納用
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> bones;	//ボーン情報
	std::vector<DirectX::XMMATRIX> matRot;							//回転行列情報

private:	//変数
	std::vector<OBB> m_obbs;		//全身のOBB
	OBB m_obb;						//剣のOBB
	XMFLOAT3 m_pos;					//ポジション
	XMFLOAT3 m_cameraAngle;			//カメラのアングル
	XMFLOAT3 m_rollingAngle;		//回避する方向のアングル
	XMFLOAT3 m_cameraToPlayer;		//カメラから自機の方向ベクトル
	XMFLOAT3 m_cameraTarget;		//カメラのターゲット
	XMFLOAT3 m_enemyPos;
	int m_animationTimer;			//攻撃し始めてからのフレーム数
	int m_animationType;			//現在のアニメーションタイプ
	int m_oldAnimationType;			//過去のアニメーションタイプ
	int m_healTimer;				//スタミナ回復判定用タイマー
	int m_padState;					//Padの入力情報
	int m_padRetentionTimer;		//Padの先行入力保存用
	int m_attackCollisionTimer[3];	//攻撃判定を始めるフレーム
	int m_estusHeal;				//HP回復残量
	int m_estusTimer;				//HP回復判定開始までのフレーム
	int m_trailCount;
	int m_keepAnimationType;
	float m_cameraMoveEase;			//カメラのイージング用タイマー
	float m_cameraY;				//カメラのY軸保存用(めり込み回避)
	float m_cameraDist;				//自機とカメラの距離
	float m_blendTimer;				//補間用タイマー
	float m_endAngle;
	float m_attackEase = 0.0f;
	bool m_isTarget;				//ターゲットモード中か
	bool m_isEase;					//カメラがイージング中か
	bool m_isAttack;				//攻撃中か
	bool m_isInvincible;			//無敵か
	bool m_isAccept;				//途中入力を受け付けてるか
	bool m_isChange;				//アニメーションが切り替わったか
	bool m_isAnimation;				//STANDとRUN以外true
	bool m_isEstus;					//回復中か
	bool m_isHeal;
	bool m_isHelmet;
	bool m_isTrailStart;
	bool m_isStickReleaseTrigger;
	bool m_isSheathed;				//納刀用
	bool m_isOldSheathed;

private:	//変数(ステータス関係)
	int m_hp;
	int m_mp;
	int m_stamina;
	int m_power;
	int m_estus;

private:	//オブジェクト(Draw用)
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
	void Input();		//本当にInputなのか分からん
	void Setter();		//今のところ、いる??
	void CalcOBB();		//剣のOBB算出
	void OtherUpdate();	//基本的にUpdateと剣の追従処理
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
	void UnInvincible() { m_isInvincible = false; }	//無敵付与(多重ヒット回避用)

public:	//呼ぶやつ
	bool IsDead();				//死亡判定
	void HitAttack(int damage);	//攻撃受けたら呼ぶ
	void CalcExpulsion(bool isTackle);
};
