#pragma once
#include "DirectXImportant.h"

class Camera
{
private:
	static XMMATRIX matView;
	static XMMATRIX matPerspective;

public:
	static XMFLOAT3 eye;
	static XMFLOAT3 target;
	static XMFLOAT3 up;
	static float fov;

public:
	//static void SetEye(XMFLOAT3 Eye);
	//static void SetTarget(XMFLOAT3 Target);

	/*通常カメラ*/
	static XMMATRIX ViewMatrix();
	static XMMATRIX PerspectiveMatrix();
	static void InverseMatrix();//?
	static void SetEye(XMFLOAT3 eye);
	static void SetTarget(XMFLOAT3 target);
	static void SetUp(XMFLOAT3 up);
	static void SetFov(float fov);

	//static void MoveEye(XMFLOAT3 eye);
	//static void MoveTarget(XMFLOAT3 target);
	//static void MoveUp(XMFLOAT3 up);

	/*追従カメラ*/
	static XMMATRIX FollowCamera(float dist, XMFLOAT3 Rotation, XMMATRIX matRot);

	/*ビルボード*/
	static XMMATRIX BillboardMatrix();
	static XMMATRIX BillboardYMatrix();
};

