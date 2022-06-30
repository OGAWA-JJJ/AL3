#include "SphereCollider.h"

//using namespace DirectX;

void SphereCollider::Update()
{
	//ワールド行列から座標を抽出
	const DirectX::XMMATRIX& matWorld = object->GetMatWorld();

	//球のメンバ変数を更新
	//matWorld.r[3] + offset;
	Sphere::center = DirectX::XMVectorAdd(matWorld.r[3], offset);
	Sphere::radius = radius;
}
