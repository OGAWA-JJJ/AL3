#include "Collision.h"

//using namespace DirectX;

bool Collision::CheakSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter)
{
	DirectX::XMVECTOR distV = DirectX::XMVector3Dot(sphere.center, plane.normal);
	float dist = distV.m128_f32[0] - plane.distance;
	if (fabsf(dist) > sphere.radius) return false;
	if (inter) {
		//*inter = -dist * plane.normal + sphere.center;
		DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
			-dist * plane.normal.m128_f32[0] + sphere.center.m128_f32[0],
			-dist * plane.normal.m128_f32[1] + sphere.center.m128_f32[1],
			-dist * plane.normal.m128_f32[2] + sphere.center.m128_f32[2],
			-dist * plane.normal.m128_f32[3] + sphere.center.m128_f32[3]);
		*inter = DirectX::XMLoadFloat4(&buff);
	}

	return true;
}

bool Collision::CheakSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter)
{
	DirectX::XMVECTOR p;
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	//DirectX::XMVECTOR v = p - sphere.center;
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(p, sphere.center);
	v = DirectX::XMVector3Dot(v, v);
	if (v.m128_f32[0] > sphere.radius * sphere.radius) return false;
	if (inter) {
		*inter = p;
	}

	return true;
}

bool Collision::CheakSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter)
{
	DirectX::XMFLOAT3 posA, posB;
	DirectX::XMStoreFloat3(&posA, sphereA.center);
	DirectX::XMStoreFloat3(&posB, sphereB.center);
	//2点間の距離
	float dist = CalcDist(posA, posB);
	//チェック
	if (dist > sphereA.radius + sphereB.radius) { return false; }

	//疑似交点を計算
	if (inter) {

	}

	return true;
}

bool Collision::CheakRay2Plane(const Ray& ray, const Plane& plane, float* distance, DirectX::XMVECTOR* inter)
{
	const float epsilon = 1.0e-5f;
	float d1 = DirectX::XMVector3Dot(plane.normal, ray.dir).m128_f32[0];
	if (d1 > -epsilon) return false;
	float d2 = DirectX::XMVector3Dot(plane.normal, ray.start).m128_f32[0];
	float dist = d2 - plane.distance;
	float t = dist / -d1;
	if (t < 0) return false;
	if (distance) *distance = t;
	if (inter) {
		//*inter = ray.start + t * ray.dir;
		DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
			ray.start.m128_f32[0] + t * ray.dir.m128_f32[0],
			ray.start.m128_f32[1] + t * ray.dir.m128_f32[1],
			ray.start.m128_f32[2] + t * ray.dir.m128_f32[2],
			ray.start.m128_f32[3] + t * ray.dir.m128_f32[3]);
		*inter = DirectX::XMLoadFloat4(&buff);
	}

	return true;
}

bool Collision::CheakRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, DirectX::XMVECTOR* inter)
{
	Plane plane;
	DirectX::XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = DirectX::XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	if (!CheakRay2Plane(ray, plane, distance, &interPlane)) return false;
	const float epsilon = 1.0e-5f;
	DirectX::XMVECTOR m;

	//DirectX::XMVECTOR pt_p0 = triangle.p0 - interPlane;
	DirectX::XMVECTOR pt_p0 = DirectX::XMVectorSubtract(triangle.p0, interPlane);
	//DirectX::XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	DirectX::XMVECTOR p0_p1 = DirectX::XMVectorSubtract(triangle.p1, triangle.p0);
	m = DirectX::XMVector3Cross(pt_p0, p0_p1);
	if (DirectX::XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;

	//DirectX::XMVECTOR pt_p1 = triangle.p1 - interPlane;
	DirectX::XMVECTOR pt_p1 = DirectX::XMVectorSubtract(triangle.p1, interPlane);
	//DirectX::XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	DirectX::XMVECTOR p1_p2 = DirectX::XMVectorSubtract(triangle.p2, triangle.p1);
	m = DirectX::XMVector3Cross(pt_p1, p1_p2);
	if (DirectX::XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;

	//DirectX::XMVECTOR pt_p2 = triangle.p2 - interPlane;
	DirectX::XMVECTOR pt_p2 = DirectX::XMVectorSubtract(triangle.p2, interPlane);
	//DirectX::XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	DirectX::XMVECTOR p2_p0 = DirectX::XMVectorSubtract(triangle.p0, triangle.p2);
	m = DirectX::XMVector3Cross(pt_p2, p2_p0);
	if (DirectX::XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) return false;

	if (inter) {
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheakRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, DirectX::XMVECTOR* inter)
{
	//DirectX::XMVECTOR m = ray.start - sphere.center;
	DirectX::XMVECTOR m = DirectX::XMVectorSubtract(ray.start, sphere.center);
	float b = DirectX::XMVector3Dot(m, ray.dir).m128_f32[0];
	float c = DirectX::XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	if (c > 0.0f && b > 0.0f) return false;
	float discr = b * b - c;
	if (discr < 0.0f) return false;
	float t = -b - sqrtf(discr);
	if (t < 0) t = 0.0f;
	if (distance) *distance = t;
	if (inter) {
		//*inter = ray.start + t * ray.dir;
		DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
			ray.start.m128_f32[0] + t * ray.dir.m128_f32[0],
			ray.start.m128_f32[1] + t * ray.dir.m128_f32[1],
			ray.start.m128_f32[2] + t * ray.dir.m128_f32[2],
			ray.start.m128_f32[3] + t * ray.dir.m128_f32[3]);
		*inter = DirectX::XMLoadFloat4(&buff);
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	//DirectX::XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	DirectX::XMVECTOR p0_p1 = DirectX::XMVectorSubtract(triangle.p1, triangle.p0);
	//DirectX::XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	DirectX::XMVECTOR p0_p2 = DirectX::XMVectorSubtract(triangle.p2, triangle.p0);
	//DirectX::XMVECTOR p0_pt = point - triangle.p0;
	DirectX::XMVECTOR p0_pt = DirectX::XMVectorSubtract(point, triangle.p0);

	DirectX::XMVECTOR d1 = DirectX::XMVector3Dot(p0_p1, p0_pt);
	DirectX::XMVECTOR d2 = DirectX::XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f)
	{
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	//DirectX::XMVECTOR p1_pt = point - triangle.p1;
	DirectX::XMVECTOR p1_pt = DirectX::XMVectorSubtract(point, triangle.p1);

	DirectX::XMVECTOR d3 = DirectX::XMVector3Dot(p0_p1, p1_pt);
	DirectX::XMVECTOR d4 = DirectX::XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0])
	{
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f)
	{
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		//*closest = triangle.p0 + v * p0_p1;
		DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
			triangle.p0.m128_f32[0] + v * p0_p1.m128_f32[0],
			triangle.p0.m128_f32[1] + v * p0_p1.m128_f32[1],
			triangle.p0.m128_f32[2] + v * p0_p1.m128_f32[2],
			triangle.p0.m128_f32[3] + v * p0_p1.m128_f32[3]);
		*closest = DirectX::XMLoadFloat4(&buff);
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	//DirectX::XMVECTOR p2_pt = point - triangle.p2;
	DirectX::XMVECTOR p2_pt = DirectX::XMVectorSubtract(point, triangle.p2);

	DirectX::XMVECTOR d5 = DirectX::XMVector3Dot(p0_p1, p2_pt);
	DirectX::XMVECTOR d6 = DirectX::XMVector3Dot(p0_p2, p2_pt);
	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0])
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f)
	{
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		//*closest = triangle.p0 + w * p0_p2;
		DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
			triangle.p0.m128_f32[0] + w * p0_p2.m128_f32[0],
			triangle.p0.m128_f32[1] + w * p0_p2.m128_f32[1],
			triangle.p0.m128_f32[2] + w * p0_p2.m128_f32[2],
			triangle.p0.m128_f32[3] + w * p0_p2.m128_f32[3]);
		*closest = DirectX::XMLoadFloat4(&buff);
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];
	if (va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f)
	{
		float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		DirectX::XMVECTOR sub = DirectX::XMVectorSubtract(triangle.p2, triangle.p1);
		//*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
			triangle.p1.m128_f32[0] + w * sub.m128_f32[0],
			triangle.p1.m128_f32[1] + w * sub.m128_f32[1],
			triangle.p1.m128_f32[2] + w * sub.m128_f32[2],
			triangle.p1.m128_f32[3] + w * sub.m128_f32[3]);
		*closest = DirectX::XMLoadFloat4(&buff);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	//DirectX::XMVECTOR add = DirectX::XMVectorAdd(triangle.p0, p0_p1);
	//*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
	DirectX::XMFLOAT4 buff = DirectX::XMFLOAT4(
		triangle.p0.m128_f32[0] + p0_p1.m128_f32[0] * v * p0_p2.m128_f32[0] * w,
		triangle.p0.m128_f32[1] + p0_p1.m128_f32[1] * v * p0_p2.m128_f32[1] * w,
		triangle.p0.m128_f32[2] + p0_p1.m128_f32[2] * v * p0_p2.m128_f32[2] * w,
		triangle.p0.m128_f32[3] + p0_p1.m128_f32[3] * v * p0_p2.m128_f32[3] * w);
	*closest = DirectX::XMLoadFloat4(&buff);
}
