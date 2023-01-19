#pragma once
#include "../3D/FbxObjects.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

#include "ParticleManager.h"

class Enemy
{
	//モーション→メリハリあると良くなりそう。ヒットストップとか。
	//バレットタイムとか。(色んな角度からアニメーション複数回再生的な)
	//連撃しろ！

	ParticleManager pManagerHit;
	ParticleManager pManagerEx;
	ParticleManager pManagerRazer;

private:
	enum AnimationType
	{
		STAND,		//待機
		RUN,		//移動
		KICK,		//蹴り攻撃
		PUNCH,		//殴り攻撃
		DIE,
		R_TURN,		//右振り向き
		L_TURN,		//左振り向き
		R_BACK,		//右回転後ろ攻撃
		L_BACK,		//左回転後ろ攻撃
		EXPLOSION,	//爆発(周囲)
		RISE,		//上昇
		SWING_DOWN,	//振り下ろし(上昇とセット)←演出ない
		TACKLE,		//突進
		AWAKE,		//戦闘前
		SWIP,		//腕振り
		MAGIC,		//遠距離攻撃(エスト狩りに使う??)
		RAZER,		//レーザー(突進後)
		ULT_READY,	//半分超えたら
		ULT			//ハルバード出す
	};

private:
	enum TurnVec
	{
		RIGHT = 0,
		LEFT = 1
	};

private:	//定数(判定系)
	int C_KICK_COLLISION_TIMER = 15;		//攻撃判定を取り出すフレーム
	int C_KICK_COLLISION_ENDTIMER = 30;	//攻撃判定後、判定を取り消すフレーム

	int C_PUNCH_COLLISION_TIMER = 60;		//攻撃判定を取り出すフレーム
	int C_PUNCH_COLLISION_ENDTIMER = 20;	//攻撃判定後、判定を取り消すフレーム

	int C_BACK_COLLISION_TIMER = 20;		//攻撃判定を取り出すフレーム
	int C_BACK_COLLISION_ENDTIMER = 90;	//攻撃判定後、判定を取り消すフレーム

	int C_EXPLOSION_COLLISION_TIMER = 180;	//爆発を溜めるフレーム
	int C_EXPLOSION_COLLISION_DELAY = 65;		//溜めた後の何もないフレーム
	int C_EXPLOSION_COLLISION_ENDTIMER = 10;	//解放する判定を行うフレーム

	int C_SWIP_COLLISION_TIMER = 40;
	int C_SWIP_COLLISION_ENDTIMER = 10;

	int C_RAZER_COLLISION_STARTTIMER = 20;
	int C_RAZER_COLLISION_TIMER = 90;
	int C_RAZER_COLLISION_DELAY = 60;

	int C_SWINGDOWN_COLLISION_TIMER = 40;
	int C_SWINGDOWN_COLLISION_ENDTIMER = 10;

private:	//定数
	float C_MAX_DIST = 35.0f;				//攻撃判定開始
	float C_MAX_TURN_RAD = 45.0f;			//振り向きが入る角度
	float C_MAX_BACK_RAD = 150.0f;		//後ろ攻撃が入る角度
	float C_MAX_BLEND_TIMER = 0.02f;		//補間速度

private:	//定数(ステータス関係)
	int C_MAX_POWER = 300;
	int C_MAX_EXPLOSION_POWER = 500;
	int C_MAX_HP = 1000;

	int C_RISE_TIMER = 110;				//上昇
	int C_SWING_DOWN_TIMER = 30;			//振り下ろし
	float C_MAX_RISE_HEIGHT = 100.0f;		//上昇高さ
	float C_MAX_MOVE_SPEED = 2.0f;		//移動速度
	float C_MAX_TURN_TIMER = 0.02f;		//振り向きイージング
	float C_MAX_RISE_TIMER = 0.01f;		//上昇イージング
	float C_MAX_SWING_DOWN_TIMER = 0.02f;	//振り下ろしイージング

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

private:	//変数(ステータス関係)
	int m_hp;

private:	//オブジェクト(Draw用)
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
	void CalcNearAngle(float myAngleY);				//攻撃終了時に挟む
	void CalcAttackCollisionTimer(const int startFrame, const int endFrame);	//接触判定のみ
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

public:	//呼ぶやつ
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