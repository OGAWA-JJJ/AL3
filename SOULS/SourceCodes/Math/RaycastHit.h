#pragma once
#include "BaseCollider.h"
#include <DirectXMath.h>

class Object;

struct RaycastHit
{
	//衝突判定のオブジェクト
	Object* object = nullptr;
	//衝突判定のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	DirectX::XMVECTOR inter;
	//衝突点までの距離
	float distance;
};
