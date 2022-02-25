#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include <DirectXMath.h>

class MeshCollider :public BaseCollider
{
public:
	MeshCollider() { shapeType = COLLISIONSHAPE_MESH; }
};
