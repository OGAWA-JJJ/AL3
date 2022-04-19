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
	XMFLOAT2 p;	//ˆÊ’u
	XMFLOAT2 v;	//‘¬“x
	XMFLOAT2 a;	//‰Á‘¬“x
	XMFLOAT2 f;	//“­‚­—Í
	float m;	//Ž¿—Ê
	float t;	//ŽžŠÔ

private:
	Light* light = nullptr;

	Model* modelA = nullptr;
	Model* modelB = nullptr;

	Object* objA = nullptr;
	Object* objB = nullptr;

	FbxModel* fbxModel1 = nullptr;
	FbxObject3D* fbxObj1 = nullptr;

	Sprite* GH1;

public:
	GameScene();
	~GameScene();
	void Init();
	void Update();
	void Draw();
};