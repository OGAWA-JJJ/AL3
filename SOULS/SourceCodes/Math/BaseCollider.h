#pragma once

#include "CollisionTypes.h"
#include "../3D/Object.h"
#include "CollisionInfo.h"

class BaseCollider
{
public:
	BaseCollider() = default;
	//���z�f�X�g���N�^
	virtual ~BaseCollider() = default;

	inline void SetObject(Object* object) { this->object = object; }

	inline Object* GetObject3D() { return object; }

	//�X�V
	virtual void Update() = 0;
	//�`��^�C�v�擾
	inline CollisionShapeTypes GetShapeType() { return shapeType; }
	//�Փˎ��R�[���o�b�N�֐�
	inline void OnCollision(const CollisionInfo& info) { object->OnCollision(info); }

protected:
	Object* object = nullptr;
	//�`��^�C�v
	CollisionShapeTypes shapeType = SHAPE_UNKNOWN;
};