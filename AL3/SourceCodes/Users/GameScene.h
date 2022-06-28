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
	enum AnimationType
	{
		STAND, SLOWRUN, RUN
	};

private:
	const float MAX_DISTANCE = 125.0f;
	const float MAX_MOVE_SPEED = 2.0f;
	const float MAX_CAMERA_MOVE_SPEED = 2.0f;
	const float EASE_CAMERA_TIMER = 0.006f;

private:
	PipelineSet pipelineSet;
	PipelineSet shadow;
	PipelineSet shadow2;

	XMFLOAT3 cameraAngle;
	float cameraMoveEase = 0.0f;
	float cameraY = 0.0f;
	bool isTarget = false;
	bool isEase = false;

	int animationType;

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

	FbxModel* fbxModel2 = nullptr;
	FbxObject3D* fbxObj2 = nullptr;

	FbxModel* fbxModel3 = nullptr;
	FbxObject3D* fbxObj3 = nullptr;

	Sprite* GH1 = nullptr;

	Model* sword = nullptr;
	Object* objSword = nullptr;

	FbxObject3D* fbx1 = nullptr;
	FbxObject3D* fbx2 = nullptr;
	FbxObject3D* fbx3 = nullptr;

public:
	GameScene();
	~GameScene();
	void Init(ID3D12Resource* texbuff = nullptr);
	void Update();
	void Draw(ID3D12Resource* texbuff = nullptr);
	void LuminanceDraw();
	void ShadowDraw();
};