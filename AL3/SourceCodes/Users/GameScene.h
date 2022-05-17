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
	const float g = 9.8f;

private:
	XMFLOAT2 p;	//位置
	XMFLOAT2 v;	//速度
	XMFLOAT2 a;	//加速度
	XMFLOAT2 f;	//働く力
	float m;	//質量
	float t;	//時間

private:
	Light* light = nullptr;

	Model* modelA = nullptr;
	Model* modelB = nullptr;

	Object* objA = nullptr;
	Object* objB = nullptr;

	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObj1 = nullptr;

	Sprite* GH1;
	Sprite* GH2;

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
};