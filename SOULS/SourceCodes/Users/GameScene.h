#pragma once
#include "../2D/Sprite.h"
#include "../XAudio2/Music.h"
#include "Player.h"
#include "Enemy.h"
#include "Stage.h"
#include "StageObject.h"

class GameScene
{
private:
	enum GameSceneType
	{
		TITLE, GAME, RESULT
	};

private:
	Light* light = nullptr;

private:
	std::unique_ptr<Player> m_player = std::make_unique<Player>();
	std::unique_ptr<Enemy> m_enemy = std::make_unique<Enemy>();
	std::unique_ptr<Stage> m_stage = std::make_unique<Stage>();
	std::unique_ptr<StageObject> m_stageObj = std::make_unique<StageObject>();

private:
	int m_gameSceneType;
	bool m_sceneChangeTri;

	int m_moveTimer = 120000;		//���I
	bool m_moveTrigger = false;

public:
	GameScene();
	~GameScene();
	void Init(ID3D12Resource* texbuff = nullptr);
	void Update();
	void Draw();
	void LuminanceDraw();	//�P�x���o�p
	void ShadowDraw();		//�e�`��p

private:
	void PlayerUpdate();
	void EnemyUpdate();
};