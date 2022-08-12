#pragma once
#include "../3D/FbxObject3D.h"

class Enemy
{
private:	//�G���f��

private:	//���[�V����
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
	void Update(DirectX::XMFLOAT3 playerPos);
	void Draw();

public:
	const DirectX::XMFLOAT3& GetPos() { return pos; }
};

