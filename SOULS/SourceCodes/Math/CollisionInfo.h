#pragma once

#include <DirectXMath.h>

class Object;
class BaseCollider;

struct CollisionInfo
{
public:
	CollisionInfo(Object* object, BaseCollider* collider, const DirectX::XMVECTOR& inter) {
		this->object = object;
		this->collider = collider;
		this->inter = inter;
	}

	Object* object = nullptr;
	BaseCollider* collider = nullptr;
	DirectX::XMVECTOR inter;
};
