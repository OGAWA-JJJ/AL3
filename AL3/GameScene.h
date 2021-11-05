#pragma once
#include "Model3D.h"
#include "DirectX2D.h"
#include "Shape.h"
#include "Music.h"

const int MaxBulletNum = 20;
const int MaxCube = 5;
const float Speed = 5.0f;
//const int MaxEnemy = 5;
const int EnemyHP = 5;

class GameScene
{
private:
	static int No;
	static enum Scene
	{
		Title, GamePlay, Clear
	};

private:
	Model3D model3D;
	DirectX2D d[14];
	Shape NormalBullet[MaxBulletNum + MaxCube + 1];
	Music music;

	XMVECTOR V[MaxBulletNum];
	XMFLOAT3 F[MaxBulletNum];
	int EnemyHP[MaxCube];
	bool BulletFlag[MaxBulletNum] = { false };
	float BulletCount = 0.5f;
	float DrawTimer = 5.0f;
	bool DestroyFlag = false;
	bool StartFlag = true;
	bool PullUpFlag = false;
	bool WarningFlag = false;
	float random[3];
	bool MusicFlag = false;

	float EnemyPosX[MaxCube];
	float EnemyPosY[MaxCube];

private:
	static void SceneChaneTitle();
	static void SceneChaneGamePlay();
	static void SceneChangeClear();
	void Bullet(XMFLOAT3 MyPosition, XMMATRIX matRot);
	bool BulletHitCheak(XMFLOAT3 BulletPosition, XMFLOAT3 EnemyPosition);

public:
	GameScene();
	void Update();
	void Draw();
};