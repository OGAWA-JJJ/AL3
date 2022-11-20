#pragma once
#include "CollisionPrimitive.h"
#include "RaycastHit.h"
#include <forward_list>
#include <d3d12.h>

class BaseCollider;

class CollisionManager
{
private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
	//�R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*>colliders;

public:
	//�ÓI�����o�ϐ�
	static CollisionManager* GetInstance();

public:
	//�R���C�_�[�̒ǉ�
	inline void AddCollider(BaseCollider* collider) { colliders.push_front(collider); }
	//�R���C�_�[�̍폜(�o�^����)
	inline void RemoveCollider(BaseCollider* collider) { colliders.remove(collider); }
	//�S�Ă̏Փ˃`�F�b�N
	void CheakAllCollisions();
	//���C�L���X�g
	bool Raycast(
		const Ray& ray,
		RaycastHit* hitInfo = nullptr,
		float maxDistance = D3D12_FLOAT32_MAX);
};
