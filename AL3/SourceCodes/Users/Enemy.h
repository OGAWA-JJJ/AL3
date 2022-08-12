#pragma once
#include "../3D/FbxObject3D.h"

class Enemy
{
private:	//敵モデル

private:	//モーション
	enum AnimationType
	{
		NONE
	};

private:
	DirectX::XMFLOAT3 pos;

public:
	Enemy();
	~Enemy();
	void Init();
	void Update();
	void Draw();

public:
	const DirectX::XMFLOAT3& GetPos() { return pos; }
};

