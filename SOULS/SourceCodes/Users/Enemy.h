#pragma once
#include "../3D/FbxObjects.h"
#include "../3D/Object.h"
#include "../Math/OBBCollision.h"

#include "ParticleManager.h"

class Enemy
{
	//モーション→メリハリあると良くなりそう。ヒットストップとか。
	//バレットタイムとか。(色んな角度からアニメーション複数回再生的な)
	ParticleManager pManager;

private:
	enum AnimationType
	{
		STAND,		//待機
		RUN,		//移動
		KICK,		//蹴り攻撃
		PUNCH,		//殴り攻撃

		DIE,

		//追加
		R_TURN,		//右振り向き
		L_TURN,		//左振り向き
		R_BACK,		//右回転後ろ攻撃
		L_BACK,		//左回転後ろ攻撃

		//未実装
		EXPLOSION,	//爆発(周囲)
		RISE,		//上昇
		SWING_DOWN,	//振り下ろし(上昇とセット)
	};

private:
	enum TurnVec
	{
		RIGHT = 0,
		LEFT = 1
	};

private:	//定数(判定系)
	const int C_KICK_COLLISION_TIMER = 30;		//攻撃判定を取り出すフレーム
	const int C_KICK_COLLISION_ENDTIMER = 20;	//攻撃判定後、判定を取り消すフレーム

	const int C_PUNCH_COLLISION_TIMER = 15;		//攻撃判定を取り出すフレーム
	const int C_PUNCH_COLLISION_ENDTIMER = 30;	//攻撃判定後、判定を取り消すフレーム

	const int C_BACK_COLLISION_TIMER = 20;		//攻撃判定を取り出すフレーム
	const int C_BACK_COLLISION_ENDTIMER = 90;	//攻撃判定後、判定を取り消すフレーム

private:	//定数
	const float C_MAX_DIST = 35.0f;
	const float C_MAX_TURN_RAD = 15.0f;
	const float C_MAX_BACK_RAD = 150.0f;
	const float C_MAX_BLEND_TIMER = 0.02f;

private:	//定数(ステータス関係)
	const int C_MAX_POWER = 100;
	const int C_MAX_HP = 1000;

	const int C_RISE_TIMER = 110;
	const int C_SWING_DOWN_TIMER = 30;
	const float C_MAX_RISE_HEIGHT = 100.0f;
	const float C_MAX_MOVE_SPEED = 2.0f;		//移動速度
	const float C_MAX_TURN_TIMER = 0.01f;		//振り向きイージング
	const float C_MAX_RISE_TIMER = 0.01f;		//上昇イージング
	const float C_MAX_SWING_DOWN_TIMER = 0.02f;	//振り下ろしイージング

private:
	std::vector<OBB> m_obbs;
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_swingDownStartPos;
	DirectX::XMFLOAT3 m_swingDownEndPos;
	int m_animationTimer;
	int m_animationType;
	int m_oldAnimationType;
	//int m_boneCount;
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

private:	//変数(ステータス関係)
	int m_hp;

private:	//オブジェクト(Draw用)
	std::array<Object*, 12> obj_Box = { nullptr };
	const int C_BOX_NUM = 12;

	std::array<bool, 37> boxes = {
		0,1,0,0,0,
		1,0,0,1,0,
		1,0,1,0,1,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,1,1,1,
		0,0,1,1,1,
		0,0
	};

	std::array<FbxObjects*, 12> fbxobj_creature = { nullptr };
	const int C_CREATURE_NUM = 12;

public:
	Enemy();
	~Enemy();

	void Init();
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();

private:
	void CalcOBB();
	void JudgAnimationType(float dist);
	void CalcAngleDiff(DirectX::XMFLOAT3& pPos);
	void CalcNearAngle(DirectX::XMFLOAT3& pPos, float myAngleY);	//攻撃終了時に挟む
	void CalcAttackCollisionTimer(const int startFrame, const int endFrame);
	void CalcRise(DirectX::XMFLOAT3& pPos);
	void CalcSwingDown(DirectX::XMFLOAT3& pPos);

public:	//Getter
	const std::vector<OBB>& GetOBBs() { return m_obbs; }
	const DirectX::XMFLOAT3& GetPos() { return m_pos; }
	//const int GetBoneCount() { return m_boneCount; }
	const int GetPower() { return C_MAX_POWER; }
	const inline float GetHpRate() { return static_cast<float>(m_hp) / static_cast<float>(C_MAX_HP); }
	const bool IsInvincible() { return m_isInvincible; }
	const bool IsAttack() { return m_isAttack; }
	const bool IsCalc() { return m_isCalc; }

public:	//Setter
	void UnInvincible() { m_isInvincible = false; }
	void DiscoverPlayer() { m_animationType = RUN; }
	void SetHitOBBNum(int hitNum) { m_hitOBBNum = hitNum; }

public:	//呼ぶやつ
	void HitAttack(int damage);
	OBB& GetAttackOBB();
	bool IsDead();
	bool IsFighting();
};