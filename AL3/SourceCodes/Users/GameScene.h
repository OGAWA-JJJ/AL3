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
	//const float MAX_DISTANCE = 125.0f;
	//const float MAX_MOVE_SPEED = 2.0f;
	//const float MAX_CAMERA_MOVE_SPEED = 0.02f;

private:
	//XMFLOAT3 cameraAngle;

private:
	//Light* light = nullptr;

	//Model* modelA = nullptr;
	//Object* objA = nullptr;

	//Model* modelB = nullptr;
	//Object* objB = nullptr;

	//Model* modelC = nullptr;
	//Object* objC = nullptr;

	//FbxModel* fbxModel1 = nullptr;
	//FbxObject3D* fbxObj1 = nullptr;

	Sprite* GH1 = nullptr;
	Sprite* GH2 = nullptr;
	Sprite* GH3 = nullptr;

private:	//MT4
	float dist2 = 250.0f;
	XMFLOAT2 radian = { 0,0 };

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
};