#pragma once
#include "GameScene.h"

class Hantei
{
public:
	Hantei();
	~Hantei();
	void Update();
	void Draw();

private:
	bool isHit;

	Sprite* GH = nullptr;

	Light* light = nullptr;

	Model* modelA = nullptr;
	Model* modelB = nullptr;

	Object* objA = nullptr;
	Object* objB = nullptr;
};
