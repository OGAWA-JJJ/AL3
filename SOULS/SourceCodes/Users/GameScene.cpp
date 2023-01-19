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
	m_sceneType = GameSceneType::TITLE;
	m_nextSceneType = m_sceneType;
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
	m_sceneType = GameSceneType::TITLE;
	m_nextSceneType = m_sceneType;
	m_sceneChangeTri = false;

	m_player->Init();
	m_enemy->Init();
	m_stage->Init();
	SpriteManager::Init();

	m_moveTimer = C_MAX_MOVE_TIMER;
	m_moveTrigger = false;
	ImguiControl::Imgui_gameInit = false;

	ImguiControl::Imgui_shadowEye_x = light->GetShadowLigitEye().x;
	ImguiControl::Imgui_shadowEye_y = light->GetShadowLigitEye().y;
	ImguiControl::Imgui_shadowEye_z = light->GetShadowLigitEye().z;
}

void GameScene::Update()
{
	SpriteManager::SceneTransUpdate();
	m_sceneType = m_nextSceneType;

	//Json
	if (ImguiControl::Imgui_export)
	{
		m_enemy->ExportJson();
	}
	if (ImguiControl::Imgui_inport)
	{
		m_enemy->InportJson();
	}
	if (ImguiControl::Imgui_addMenbers)
	{
		m_player->AddMenbers();
		m_enemy->AddMenbers();
	}

	switch (m_sceneType)
	{
	case TITLE:
	{
		//Input
		if (!m_sceneChangeTri &&
			Input::isPadTrigger(XINPUT_GAMEPAD_A))
		{
			m_sceneChangeTri = true;
		}

		//Aが押されたか
		if (m_sceneChangeTri)
		{
			//タイトル画面が暗転したか←壁くぐったらに変更する必要がある
			if (SpriteManager::IsSceneChangeEnd())
			{
				if (!m_isTransTri)
				{
					m_isTransTri = true;
					SpriteManager::SceneTrans();
				}

				if (!SpriteManager::IsSceneTrans())
				{
					m_isTransTri = false;
					m_nextSceneType = BEFORE_BATTLE;
				}
			}
		}
		break;
	}
	case BEFORE_BATTLE:
	{
		//戦闘前カメラ(1回)
		if (m_enemy->BeforeBattleScene())
		{
			if (!m_isTransTri)
			{
				m_isTransTri = true;
				SpriteManager::SceneTrans();
			}
		}
		if (m_isTransTri && !SpriteManager::IsSceneTrans())
		{
			m_isTransTri = false;
			m_player->ResetCamera();
			m_nextSceneType = BATTLE;
		}

		LightUpdate();
		m_player->Update(m_enemy->GetPos(), false);
		m_enemy->Update(m_player->GetPos());
		m_stage->Update();
		break;
	}
	case BATTLE:
	{
		OtherUpdate();

		PlayerUpdate();
		EnemyUpdate();

		if (m_enemy->IsDeadAnimationEnd())
		{
			m_nextSceneType = AFTER_BATTLE;
		}

		m_player->Update(m_enemy->GetPos());
		m_player->CalcExpulsion(m_enemy->IsTackle());
		m_enemy->Update(m_player->GetPos());
		m_stage->Update();

		SpriteManager::Update();
		SpriteManager::PlayerHPUpdate(m_player->GetHpRate());
		SpriteManager::EnemyHPUpdate(m_enemy->GetHpRate());
		SpriteManager::PlayerMpAndStaminaUpdate(
			m_player->GetMpRate(),
			m_player->GetStaminaRate(),
			false
		);
		break;
	}
	case AFTER_BATTLE:
	{
		LightUpdate();
		m_player->Update(m_enemy->GetPos());
		m_enemy->Update(m_player->GetPos());
		m_stage->Update();
		m_stage->AfterBattleUpdate();
		break;
	}
	}
}

void GameScene::Draw()
{
	switch (m_sceneType)
	{
	case TITLE:
	{
		Sprite::PreDraw(DirectXImportant::cmdList.Get());
		SpriteManager::TitleDraw(m_sceneChangeTri);
		Sprite::PostDraw();
		break;
	}
	case BEFORE_BATTLE:
	{
		//3D
		m_stage->Draw();
		m_player->Draw();
		m_enemy->Draw();
		break;
	}
	case BATTLE:
	{
		//3D
		m_stage->Draw();
		m_player->Draw();
		m_enemy->Draw();

		//2D
		Sprite::PreDraw(DirectXImportant::cmdList.Get());

		//SpriteManager::PlayerUIDraw(m_player->GetEstus());
		if (m_enemy->IsFighting())
		{
			SpriteManager::EnemyUIDraw();
		}
		if (m_player->IsDead())
		{
			SpriteManager::DiedDraw();
		}

		Sprite::PostDraw();
		break;
	}
	case AFTER_BATTLE:
	{
		//3D
		m_stage->Draw();
		m_stage->AfterBattleDraw();
		m_player->Draw();
		m_enemy->Draw();
		break;
	}
	}
}

void GameScene::LuminanceDraw()
{
	if (m_sceneType != TITLE)
	{
		m_player->LuminanceDraw();
		m_enemy->LuminanceDraw();
	}
}

void GameScene::ShadowDraw()
{
	if (m_sceneType != TITLE)
	{
		m_player->ShadowDraw();
		m_stage->ShadowDraw();
		m_enemy->ShadowDraw();
	}
}

void GameScene::ShaftOfLightDraw()
{
	m_stage->ShaftOfLightDraw();
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

	//爆発
	if (m_enemy->IsExplosion() && !m_player->IsInvincible())
	{
		float dist = OgaJHelper::CalcDist(m_player->GetPos(), m_enemy->GetPos());
		if (dist < 175.0f)
		{
			m_player->HitAttack(m_enemy->GetExplosionPower());
		}
	}

	//レーザー
	if (!m_player->IsInvincible())
	{
		if (m_enemy->IsRazerHit())
		{
			m_player->HitAttack(100);
		}
	}
}

void GameScene::OtherUpdate()
{
	//敵起動
	if (m_moveTimer > 0)
	{
		m_moveTimer--;
	}
	else
	{
		if (!m_moveTrigger)
		{
			m_enemy->DiscoverPlayer();
		}
		m_moveTrigger = true;
	}

	//GameInit
	if (ImguiControl::Imgui_gameInit)
	{
		Init();
	}
	if (ImguiControl::Imgui_enemyKill)
	{
		m_enemy->SelfKill();
		SpriteManager::EnemyHPChange();
	}

	LightUpdate();
}

void GameScene::LightUpdate()
{
	//Light系
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

	light->SetShadowLigitEye(DirectX::XMFLOAT3(
		ImguiControl::Imgui_shadowEye_x,
		ImguiControl::Imgui_shadowEye_y,
		ImguiControl::Imgui_shadowEye_z
	));

	light->Update();

	//Light情報を更新
	Object::SetLight(light);
	FbxObjects::SetLight(light);
}
