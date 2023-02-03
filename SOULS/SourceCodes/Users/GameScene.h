#pragma once
#include "../2D/Sprite.h"
#include "../XAudio2/Music.h"
#include "Player.h"
#include "Enemy.h"
#include "Stage.h"
#include "StageObject.h"
#include <wrl.h>

class GameScene
{
private:
	enum GameSceneType
	{
		TITLE, BEFORE_BATTLE, BATTLE, AFTER_BATTLE
	};

private:
	const int C_MAX_MOVE_TIMER = 30;

private:
	std::shared_ptr<Light> light = nullptr;

private:
	std::unique_ptr<Player> m_player = std::make_unique<Player>();
	std::unique_ptr<Enemy> m_enemy = std::make_unique<Enemy>();
	std::unique_ptr<Stage> m_stage = std::make_unique<Stage>();

private:
	int m_sceneType;
	int m_nextSceneType;
	bool m_sceneChangeTri;

	int m_moveTimer;		//çÏÇÍÅI
	bool m_moveTrigger = false;

	bool m_isTransTri = false;

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
	void LuminanceDraw();	//ãPìxíäèoóp
	void ShadowDraw();		//âeï`âÊóp
	void ShaftOfLightDraw();

private:
	void PlayerUpdate();
	void EnemyUpdate();
	void OtherUpdate();
	void LightUpdate();
};