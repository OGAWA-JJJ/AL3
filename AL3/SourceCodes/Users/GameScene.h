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
	Light* light = nullptr;

private:
	std::unique_ptr<Player> l_player = std::make_unique<Player>();
	std::unique_ptr<Enemy> l_enemy = std::make_unique<Enemy>();
	std::unique_ptr<Stage> l_stage = std::make_unique<Stage>();
	std::unique_ptr<StageObject> l_stageObj = std::make_unique<StageObject>();

public:
	GameScene();
	~GameScene();
	void Init(ID3D12Resource* texbuff = nullptr);
	void Update();
	void Draw();
	void LuminanceDraw();	//ãPìxíäèoóp
	void ShadowDraw();		//âeï`âÊóp
};