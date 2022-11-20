#include "OBBCollision.h"

bool OBBCollision::CollisionOBBs(const OBB& obb1, const OBB& obb2)
{
	/*---方向ベクトルの確保---*/

	DirectX::XMFLOAT3 obb1NormaVec[3];
	DirectX::XMFLOAT3 obb2NormaVec[3];

	DirectX::XMVECTOR x1 = { 1,0,0 };
	x1 = DirectX::XMVector3Transform(x1, obb1.matrix);
	DirectX::XMVECTOR y1 = { 0,1,0 };
	y1 = DirectX::XMVector3Transform(y1, obb1.matrix);
	DirectX::XMVECTOR z1 = { 0,0,1 };
	z1 = DirectX::XMVector3Transform(z1, obb1.matrix);

	DirectX::XMVECTOR x2 = { 1,0,0 };
	x2 = DirectX::XMVector3Transform(x2, obb2.matrix);
	DirectX::XMVECTOR y2 = { 0,1,0 };
	y2 = DirectX::XMVector3Transform(y2, obb2.matrix);
	DirectX::XMVECTOR z2 = { 0,0,1 };
	z2 = DirectX::XMVector3Transform(z2, obb2.matrix);

	DirectX::XMStoreFloat3(&obb1NormaVec[0], x1);
	DirectX::XMStoreFloat3(&obb1NormaVec[1], y1);
	DirectX::XMStoreFloat3(&obb1NormaVec[2], z1);

	DirectX::XMStoreFloat3(&obb2NormaVec[0], x2);
	DirectX::XMStoreFloat3(&obb2NormaVec[1], y2);
	DirectX::XMStoreFloat3(&obb2NormaVec[2], z2);

#pragma region obb1
	//obb1のx方向ベクトル(正規化済)
	DirectX::XMFLOAT3 obb1NormaVecX;
	obb1NormaVecX.x = obb1NormaVec[0].x;
	obb1NormaVecX.y = obb1NormaVec[0].y;
	obb1NormaVecX.z = obb1NormaVec[0].z;
	//obb1のx方向ベクトル
	DirectX::XMFLOAT3 obb1VecX;
	obb1VecX.x = obb1NormaVecX.x * obb1.length.x;
	obb1VecX.y = obb1NormaVecX.y * obb1.length.x;
	obb1VecX.z = obb1NormaVecX.z * obb1.length.x;


	//obb1のy方向ベクトル(正規化済)
	DirectX::XMFLOAT3 obb1NormaVecY;
	obb1NormaVecY.x = obb1NormaVec[1].x;
	obb1NormaVecY.y = obb1NormaVec[1].y;
	obb1NormaVecY.z = obb1NormaVec[1].z;
	//obb1のy方向ベクトル
	DirectX::XMFLOAT3 obb1VecY;
	obb1VecY.x = obb1NormaVecY.x * obb1.length.y;
	obb1VecY.y = obb1NormaVecY.y * obb1.length.y;
	obb1VecY.z = obb1NormaVecY.z * obb1.length.y;


	//obb1のz方向ベクトル(正規化済)
	DirectX::XMFLOAT3 obb1NormaVecZ;
	obb1NormaVecZ.x = obb1NormaVec[2].x;
	obb1NormaVecZ.y = obb1NormaVec[2].y;
	obb1NormaVecZ.z = obb1NormaVec[2].z;
	//obb1のz方向ベクトル
	DirectX::XMFLOAT3 obb1VecZ;
	obb1VecZ.x = obb1NormaVecZ.x * obb1.length.z;
	obb1VecZ.y = obb1NormaVecZ.y * obb1.length.z;
	obb1VecZ.z = obb1NormaVecZ.z * obb1.length.z;
#pragma endregion

#pragma region obb2
	//obb2のx方向ベクトル(正規化済)
	DirectX::XMFLOAT3 obb2NormaVecX;
	obb2NormaVecX.x = obb2NormaVec[0].x;
	obb2NormaVecX.y = obb2NormaVec[0].y;
	obb2NormaVecX.z = obb2NormaVec[0].z;
	//obb2のx方向ベクトル
	DirectX::XMFLOAT3 obb2VecX;
	obb2VecX.x = obb2NormaVecX.x * obb2.length.x;
	obb2VecX.y = obb2NormaVecX.y * obb2.length.x;
	obb2VecX.z = obb2NormaVecX.z * obb2.length.x;


	//obb2のy方向ベクトル(正規化済)
	DirectX::XMFLOAT3 obb2NormaVecY;
	obb2NormaVecY.x = obb2NormaVec[1].x;
	obb2NormaVecY.y = obb2NormaVec[1].y;
	obb2NormaVecY.z = obb2NormaVec[1].z;
	//obb2のy方向ベクトル
	DirectX::XMFLOAT3 obb2VecY;
	obb2VecY.x = obb2NormaVecY.x * obb2.length.y;
	obb2VecY.y = obb2NormaVecY.y * obb2.length.y;
	obb2VecY.z = obb2NormaVecY.z * obb2.length.y;


	//obb2のz方向ベクトル(正規化済)
	DirectX::XMFLOAT3 obb2NormaVecZ;
	obb2NormaVecZ.x = obb2NormaVec[2].x;
	obb2NormaVecZ.y = obb2NormaVec[2].y;
	obb2NormaVecZ.z = obb2NormaVec[2].z;
	//obb2のz方向ベクトル
	DirectX::XMFLOAT3 obb2VecZ;
	obb2VecZ.x = obb2NormaVecZ.x * obb2.length.z;
	obb2VecZ.y = obb2NormaVecZ.y * obb2.length.z;
	obb2VecZ.z = obb2NormaVecZ.z * obb2.length.z;
#pragma endregion

	DirectX::XMFLOAT3 interval;
	interval.x = obb1.pos.x - obb2.pos.x;
	interval.y = obb1.pos.y - obb2.pos.y;
	interval.z = obb1.pos.z - obb2.pos.z;

	/*---分離軸の計算---*/

#pragma region obb1
	//分離軸(obb1VecX)
	float rA;
	DirectX::XMStoreFloat(&rA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&obb1VecX)));
	float rB = LenSegOnSeparateAxis(obb1NormaVecX, obb2VecX, obb2VecY, obb2VecZ);
	float L;
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		DirectX::XMLoadFloat3(&obb1NormaVecX)
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }


	//分離軸(obb1VecY)
	DirectX::XMStoreFloat(&rA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&obb1VecY)));
	rB = LenSegOnSeparateAxis(obb1NormaVecY, obb2VecX, obb2VecY, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		DirectX::XMLoadFloat3(&obb1NormaVecY)
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }


	//分離軸(obb1VecZ)
	DirectX::XMStoreFloat(&rA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&obb1VecZ)));
	rB = LenSegOnSeparateAxis(obb1NormaVecZ, obb2VecX, obb2VecY, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		DirectX::XMLoadFloat3(&obb1NormaVecZ)
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }
#pragma endregion

#pragma region obb2
	//分離軸(obb2VecX)
	DirectX::XMStoreFloat(&rA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&obb2VecX)));
	rB = LenSegOnSeparateAxis(obb2NormaVecX, obb1VecX, obb1VecY, obb1VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		DirectX::XMLoadFloat3(&obb2NormaVecX)
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//分離軸(obb2VecY)
	DirectX::XMStoreFloat(&rA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&obb2VecY)));
	rB = LenSegOnSeparateAxis(obb2NormaVecY, obb1VecX, obb1VecY, obb1VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		DirectX::XMLoadFloat3(&obb2NormaVecY)
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//分離軸(obb2VecZ)
	DirectX::XMStoreFloat(&rA, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&obb2VecZ)));
	rB = LenSegOnSeparateAxis(obb2NormaVecZ, obb1VecX, obb1VecY, obb1VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		DirectX::XMLoadFloat3(&obb2NormaVecZ)
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }
#pragma endregion

#pragma region other
	//C11
	DirectX::XMVECTOR crossVec;
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecX),
		DirectX::XMLoadFloat3(&obb2NormaVecX)
	);
	DirectX::XMFLOAT3 crossFlo;
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecY, obb1VecZ);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecY, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C12
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecX),
		DirectX::XMLoadFloat3(&obb2NormaVecY)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecY, obb1VecZ);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecX, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C13
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecX),
		DirectX::XMLoadFloat3(&obb2NormaVecZ)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecY, obb1VecZ);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecX, obb2VecY);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C21
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecY),
		DirectX::XMLoadFloat3(&obb2NormaVecX)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecX, obb1VecZ);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecY, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C22
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecY),
		DirectX::XMLoadFloat3(&obb2NormaVecY)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecX, obb1VecZ);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecX, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C23
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecY),
		DirectX::XMLoadFloat3(&obb2NormaVecZ)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecX, obb1VecZ);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecX, obb2VecY);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C31
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecZ),
		DirectX::XMLoadFloat3(&obb2NormaVecX)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecX, obb1VecY);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecY, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C32
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecZ),
		DirectX::XMLoadFloat3(&obb2NormaVecY)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecX, obb1VecY);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecX, obb2VecZ);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

	//C33
	crossVec = DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&obb1NormaVecZ),
		DirectX::XMLoadFloat3(&obb2NormaVecZ)
	);
	DirectX::XMStoreFloat3(&crossFlo, crossVec);
	rA = LenSegOnSeparateAxis(crossFlo, obb1VecX, obb1VecY);
	rB = LenSegOnSeparateAxis(crossFlo, obb2VecX, obb2VecY);
	DirectX::XMStoreFloat(&L, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&interval),
		crossVec
	));
	L = fabsf(L);
	if (L > rA + rB) { return false; }

#pragma endregion

	return true;
}

float OBBCollision::LenSegOnSeparateAxis(const DirectX::XMFLOAT3& sep, const DirectX::XMFLOAT3& e1, const DirectX::XMFLOAT3& e2, const DirectX::XMFLOAT3& e3)
{
	float r1;
	DirectX::XMStoreFloat(&r1, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&sep),
		DirectX::XMLoadFloat3(&e1)
	));
	r1 = fabsf(r1);

	float r2;
	DirectX::XMStoreFloat(&r2, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&sep),
		DirectX::XMLoadFloat3(&e2)
	));
	r2 = fabsf(r2);

	float r3;
	DirectX::XMStoreFloat(&r3, DirectX::XMVector3Dot(
		DirectX::XMLoadFloat3(&sep),
		DirectX::XMLoadFloat3(&e3)
	));

	if (e3.x == 0 && e3.y == 0 && e3.z == 0) { r3 = 0; }
	else { r3 = fabsf(r3); }

	return r1 + r2 + r3;
}
