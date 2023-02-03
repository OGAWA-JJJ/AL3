#pragma once
#include "../DirectX/DirectXImportant.h"
#include <DirectXMath.h>

class Camera
{
private:
	static DirectX::XMMATRIX matView;
	static DirectX::XMMATRIX matPerspective;

private:
	static DirectX::XMFLOAT3 eye;
	static DirectX::XMFLOAT3 target;
	static DirectX::XMFLOAT3 up;
	static float fov;

	static bool isShake;
	static DirectX::XMFLOAT3 shakeEye;
	static DirectX::XMFLOAT3 shakeTarget;

public:
	//static void SetEye(XMFLOAT3 Eye);
	//static void SetTarget(XMFLOAT3 Target);

	/*通常カメラ*/
	static DirectX::XMMATRIX ViewMatrix();
	static DirectX::XMMATRIX PerspectiveMatrix();
	static void InverseMatrix();//?
	static void SetEye(DirectX::XMFLOAT3 eye);
	static void SetTarget(DirectX::XMFLOAT3 target);
	static void SetUp(DirectX::XMFLOAT3 up);
	static void SetFov(float fov);
	static DirectX::XMFLOAT3 GetEye() { return eye; }
	static DirectX::XMFLOAT3 GetTarget() { return target; }
	static DirectX::XMFLOAT3 GetUp() { return up; }

	//static void MoveEye(XMFLOAT3 eye);
	//static void MoveTarget(XMFLOAT3 target);
	//static void MoveUp(XMFLOAT3 up);

	/*追従カメラ*/
	static DirectX::XMMATRIX FollowCamera(float dist, DirectX::XMFLOAT3 Rotation, DirectX::XMMATRIX matRot);

	/*ビルボード*/
	static DirectX::XMMATRIX BillboardMatrix();
	static DirectX::XMMATRIX BillboardYMatrix();
};

