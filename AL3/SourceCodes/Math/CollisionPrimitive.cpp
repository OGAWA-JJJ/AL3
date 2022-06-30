#include "CollisionPrimitive.h"

//using namespace DirectX;

void Triangle::ComputeNormal()
{
	//DirectX::XMVECTOR p0_p1 = p1 - p0;
	DirectX::XMVECTOR p0_p1 = DirectX::XMVectorSubtract(p1, p0);
	//DirectX::XMVECTOR p0_p2 = p2 - p0;
	DirectX::XMVECTOR p0_p2 = DirectX::XMVectorSubtract(p2, p0);

	normal = DirectX::XMVector3Cross(p0_p1, p0_p2);
	normal = DirectX::XMVector3Normalize(normal);
}