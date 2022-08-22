#include "SpriteManager.h"
#include "../DirectX/DirectXImportant.h"

float SpriteManager::C_ENEMY_HP_BAR_SIZE_X = 504.0f;
float SpriteManager::C_ENEMY_HP_BAR_SIZE_Y = 24.0f;
int SpriteManager::C_YELLOW_MOVE_DELAY = 60;

int SpriteManager::m_yellowCount;
float SpriteManager::m_hpRate = 0.0f;
float SpriteManager::m_diedAlpha = 0.0f;
bool SpriteManager::m_enemyDamaged;

Sprite* SpriteManager::tex_numbers[] = { nullptr };
Sprite* SpriteManager::tex_hp_red = nullptr;
Sprite* SpriteManager::tex_hp_yellow = nullptr;
Sprite* SpriteManager::tex_hp_back = nullptr;
Sprite* SpriteManager::tex_hp_flame = nullptr;

Sprite* SpriteManager::tex_enemy_name = nullptr;
Sprite* SpriteManager::tex_died = nullptr;

void SpriteManager::Init()
{
	//ï™ÇØÇ»Ç¢Ç∆ÉãÅ[ÉvéûÇ…ÇæÇÈÇ¢Ç©Ç‡
	m_yellowCount = 0;
	m_enemyDamaged = false;

	Sprite::LoadTexture(0, L"Resources/numbers/number_0.png");
	Sprite::LoadTexture(1, L"Resources/numbers/number_1.png");
	Sprite::LoadTexture(2, L"Resources/numbers/number_2.png");
	Sprite::LoadTexture(3, L"Resources/numbers/number_3.png");
	Sprite::LoadTexture(4, L"Resources/numbers/number_4.png");
	Sprite::LoadTexture(5, L"Resources/numbers/number_5.png");
	Sprite::LoadTexture(6, L"Resources/numbers/number_6.png");
	Sprite::LoadTexture(7, L"Resources/numbers/number_7.png");
	Sprite::LoadTexture(8, L"Resources/numbers/number_8.png");
	Sprite::LoadTexture(9, L"Resources/numbers/number_9.png");

	Sprite::LoadTexture(10, L"Resources/hps/hp_bar_red.png");
	//Sprite::LoadTexture(10, L"Resources/hamus.png");
	Sprite::LoadTexture(11, L"Resources/hps/hp_bar_yellow.png");
	Sprite::LoadTexture(12, L"Resources/hps/hp_bar_back.png");
	Sprite::LoadTexture(13, L"Resources/hps/hp_bar_flame.png");
	Sprite::LoadTexture(14, L"Resources/hps/name.png");
	Sprite::LoadTexture(15, L"Resources/hps/you_died.png");

	for (int i = 0; i < 10; i++)
	{
		tex_numbers[i] = Sprite::Create(i, DirectX::XMFLOAT2(0, 0));
	}

	const float hp_bar_width = 320.0f;
	const float hp_bar_height = 620.0f;
	const float diff = 67.0f;
	tex_hp_red = Sprite::Create(10, DirectX::XMFLOAT2(hp_bar_width + diff, hp_bar_height));
	tex_hp_yellow = Sprite::Create(11, DirectX::XMFLOAT2(hp_bar_width + diff, hp_bar_height));
	tex_hp_back = Sprite::Create(12, DirectX::XMFLOAT2(hp_bar_width + diff, hp_bar_height));
	tex_hp_flame = Sprite::Create(13, DirectX::XMFLOAT2(hp_bar_width, hp_bar_height - 20.0f));

	tex_enemy_name = Sprite::Create(14, DirectX::XMFLOAT2(608.0f, 580.0f));
	tex_died = Sprite::Create(15, DirectX::XMFLOAT2(0, 315.0f));
	tex_died->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));
	m_diedAlpha = 0.0f;
}

void SpriteManager::EnemyUIDraw()
{
	Sprite::PreDraw(DirectXImportant::cmdList.Get());
	tex_hp_back->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f));
	tex_hp_back->Draw();
	tex_hp_yellow->Draw();
	tex_hp_red->Draw();
	tex_hp_flame->Draw();

	tex_enemy_name->Draw();
	Sprite::PostDraw();
}

void SpriteManager::DiedDraw()
{
	tex_died->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, m_diedAlpha));
	Sprite::PreDraw(DirectXImportant::cmdList.Get());
	tex_died->Draw();
	Sprite::PostDraw();

	if (m_diedAlpha < 0.9f) { m_diedAlpha += 0.01f; }
}

void SpriteManager::Update()
{
	if (m_enemyDamaged)
	{
		if (m_yellowCount < C_YELLOW_MOVE_DELAY)
		{
			m_yellowCount++;
		}
		else
		{
			tex_hp_yellow->SetSize(DirectX::XMFLOAT2(
				m_hpRate,
				C_ENEMY_HP_BAR_SIZE_Y));

			m_yellowCount = 0;
			m_enemyDamaged = false;
		}
	}
}

void SpriteManager::PlayerDamaged(float hpRate)
{

}

void SpriteManager::EnemyDamaged(float hpRate)
{
	if (!m_enemyDamaged)
	{
		m_hpRate = C_ENEMY_HP_BAR_SIZE_X * hpRate;

		tex_hp_red->SetSize(DirectX::XMFLOAT2(
			m_hpRate,
			C_ENEMY_HP_BAR_SIZE_Y));
		m_enemyDamaged = true;
	}
	else
	{
		m_hpRate = C_ENEMY_HP_BAR_SIZE_X * hpRate;

		tex_hp_red->SetSize(DirectX::XMFLOAT2(
			m_hpRate,
			C_ENEMY_HP_BAR_SIZE_Y));

		m_yellowCount = 0;
	}
}
