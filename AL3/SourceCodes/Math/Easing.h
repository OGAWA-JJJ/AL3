#pragma once
#include <cmath>
#include <DirectXMath.h>

static inline const DirectX::XMFLOAT3 operator +(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

static inline const DirectX::XMFLOAT3 operator -(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

static inline const DirectX::XMFLOAT3 operator *(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

static inline const DirectX::XMFLOAT3 operator /(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x / v2.x;
	result.y = v1.y / v2.y;
	result.z = v1.z / v2.z;
	return result;
}

static inline const DirectX::XMFLOAT3 operator /(const DirectX::XMFLOAT3 v1, float s)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x / s;
	result.y = v1.y / s;
	result.z = v1.z / s;
	return result;
}

namespace OgaJEase {

constexpr float PI = 3.14159265359f;

/*-----Sine-----*/

inline float easeInSine(const float& t) {
	return 1 - cosf((t * PI) / 2);
}

inline DirectX::XMFLOAT3 easeInSineXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - cosf((t * PI) / 2);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeOutSine(const float& t) {
	return sinf((t * PI) / 2);
}

inline DirectX::XMFLOAT3 easeOutSineXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = sinf((t * PI) / 2);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeInOutSine(const float& t) {
	return -(cosf(PI * t) - 1) / 2;
}

inline DirectX::XMFLOAT3 easeInOutSineXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = -(cosf(PI * t) - 1) / 2;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

/*-----Quad-----*/

inline float easeInQuad(const float& t) {
	return t * t;
}

inline DirectX::XMFLOAT3 easeInQuadXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t * t;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeOutQuad(const float& t) {
	return 1 - (1 - t) * (1 - t);
}

inline DirectX::XMFLOAT3 easeOutQuadXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - (1 - t) * (1 - t);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeInOutQuad(const float& t) {
	return t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;
}

inline DirectX::XMFLOAT3 easeInOutQuadXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

/*-----Cubic-----*/

inline float easeInCubic(const float& t) {
	return t * t * t;
}

inline DirectX::XMFLOAT3 easeInCubicXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t * t * t;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeOutCubic(const float& t) {
	return 1 - powf(1 - t, 3);
}

inline DirectX::XMFLOAT3 easeOutCubicXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - powf(1 - t, 3);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeInOutCubic(const float& t) {
	return t < 0.5 ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;
}

inline DirectX::XMFLOAT3 easeInOutCubicXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t < 0.5 ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

/*-----Quart-----*/

inline float easeInQuart(const float& t) {
	return t * t * t * t;
}

inline DirectX::XMFLOAT3 easeInQuartXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t * t * t * t;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeOutQuart(const float& t) {
	return 1 - powf(1 - t, 4);
}

inline DirectX::XMFLOAT3 easeOutQuartXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - powf(1 - t, 4);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

inline float easeInOutQuart(const float& t) {
	return t < 0.5 ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2;
}

inline DirectX::XMFLOAT3 easeInOutQuartXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t < 0.5 ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

}