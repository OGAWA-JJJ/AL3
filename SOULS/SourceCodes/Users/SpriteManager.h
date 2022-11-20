#pragma once
#include "../2D/Sprite.h"

class SpriteManager
{
private:
	static float C_ENEMY_HP_BAR_SIZE_X;
	static float C_ENEMY_HP_BAR_SIZE_Y;

	static float C_PLAYER_HP_BAR_SIZE_X;
	static float C_PLAYER_HP_BAR_SIZE_Y;

	static float C_PLAYER_MP_BAR_SIZE_X;
	static float C_PLAYER_STAMINA_BAR_SIZE_X;

	static int C_YELLOW_MOVE_DELAY;

private:
	static DirectX::XMFLOAT4 m_texColor;
	static int m_enemy_yellowCount;
	static int m_player_yellowCount;
	static float m_enemy_hpRate;
	static float m_player_hpRate;
	static float m_diedAlpha;
	static bool m_enemyDamaged;
	static bool m_playerDamaged;
	static bool m_isStart;
	static bool m_isSceneChangeEnd;

private:
	static Sprite* tex_numbers[10];

	static Sprite* tex_enemy_hp_red;
	static Sprite* tex_enemy_hp_yellow;
	static Sprite* tex_enemy_hp_back;
	static Sprite* tex_enemy_hp_flame;

	static Sprite* tex_enemy_name;
	static Sprite* tex_died;

	static Sprite* tex_player_hp_red;
	static Sprite* tex_player_hp_yellow;
	static Sprite* tex_player_hp_back;

	static Sprite* tex_player_mp_blue;
	static Sprite* tex_player_mp_yellow;
	static Sprite* tex_player_mp_back;

	static Sprite* tex_player_stamina_green;
	static Sprite* tex_player_stamina_yellow;
	static Sprite* tex_player_stamina_back;

	static Sprite* tex_title;
	static Sprite* tex_title_press_a;

public:
	static void Init();
	static void Update();
	static void PlayerUIDraw();
	static void EnemyUIDraw();
	static void DiedDraw();
	static void TitleDraw(bool& isSceneChange);

	static void PlayerMpAndStaminaUpdate(float mpRate, float staminaRate, bool isHealdelay);
	static void PlayerDamaged(float hpRate);
	static void EnemyDamaged(float hpRate);

	static bool IsSceneChangeEnd() { return m_isSceneChangeEnd; }
};

