#include "Enemy.h"

Enemy::Enemy()
{
	pos = {};
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	//obj_Lich->SetPosition(XMFLOAT3(0.0f, 35.0f, -400.0f));
	//obj_Lich->SetRotation(XMFLOAT3(0, -100, 0));
}

void Enemy::Update(DirectX::XMFLOAT3 playerPos)
{
	/*if (ImguiControl::Imgui_isTargetMove)
	{
		count += 0.02f;
		obj_Lich->SetPosition(XMFLOAT3(
			sinf(count) * 175.0f,
			obj_Lich->GetPosition().y,
			obj_Lich->GetPosition().z
		));
	}*/
}

void Enemy::Draw()
{
}
