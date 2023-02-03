#pragma once
#include <DirectXMath.h>

struct OBB
{
	DirectX::XMFLOAT3 pos;		//中心座標
	DirectX::XMMATRIX matrix;	//ワールド行列
	DirectX::XMFLOAT3 length;	//スケール
};

class OBBCollision
{
public:
	static bool CollisionOBBs(const OBB& obb1, const OBB& obb2);

private:
	static float LenSegOnSeparateAxis(
		const DirectX::XMFLOAT3& sep,
		const DirectX::XMFLOAT3& e1,
		const DirectX::XMFLOAT3& e2,
		const DirectX::XMFLOAT3& e3 = { 0,0,0 });
};

