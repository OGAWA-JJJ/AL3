#include "GameScene.h"

GameScene::GameScene()
{
	timer = 0;
	waterCount = 0;
	randCount = 0;
	score = 0;
	side = rand() % 2;
	miss = false;
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
	timer = 0;
	waterCount = 0;
	randCount = 0;
	score = 0;
	side = rand() % 2;
	miss = false;
}

void GameScene::Update()
{
	if (No == Title) {

		//タイトル入力

	}

	else if (No == GamePlay) {

		if (Input::isKeyTrigger(DIK_A)) {
			//失敗
			if (side == !Right) { miss = true; }
			//成功
			else {
				waterCount--;
				score += SCORE;
				side = Left;
			}
		}

		if (Input::isKeyTrigger(DIK_D)) {
			//失敗
			if (side == !Left) { miss = true; }
			//成功
			else {
				waterCount--;
				score += SCORE;
				side = Right;
			}
		}

		if (Input::isKeyTrigger(DIK_SPACE)) {
			//失敗
			if (waterCount != 0) {}
			//成功
			else {}
		}

		//シーン管理
		if (miss) { No = Result; }

		//時間管理
		if (timer <= MAX_TIME) { timer++; }
		else { No = Result; }

	}

	else if (No == Result) {

		//スコア表示とか

	}
}

void GameScene::Draw()
{
}
