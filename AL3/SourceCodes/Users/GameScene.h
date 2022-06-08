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
	const float MAX_DISTANCE = 125.0f;
	const float MAX_MOVE_SPEED = 2.0f;
	const float MAX_CAMERA_MOVE_SPEED = 2.0f;
	const float EASE_CAMERA_TIMER = 0.006f;

private:
	XMFLOAT3 cameraAngle;
	float cameraMoveEase = 0.0f;
	float cameraY = 0.0f;
	bool isTarget = false;
	bool isEase = false;

private:
	Light* light = nullptr;

	Model* modelA = nullptr;
	Object* objA = nullptr;

	Model* modelB = nullptr;
	Object* objB = nullptr;

	Model* modelC = nullptr;
	Object* objC = nullptr;

	Model* modelD = nullptr;
	Object* objD = nullptr;

	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObj1 = nullptr;

	Sprite* GH1 = nullptr;

private:
	Object* obj1 = nullptr;
	Object* obj2 = nullptr;
	Object* obj3 = nullptr;
	Object* obj4 = nullptr;
	Object* obj5 = nullptr;

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
	void LuminanceDraw();
};