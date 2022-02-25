#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../3D/Model.h"
#include "../3D/Shape.h"
#include "../XAudio2/Music.h"
#include "../3D/FbxObject3D.h"

class GameScene
{
private:
	Light* light = nullptr;

	Model* modelA = nullptr;
	Model* modelB = nullptr;

	Object* objA = nullptr;
	Object* objB = nullptr;

	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObj1 = nullptr;

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
};