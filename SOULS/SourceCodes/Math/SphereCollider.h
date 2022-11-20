#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include <DirectXMath.h>

class SphereCollider :public BaseCollider, public Sphere
{
private:
	//省略用
	using XMVECTOR = DirectX::XMVECTOR;

private:
	//オブジェクト中心からのオフセット
	XMVECTOR offset;
	//半径
	float radius;

public:
	SphereCollider(XMVECTOR offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset(offset),
		radius(radius)
	{
		shapeType = COLLISIONSHAPE_SHPERE;
	}

	//更新
	void Update() override;

	inline void SetRadius(float radius) { this->radius = radius; }
};
