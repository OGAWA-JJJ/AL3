#pragma once
#include "CollisionPrimitive.h"

class Collision
{
public:
	static bool CheakSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter = nullptr);
	static bool CheakSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter = nullptr);
	static bool CheakSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter = nullptr);
	static bool CheakRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	static bool CheakRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	static bool CheakRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, DirectX::XMVECTOR* inter = nullptr);
	static void ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest);

private:
	static float CalcDist(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2) {
		float diff;
		diff = sqrtf(
			(pos1.x - pos2.x) * (pos1.x - pos2.x) +
			(pos1.y - pos2.y) * (pos1.y - pos2.y) +
			(pos1.z - pos2.z) * (pos1.z - pos2.z));
		return diff;
	}
};

