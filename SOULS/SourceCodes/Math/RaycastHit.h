#pragma once
#include "BaseCollider.h"
#include <DirectXMath.h>

class Object;

struct RaycastHit
{
	//�Փ˔���̃I�u�W�F�N�g
	Object* object = nullptr;
	//�Փ˔���̃R���C�_�[
	BaseCollider* collider = nullptr;
	//�Փ˓_
	DirectX::XMVECTOR inter;
	//�Փ˓_�܂ł̋���
	float distance;
};
