#include "GameScene.h"
#include "../DirectX/Camera.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/DirectXImportant.h"
#include "PipelineManager.h"
#include "../Math/OgaJHelper.h"
#include "../Math/OBBCollision.h"
#include "SpriteManager.h"
#include "../Input/Input.h"
#include "ModelManager.h"

GameScene::GameScene()
{
	m_gameSceneType = GAME;
	m_sceneChangeTri = false;

	PipelineManager::Init();

	light = Light::Create();
	light->SetLightColor(
		{
			ImguiControl::Imgui_lightColor_r,
			ImguiControl::Imgui_lightColor_g,
			ImguiControl::Imgui_lightColor_b
		});
	light->SetLightDir(
		{
			ImguiControl::Imgui_lightDir_x,
			ImguiControl::Imgui_lightDir_y,
			ImguiControl::Imgui_lightDir_z,
		});

	Object::SetLight(light);
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
	m_player->Init();
	m_enemy->Init();
	m_stage->Init();

	//Camera
	DirectX::XMFLOAT3 enemyToPlayer = OgaJHelper::CalcDirectionVec3(m_enemy->GetPos(), m_player->GetPos());
	enemyToPlayer = OgaJHelper::CalcNormalizeVec3(enemyToPlayer);

	Camera::SetEye(DirectX::XMFLOAT3(
		m_player->GetPos().x + enemyToPlayer.x * m_player->GetCameraDist(),
		50.0f,
		m_player->GetPos().z + enemyToPlayer.z * m_player->GetCameraDist()));

	DirectX::XMFLOAT3 hoge = Camera::GetEye();
}

void GameScene::Update()
{
	if (m_sceneChangeTri)
	{
		if (SpriteManager::IsSceneChangeEnd())
		{
			m_gameSceneType = GAME;
		}
	}

	if (m_gameSceneType == TITLE)
	{
		if (Input::isPadTrigger(XINPUT_GAMEPAD_A))
		{
			m_sceneChangeTri = true;
		}
	}
	else if (m_gameSceneType == GAME)
	{
		light->SetLightColor(
			{
				ImguiControl::Imgui_lightColor_r,
				ImguiControl::Imgui_lightColor_g,
				ImguiControl::Imgui_lightColor_b
			});
		light->SetLightDir(
			{
				ImguiControl::Imgui_lightDir_x,
				ImguiControl::Imgui_lightDir_y,
				ImguiControl::Imgui_lightDir_z,
			});
		light->Update();

		//Light情報を更新
		Object::SetLight(light);
		FbxObjects::SetLight(light);

		//敵起動
		if (m_moveTimer > 0) { m_moveTimer--; }
		else
		{
			if (!m_moveTrigger) { m_enemy->DiscoverPlayer(); }
			m_moveTrigger = true;
		}

		PlayerUpdate();
		EnemyUpdate();

		if (m_enemy->IsExplosion() && !m_player->IsInvincible())
		{
			float dist = OgaJHelper::CalcDist(m_player->GetPos(), m_enemy->GetPos());
			if (dist < 175.0f)
			{
				m_player->HitAttack(m_enemy->GetExplosionPower());
			}
		}

		m_player->Update(m_enemy->GetPos());
		m_enemy->Update(m_player->GetPos());
		m_stage->Update();

		//死亡判定(SceneChange用)
		bool isDead = m_player->IsDead();
		isDead = m_enemy->IsDead();

		SpriteManager::Update();
		SpriteManager::PlayerHPUpdate(m_player->GetHpRate());
		SpriteManager::PlayerMpAndStaminaUpdate(
			m_player->GetMpRate(),
			m_player->GetStaminaRate(),
			false
		);
	}
}

void GameScene::Draw()
{
	if (m_gameSceneType == TITLE)
	{
		SpriteManager::TitleDraw(m_sceneChangeTri);
	}
	else if (m_gameSceneType == GAME)
	{
		m_player->Draw();
		m_enemy->Draw();
		m_stage->Draw();
		SpriteManager::PlayerUIDraw(m_player->GetEstus());

		if (m_enemy->IsFighting())
		{
			SpriteManager::EnemyUIDraw();
		}
		if (m_player->IsDead())
		{
			SpriteManager::DiedDraw();
		}
	}
}

void GameScene::LuminanceDraw()
{
	if (m_gameSceneType == GAME)
	{
		m_player->LuminanceDraw();
		m_enemy->LuminanceDraw();
	}
}

void GameScene::ShadowDraw()
{
	if (m_gameSceneType == GAME)
	{
		m_player->ShadowDraw();
		m_stage->ShadowDraw();
	}
}

void GameScene::PlayerUpdate()
{
	//プレイヤーの攻撃時判定
	if (m_player->IsAttack())
	{
		//敵が無敵じゃなかったら判定
		if (!m_enemy->IsInvincible())
		{
			std::vector<OBB> l_obbs = m_enemy->GetOBBs();
			OBB l_obb = m_player->GetSwordOBB();

			//全ボーンと計算(変えたい)→メッシュだと部位特定だるそう
			for (int i = 0; i < l_obbs.size(); i++)
			{
				bool l_isHit = OBBCollision::CollisionOBBs(l_obb, l_obbs[i]);
				if (l_isHit)
				{
					m_enemy->SetHitOBBNum(i);
					m_enemy->HitAttack(m_player->GetPower());
					SpriteManager::EnemyDamaged(m_enemy->GetHpRate());
					break;
				}
			}
		}
	}
	else
	{
		//多重ヒット回避
		m_enemy->UnInvincible();
	}
}

void GameScene::EnemyUpdate()
{
	//敵の攻撃時判定
	if (m_enemy->IsAttack() && m_enemy->IsCalc())
	{
		if (!m_player->IsInvincible())
		{
			std::vector<OBB> l_eObbs = m_enemy->GetAttackOBBs();
			std::vector<OBB> l_pObbs = m_player->GetOBBs();

			for (int i = 0; i < l_eObbs.size(); i++)
			{
				for (int j = 0; j < l_pObbs.size(); j++)
				{
					bool l_isHit = OBBCollision::CollisionOBBs(l_eObbs[i], l_pObbs[j]);
					if (l_isHit)
					{
						m_player->HitAttack(m_enemy->GetPower());

						//まだ空←入れた
						SpriteManager::PlayerDamaged(m_player->GetHpRate());
						return;
					}
				}
			}
		}
	}
	else if (!m_enemy->IsAttack())
	{
		//m_player->UnInvincible();
	}
}
