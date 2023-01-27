#pragma once
#include <DirectXMath.h>

class ImguiControl
{
public:	//Camera
	static float Imgui_fov;
	static float Imgui_far_z;
	static float Imgui_eye_x;
	static float Imgui_eye_y;
	static float Imgui_eye_z;
	static float Imgui_target_x;
	static float Imgui_target_y;
	static float Imgui_target_z;

public:	//Light
	static float Imgui_lightColor_r;
	static float Imgui_lightColor_g;
	static float Imgui_lightColor_b;
	static float Imgui_lightDir_x;
	static float Imgui_lightDir_y;
	static float Imgui_lightDir_z;

public:	//Draw
	static bool Imgui_isOBBDraw;
	static bool Imgui_isPlayerDraw;
	static bool Imgui_isEnemyDraw;
	static bool Imgui_isWeaponDraw;
	static bool Imgui_isSponzaDraw;

	static float Imgui_cameraDist;

public:	//Enemy
	static float Imgui_enemyBlendTimer;
	static float Imgui_enemyCurrentAniTimer;
	static float Imgui_enemyOldAniTimer;
	static char* Imgui_enemyAniType;
	static char* Imgui_enemyOldAniType;
	static char* Imgui_enemyInvi;

	//判定系
	static int C_KICK_COLLISION_TIMER;
	static int C_KICK_COLLISION_ENDTIMER;

	static int C_PUNCH_COLLISION_TIMER;
	static int C_PUNCH_COLLISION_ENDTIMER;

	static int C_BACK_COLLISION_TIMER;
	static int C_BACK_COLLISION_ENDTIMER;

	static int C_EXPLOSION_COLLISION_TIMER;
	static int C_EXPLOSION_COLLISION_DELAY;
	static int C_EXPLOSION_COLLISION_ENDTIMER;

	static int C_SWIP_COLLISION_TIMER;
	static int C_SWIP_COLLISION_ENDTIMER;

	static int C_RAZER_COLLISION_STARTTIMER;
	static int C_RAZER_COLLISION_TIMER;
	static int C_RAZER_COLLISION_DELAY;

	static int C_SWINGDOWN_COLLISION_TIMER;
	static int C_SWINGDOWN_COLLISION_ENDTIMER;

	//その他
	static float C_MAX_DIST;
	static float C_MAX_TURN_RAD;
	static float C_MAX_BACK_RAD;
	static float C_MAX_BLEND_TIMER;

	static int C_MAX_POWER;
	static int C_MAX_EXPLOSION_POWER;
	static int C_MAX_HP;

	static int C_RISE_TIMER;
	static int C_SWING_DOWN_TIMER;
	static float C_MAX_RISE_HEIGHT;
	static float C_MAX_MOVE_SPEED;
	static float C_MAX_TURN_TIMER;
	static float C_MAX_RISE_TIMER;
	static float C_MAX_SWING_DOWN_TIMER;

	static int C_MAX_TACKLE_TIMER;
	static int C_MAX_TACKLE_COUNT;
	static float C_MAX_TACKLE_SPEED;
	static float C_MAX_TACKLE_RANGE;
	static float C_CALC_TACKLE_RANGE;
	static float c_beforeBattleAddTimer;

public:	//Player
	static float Imgui_playerBlendTimer;
	static float Imgui_playerCurrentAniTimer;
	static float Imgui_playerOldAniTimer;
	static char* Imgui_playerAniType;
	static char* Imgui_playerOldAniType;
	static char* Imgui_playerIsAccept;
	static char* Imgui_playerIsChange;
	static char* Imgui_playerIsAttack;
	static char* Imgui_playerIsInvincible;
	static bool isHel;

public:		//判定系
	static int P_ATTACK_COLLISION_TIMER[3];		//攻撃判定
	static int P_ATTACK_COLLISION_ENDTIMER;		//←回避を入れれるフレームに変更

public:		//定数
	static float P_MAX_CAMERA_NEAR_DISTANCE;	//カメラと自機の距離の最大
	static float P_MAX_CAMERA_FAR_DISTANCE;		//カメラと自機の距離の最大
	static int P_MAX_PAD_RETENTION;				//PAD保持時間
	static float P_MAX_CAMERA_MOVE_SPEED;		//カメラの最大速度
	static float P_EASE_CAMERA_TIMER;			//Targetモードが切り替わった際の速度
	static float P_MAX_BLEND_TIMER;				//補間速度
	static float P_DOWN_POWER;					//スティック反応ライン
	static float P_ROTATE_ADD_ANGLE;			//回転速度
	static float P_EXPLUSION_RAD;				//排斥

public:	//ステータス関係
	static int P_MAX_HP;
	static int P_MAX_MP;
	static int P_MAX_STAMINA;
	static int P_MAX_POWER[3];					//連撃ダメージ
	static int P_MAX_ESTUS;						//所持数
	static int P_ESTUS_TIMER;					//エスト飲んでから回復までのフレーム数
	static int P_MAX_ESTUS_HEAL;				//回復量
	static int P_MAX_ESTUS_HEAL_SPEED;			//1フレームの回復量
	static int P_AUTOHEAL_STAMINA_TIMER;		//回復し始めるまでのフレーム
	static int P_ATTACK_SUB_STAMINA;			//減少スタミナ(攻撃)
	static int P_ROLLING_SUB_STAMINA;			//減少スタミナ(回避)
	static int P_HEAL_VOL;						//1フレームのスタミナ回復量
	static float P_MAX_MOVE_SPEED;				//自機の最大速度

public:	//OBB
	static float Imgui_playerOBBPos[10][3];
	static float Imgui_enemyOBBPos[12][3];

	static float Imgui_playerOBBScale[10][3];
	static float Imgui_enemyOBBScale[12][3];

public:	//Shadow
	static float Imgui_shadowEye_x;
	static float Imgui_shadowEye_y;
	static float Imgui_shadowEye_z;
	static float Imgui_orthoMag_x;
	static float Imgui_orthoMag_y;

public:	//Stage
	static float Imgui_stageArea;
	static float Imgui_stageScale;

public:	//Other
	static bool Imgui_gameInit;
	static bool Imgui_enemyKill;
	static bool Imgui_export;
	static bool Imgui_inport;
	static bool Imgui_addMenbers;

public:	//Check
	static float SwordXt;
	static float SwordYt;
	static float SwordZt;

	static float SwordXr;
	static float SwordYr;
	static float SwordZr;

public:
	ImguiControl();
	static void Update();
};

