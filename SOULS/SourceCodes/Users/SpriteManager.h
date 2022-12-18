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
	static std::shared_ptr<Sprite> tex_numbers[10];

	static std::shared_ptr<Sprite> tex_enemy_hp_red;
	static std::shared_ptr<Sprite> tex_enemy_hp_yellow;
	static std::shared_ptr<Sprite> tex_enemy_hp_back;
	static std::shared_ptr<Sprite> tex_enemy_hp_flame;

	static std::shared_ptr<Sprite> tex_enemy_name;
	static std::shared_ptr<Sprite> tex_died;

	static std::shared_ptr<Sprite> tex_player_hp_red;
	static std::shared_ptr<Sprite> tex_player_hp_yellow;
	static std::shared_ptr<Sprite> tex_player_hp_back;

	static std::shared_ptr<Sprite> tex_player_mp_blue;
	static std::shared_ptr<Sprite> tex_player_mp_yellow;
	static std::shared_ptr<Sprite> tex_player_mp_back;

	static std::shared_ptr<Sprite> tex_player_stamina_green;
	static std::shared_ptr<Sprite> tex_player_stamina_yellow;
	static std::shared_ptr<Sprite> tex_player_stamina_back;

	static std::shared_ptr<Sprite> tex_title;
	static std::shared_ptr<Sprite> tex_title_press_a;

	static std::shared_ptr<Sprite> tex_estus;

public:
	static void StaticInit();
	static void Init();
	static void Update();
	static void PlayerUIDraw(const int estusNum);
	static void EnemyUIDraw();
	static void DiedDraw();
	static void TitleDraw(bool& isSceneChange);

	static void PlayerMpAndStaminaUpdate(float mpRate, float staminaRate, bool isHealdelay);
	static void PlayerDamaged(float hpRate);
	static void EnemyDamaged(float hpRate);

	static void PlayerHPUpdate(float hpRate);

	static bool IsSceneChangeEnd() { return m_isSceneChangeEnd; }
};

