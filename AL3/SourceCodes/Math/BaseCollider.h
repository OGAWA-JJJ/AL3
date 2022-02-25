#pragma once

#include "CollisionTypes.h"
#include "../3D/Object.h"
#include "CollisionInfo.h"

class BaseCollider
{
public:
	BaseCollider() = default;
	//仮想デストラクタ
	virtual ~BaseCollider() = default;

	inline void SetObject(Object* object) { this->object = object; }

	inline Object* GetObject3D() { return object; }

	//更新
	virtual void Update() = 0;
	//形状タイプ取得
	inline CollisionShapeTypes GetShapeType() { return shapeType; }
	//衝突時コールバック関数
	inline void OnCollision(const CollisionInfo& info) { object->OnCollision(info); }

protected:
	Object* object = nullptr;
	//形状タイプ
	CollisionShapeTypes shapeType = SHAPE_UNKNOWN;
};