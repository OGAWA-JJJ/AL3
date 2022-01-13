#pragma once
#include "../2D/DirectX2D.h"
#include "../3D/Object3D.h"
#include "../3D/Shape.h"
#include "../XAudio2/Music.h"

const int MAX_TIME = 30 * 60;
const int SCORE = 100;

class GameScene
{
private:
	int No;
	enum Scene
	{
		Title, GamePlay, Result
	};

private:
	enum Side
	{
		Left, Right
	};
	int timer;
	int waterCount;
	int randCount;
	int score;
	bool side;
	bool miss;

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
};