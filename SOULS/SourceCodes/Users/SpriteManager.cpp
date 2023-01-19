#include "SpriteManager.h"
#include "../DirectX/DirectXImportant.h"

float SpriteManager::C_ENEMY_HP_BAR_SIZE_X = 504.0f;
float SpriteManager::C_ENEMY_HP_BAR_SIZE_Y = 24.0f;

float SpriteManager::C_PLAYER_HP_BAR_SIZE_X = 252.0f;
float SpriteManager::C_PLAYER_HP_BAR_SIZE_Y = 12.0f;

float SpriteManager::C_PLAYER_MP_BAR_SIZE_X = 200.0f;
float SpriteManager::C_PLAYER_STAMINA_BAR_SIZE_X = 220.0f;

int SpriteManager::C_YELLOW_MOVE_DELAY = 60;
int SpriteManager::C_TRANS_DELAY = 90;

DirectX::XMFLOAT4 SpriteManager::m_texColor = { 0,0,0,1 };
int SpriteManager::m_enemy_yellowCount = 0;
int SpriteManager::m_player_yellowCount = 0;
int SpriteManager::m_trans_conut = 0;
float SpriteManager::m_enemy_hpRate = 0.0f;
float SpriteManager::m_player_hpRate = 0.0f;
float SpriteManager::m_diedAlpha = 0.0f;
bool SpriteManager::m_enemyDamaged = false;
bool SpriteManager::m_playerDamaged = false;
bool SpriteManager::m_isStart = false;
bool SpriteManager::m_isSceneChangeEnd = false;
bool SpriteManager::m_isTrans = false;
bool SpriteManager::m_isTransUpdate = false;

std::shared_ptr<Sprite> SpriteManager::tex_numbers[] = { nullptr };
std::shared_ptr<Sprite> SpriteManager::tex_enemy_hp_red = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_enemy_hp_yellow = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_enemy_hp_back = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_enemy_hp_flame = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_enemy_name = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_died = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_player_hp_red = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_player_hp_yellow = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_player_hp_back = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_player_mp_blue = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_player_mp_yellow = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_player_mp_back = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_player_stamina_green = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_player_stamina_yellow = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_player_stamina_back = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_title = nullptr;
std::shared_ptr<Sprite> SpriteManager::tex_title_press_a = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_estus = nullptr;

std::shared_ptr<Sprite> SpriteManager::tex_trans = nullptr;

void SpriteManager::StaticInit()
{
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

	Sprite::LoadTexture(10, L"Resources/UIs/hp_bar_red.png");
	Sprite::LoadTexture(11, L"Resources/UIs/hp_bar_yellow.png");
	Sprite::LoadTexture(12, L"Resources/UIs/hp_bar_back.png");
	Sprite::LoadTexture(13, L"Resources/UIs/hp_bar_flame.png");
	Sprite::LoadTexture(14, L"Resources/UIs/name.png");
	Sprite::LoadTexture(15, L"Resources/UIs/you_died.png");

	Sprite::LoadTexture(16, L"Resources/UIs/mp_bar_blue.png");
	Sprite::LoadTexture(17, L"Resources/UIs/stamina_bar_green.png");
	Sprite::LoadTexture(18, L"Resources/UIs/title.png");
	Sprite::LoadTexture(19, L"Resources/UIs/press_a.png");

	Sprite::LoadTexture(20, L"Resources/UIs/estus.png");

	Sprite::LoadTexture(21, L"Resources/UIs/trans.png");

	for (int i = 0; i < 10; i++)
	{
		tex_numbers[i] = Sprite::Create(i, DirectX::XMFLOAT2(108, 660));
		tex_numbers[i]->SetSize(DirectX::XMFLOAT2(32, 32));
	}

	//�G�摜������
	const float l_enemy_hp_bar_x = 320.0f;
	const float l_enemy_hp_bar_y = 620.0f;
	const float l_diff = 67.0f;
	tex_enemy_hp_red = Sprite::Create(10, DirectX::XMFLOAT2(l_enemy_hp_bar_x + l_diff, l_enemy_hp_bar_y));
	tex_enemy_hp_yellow = Sprite::Create(11, DirectX::XMFLOAT2(l_enemy_hp_bar_x + l_diff, l_enemy_hp_bar_y));
	tex_enemy_hp_back = Sprite::Create(12, DirectX::XMFLOAT2(l_enemy_hp_bar_x + l_diff, l_enemy_hp_bar_y));
	tex_enemy_hp_flame = Sprite::Create(13, DirectX::XMFLOAT2(l_enemy_hp_bar_x, l_enemy_hp_bar_y - 20.0f));

	tex_enemy_name = Sprite::Create(14, DirectX::XMFLOAT2(608.0f, 580.0f));
	tex_died = Sprite::Create(15, DirectX::XMFLOAT2(0, 315.0f));
	tex_died->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));

	//���@�摜������(�T�C�Y��ς���K�v�A��)
	const float l_player_hp_bar_x = 20.0f;
	const float l_player_hp_bar_y = 20.0f;
	const float l_player_diff = 20.0f;
	tex_player_hp_red = Sprite::Create(10, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y));
	tex_player_hp_yellow = Sprite::Create(11, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y));
	tex_player_hp_back = Sprite::Create(12, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y));
	tex_player_hp_red->SetSize(DirectX::XMFLOAT2(C_PLAYER_HP_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));
	tex_player_hp_yellow->SetSize(DirectX::XMFLOAT2(C_PLAYER_HP_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));
	tex_player_hp_back->SetSize(DirectX::XMFLOAT2(C_PLAYER_HP_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));

	tex_player_mp_blue = Sprite::Create(16, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y + l_player_diff));
	tex_player_mp_yellow = Sprite::Create(11, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y + l_player_diff));
	tex_player_mp_back = Sprite::Create(12, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y + l_player_diff));
	tex_player_mp_blue->SetSize(DirectX::XMFLOAT2(C_PLAYER_MP_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));
	tex_player_mp_yellow->SetSize(DirectX::XMFLOAT2(C_PLAYER_MP_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));
	tex_player_mp_back->SetSize(DirectX::XMFLOAT2(C_PLAYER_MP_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));

	tex_player_stamina_green = Sprite::Create(17, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y + l_player_diff * 2));
	tex_player_stamina_yellow = Sprite::Create(11, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y + l_player_diff * 2));
	tex_player_stamina_back = Sprite::Create(12, DirectX::XMFLOAT2(l_player_hp_bar_x, l_player_hp_bar_y + l_player_diff * 2));
	tex_player_stamina_green->SetSize(DirectX::XMFLOAT2(C_PLAYER_STAMINA_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));
	tex_player_stamina_yellow->SetSize(DirectX::XMFLOAT2(C_PLAYER_STAMINA_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));
	tex_player_stamina_back->SetSize(DirectX::XMFLOAT2(C_PLAYER_STAMINA_BAR_SIZE_X, C_PLAYER_HP_BAR_SIZE_Y));

	tex_title = Sprite::Create(18, DirectX::XMFLOAT2(0, 0));
	tex_title->SetColor(DirectX::XMFLOAT4(0, 0, 0, 1));

	tex_title_press_a = Sprite::Create(19, DirectX::XMFLOAT2(469, 574));

	tex_estus = Sprite::Create(20, DirectX::XMFLOAT2(48, 572));
	tex_estus->SetSize(DirectX::XMFLOAT2(72, 128));

	tex_trans = Sprite::Create(21, DirectX::XMFLOAT2(0, 0));
}

void SpriteManager::Init()
{
	//�����Ȃ��ƃ��[�v���ɂ��邢����
	m_enemy_yellowCount = 0;
	m_player_yellowCount = 0;
	m_enemyDamaged = false;
	m_playerDamaged = false;
	m_diedAlpha = 0.0f;
}

void SpriteManager::TitleDraw(bool isSceneChange)
{
	//�A���t�@�l�ύX����
	bool l_isAlphaMax = false;
	if (!isSceneChange)
	{
		if (m_texColor.x < 1.0f)
		{
			m_texColor.x += 0.01f;
			m_texColor.y += 0.01f;
			m_texColor.z += 0.01f;
		}
		else { l_isAlphaMax = true; }
		tex_title->SetColor(m_texColor);
	}
	else
	{
		if (m_texColor.x > 0.0f)
		{
			m_texColor.x -= 0.01f;
			m_texColor.y -= 0.01f;
			m_texColor.z -= 0.01f;
		}
		else
		{
			m_isSceneChangeEnd = true;
		}
		tex_title->SetColor(m_texColor);
	}

	tex_title->Draw();
	if (l_isAlphaMax) { tex_title_press_a->Draw(); }
}

void SpriteManager::PlayerUIDraw(const int estusNum)
{
	tex_player_hp_back->Draw();
	tex_player_mp_back->Draw();
	tex_player_stamina_back->Draw();

	tex_player_hp_yellow->Draw();
	tex_player_mp_yellow->Draw();
	tex_player_stamina_yellow->Draw();

	tex_player_hp_red->Draw();
	tex_player_mp_blue->Draw();
	tex_player_stamina_green->Draw();

	tex_estus->Draw();
	tex_numbers[estusNum]->Draw();
}

void SpriteManager::EnemyUIDraw()
{
	tex_enemy_hp_back->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f));
	tex_enemy_hp_back->Draw();
	tex_enemy_hp_yellow->Draw();
	tex_enemy_hp_red->Draw();
	tex_enemy_hp_flame->Draw();

	tex_enemy_name->Draw();
}

void SpriteManager::DiedDraw()
{
	tex_died->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, m_diedAlpha));

	tex_died->Draw();

	if (m_diedAlpha < 0.9f) { m_diedAlpha += 0.01f; }
}

/*----------�`�揈���ȊO----------*/
void SpriteManager::Update()
{
	//�G
	if (m_enemyDamaged)
	{
		if (m_enemy_yellowCount < C_YELLOW_MOVE_DELAY)
		{
			m_enemy_yellowCount++;
		}
		else
		{
			tex_enemy_hp_yellow->SetSize(DirectX::XMFLOAT2(
				m_enemy_hpRate,
				C_ENEMY_HP_BAR_SIZE_Y));

			m_enemy_yellowCount = 0;
			m_enemyDamaged = false;
		}
	}

	//���@
	if (m_playerDamaged)
	{
		if (m_player_yellowCount < C_YELLOW_MOVE_DELAY)
		{
			m_player_yellowCount++;
		}
		else
		{
			//�����}�ɕς��
			tex_player_hp_yellow->SetSize(DirectX::XMFLOAT2(
				m_player_hpRate,
				C_PLAYER_HP_BAR_SIZE_Y));

			m_player_yellowCount = 0;
			m_playerDamaged = false;
		}
	}
}

void SpriteManager::PlayerMpAndStaminaUpdate(float mpRate, float staminaRate, bool isYellowUpdate)
{
	//���F���ォ�������ꍇ
	if (isYellowUpdate)
	{

	}

	//���F�Ɠ����Ɍ��炷�ꍇ
	else
	{
		float l_mpSize = C_PLAYER_MP_BAR_SIZE_X * mpRate;
		float l_staminaSize = C_PLAYER_STAMINA_BAR_SIZE_X * staminaRate;

		tex_player_mp_blue->SetSize(DirectX::XMFLOAT2(
			l_mpSize,
			C_PLAYER_HP_BAR_SIZE_Y));
		tex_player_mp_yellow->SetSize(DirectX::XMFLOAT2(
			l_mpSize,
			C_PLAYER_HP_BAR_SIZE_Y));

		tex_player_stamina_green->SetSize(DirectX::XMFLOAT2(
			l_staminaSize,
			C_PLAYER_HP_BAR_SIZE_Y));
		tex_player_stamina_yellow->SetSize(DirectX::XMFLOAT2(
			l_staminaSize,
			C_PLAYER_HP_BAR_SIZE_Y));
	}
}

void SpriteManager::PlayerDamaged(float hpRate)
{
	if (!m_playerDamaged)
	{
		m_playerDamaged = true;
		PlayerHPUpdate(hpRate);
	}
	else
	{
		m_player_yellowCount = 0;
		PlayerHPUpdate(hpRate);
	}
}

void SpriteManager::EnemyDamaged(float hpRate)
{
	if (!m_enemyDamaged)
	{
		m_enemyDamaged = true;
		EnemyHPUpdate(hpRate);
	}
	else
	{
		m_enemy_yellowCount = 0;
		EnemyHPUpdate(hpRate);
	}
}

void SpriteManager::PlayerHPUpdate(float hpRate)
{
	m_player_hpRate = C_PLAYER_HP_BAR_SIZE_X * hpRate;

	tex_player_hp_red->SetSize(DirectX::XMFLOAT2(
		m_player_hpRate,
		C_PLAYER_HP_BAR_SIZE_Y));

	if (m_player_yellowCount >= C_YELLOW_MOVE_DELAY)
	{
		tex_player_hp_yellow->SetSize(DirectX::XMFLOAT2(
			m_player_hpRate,
			C_PLAYER_HP_BAR_SIZE_Y));
	}
}

void SpriteManager::EnemyHPUpdate(float hpRate)
{
	m_enemy_hpRate = C_ENEMY_HP_BAR_SIZE_X * hpRate;

	tex_enemy_hp_red->SetSize(DirectX::XMFLOAT2(
		m_enemy_hpRate,
		C_ENEMY_HP_BAR_SIZE_Y));

	if (m_enemy_yellowCount >= C_YELLOW_MOVE_DELAY)
	{
		tex_enemy_hp_yellow->SetSize(DirectX::XMFLOAT2(
			m_enemy_hpRate,
			C_ENEMY_HP_BAR_SIZE_Y));
	}
}

void SpriteManager::SceneTransUpdate()
{
	if (m_isTransUpdate)
	{
		//��������
		if (m_trans_conut < C_TRANS_DELAY / 2)
		{
			float l_alpha =
				1.0f / (C_TRANS_DELAY / 2) * m_trans_conut;
			tex_trans->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, l_alpha));
			m_trans_conut++;
			if (m_trans_conut >= C_TRANS_DELAY / 2)
			{
				m_isTrans = false;
			}
		}
		//��������
		else if (m_trans_conut < C_TRANS_DELAY)
		{
			int l_halfDelay = C_TRANS_DELAY / 2;
			float l_alpha =
				1.0f - (1.0f / l_halfDelay * (m_trans_conut - l_halfDelay));
			tex_trans->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, l_alpha));
			m_trans_conut++;
		}
		//���Z�b�g
		else
		{
			tex_trans->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));
			m_trans_conut = 0;
			m_isTransUpdate = false;
			return;
		}
	}
}

void SpriteManager::SceneTransDraw()
{
	if (m_isTransUpdate)
	{
		Sprite::PreDraw(DirectXImportant::cmdList.Get());
		tex_trans->Draw();
		Sprite::PostDraw();
	}
}
