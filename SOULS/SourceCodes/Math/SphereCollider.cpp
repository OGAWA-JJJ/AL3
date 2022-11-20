#include "SphereCollider.h"

//using namespace DirectX;

void SphereCollider::Update()
{
	//���[���h�s�񂩂���W�𒊏o
	const DirectX::XMMATRIX& matWorld = object->GetMatWorld();

	//���̃����o�ϐ����X�V
	//matWorld.r[3] + offset;
	Sphere::center = DirectX::XMVectorAdd(matWorld.r[3], offset);
	Sphere::radius = radius;
}
