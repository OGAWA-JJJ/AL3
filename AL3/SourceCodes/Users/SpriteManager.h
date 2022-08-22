#pragma once
#include "../2D/Sprite.h"

class SpriteManager
{
private:
	static float C_ENEMY_HP_BAR_SIZE_X;
	static float C_ENEMY_HP_BAR_SIZE_Y;
	static int C_YELLOW_MOVE_DELAY;

private:
	static int m_yellowCount;
	static float m_hpRate;
	static float m_diedAlpha;
	static bool m_enemyDamaged;

private:
	static Sprite* tex_numbers[10];

	static Sprite* tex_hp_red;
	static Sprite* tex_hp_yellow;
	static Sprite* tex_hp_back;
	static Sprite* tex_hp_flame;

	static Sprite* tex_enemy_name;
	static Sprite* tex_died;

public:
	static void Init();
	static void Update();
	static void EnemyUIDraw();
	static void DiedDraw();

	static void PlayerDamaged(float hpRate);
	static void EnemyDamaged(float hpRate);
};

