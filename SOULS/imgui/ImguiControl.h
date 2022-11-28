#pragma once
#include <DirectXMath.h>

class ImguiControl
{
public:	//Vectorにしたい
	static float Imgui_fov;
	static float Imgui_far_z;
	static float Imgui_lightColor_r;
	static float Imgui_lightColor_g;
	static float Imgui_lightColor_b;
	static float Imgui_lightDir_x;
	static float Imgui_lightDir_y;
	static float Imgui_lightDir_z;

	static bool Imgui_isOBBDraw;
	static bool Imgui_isPlayerDraw;
	static bool Imgui_isEnemyDraw;
	static bool Imgui_isWeaponDraw;
	static bool Imgui_isSponzaDraw;

	static float Imgui_cameraDist;

	static float Imgui_enemyBlendTimer;
	static float Imgui_enemyCurrentAniTimer;
	static float Imgui_enemyOldAniTimer;
	static char* Imgui_enemyAniType;
	static char* Imgui_enemyInvi;

	static float Imgui_playerBlendTimer;
	static float Imgui_playerCurrentAniTimer;
	static float Imgui_playerOldAniTimer;
	static char* Imgui_playerAniType;
	static char* Imgui_playerOldAniType;
	static char* Imgui_playerIsAccept;
	static char* Imgui_playerIsChange;

	static float Imgui_playerOBBPos[10][3];
	static float Imgui_enemyOBBPos[12][3];

	//仮
	static bool isHel;

public:
	ImguiControl();
	static void Update();
};

