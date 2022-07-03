#pragma once
#include <cmath>
#include <DirectXMath.h>

static const DirectX::XMFLOAT3 operator +(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

static const DirectX::XMFLOAT3 operator -(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

static const DirectX::XMFLOAT3 operator *(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

static const DirectX::XMFLOAT3 operator /(const DirectX::XMFLOAT3 v1, const DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x / v2.x;
	result.y = v1.y / v2.y;
	result.z = v1.z / v2.z;
	return result;
}

static const DirectX::XMFLOAT3 operator /(const DirectX::XMFLOAT3 v1, float s)
{
	DirectX::XMFLOAT3 result;
	result.x = v1.x / s;
	result.y = v1.y / s;
	result.z = v1.z / s;
	return result;
}

namespace OgaJEase {

float PI = 3.14159265359f;
using XMFLOAT3 = DirectX::XMFLOAT3;

/*-----Sine-----*/

float easeInSine(float t) {
	return 1 - cosf((t * PI) / 2);
}

DirectX::XMFLOAT3 easeInSineXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - cosf((t * PI) / 2);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeOutSine(float t) {
	return sinf((t * PI) / 2);
}

DirectX::XMFLOAT3 easeOutSineXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = sinf((t * PI) / 2);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeInOutSine(float t) {
	return -(cosf(PI * t) - 1) / 2;
}

DirectX::XMFLOAT3 easeInOutSineXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
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

float easeInQuad(float t) {
	return t * t;
}

DirectX::XMFLOAT3 easeInQuadXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t * t;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeOutQuad(float t) {
	return 1 - (1 - t) * (1 - t);
}

DirectX::XMFLOAT3 easeOutQuadXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - (1 - t) * (1 - t);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeInOutQuad(float t) {
	return t < 0.5 ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;
}

DirectX::XMFLOAT3 easeInOutQuadXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
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

float easeInCubic(float t) {
	return t * t * t;
}

DirectX::XMFLOAT3 easeInCubicXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t * t * t;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeOutCubic(float t) {
	return 1 - powf(1 - t, 3);
}

DirectX::XMFLOAT3 easeOutCubicXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - powf(1 - t, 3);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeInOutCubic(float t) {
	return t < 0.5 ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;
}

DirectX::XMFLOAT3 easeInOutCubicXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
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

float easeInQuart(float t) {
	return t * t * t * t;
}

DirectX::XMFLOAT3 easeInQuartXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = t * t * t * t;
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeOutQuart(float t) {
	return 1 - powf(1 - t, 4);
}

DirectX::XMFLOAT3 easeOutQuartXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
{
	DirectX::XMFLOAT3 num;
	DirectX::XMFLOAT3 sub = end - start;
	float ease = 1 - powf(1 - t, 4);
	num.x = start.x + (ease * sub.x);
	num.y = start.y + (ease * sub.y);
	num.z = start.z + (ease * sub.z);
	return num;
}

float easeInOutQuart(float t) {
	return t < 0.5 ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2;
}

DirectX::XMFLOAT3 easeInOutQuartXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& t)
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