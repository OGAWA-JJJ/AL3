#pragma once

#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include <DirectXMath.h>

class SphereCollider :public BaseCollider, public Sphere
{
private:
	//�ȗ��p
	using XMVECTOR = DirectX::XMVECTOR;

private:
	//�I�u�W�F�N�g���S����̃I�t�Z�b�g
	XMVECTOR offset;
	//���a
	float radius;

public:
	SphereCollider(XMVECTOR offset = { 0,0,0,0 }, float radius = 1.0f) :
		offset(offset),
		radius(radius)
	{
		shapeType = COLLISIONSHAPE_SHPERE;
	}

	//�X�V
	void Update() override;

	inline void SetRadius(float radius) { this->radius = radius; }
};
