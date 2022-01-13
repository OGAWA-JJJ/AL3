#include "Camera.h"

XMMATRIX Camera::matView;
XMMATRIX Camera::matPerspective;
XMFLOAT3 Camera::eye = { 0,0,-10.0f };
XMFLOAT3 Camera::target = { 0,0,0 };
XMFLOAT3 Camera::up = { 0,1,0 };
float Camera::fov = 60.0f;

XMMATRIX Camera::ViewMatrix()
{
	matView = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up));
	return matView;
}

XMMATRIX Camera::PerspectiveMatrix()
{
	matPerspective = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(fov),
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,
		0.1f, 1000.0f); //前端、奥端
	return matPerspective;
}

void Camera::InverseMatrix()
{
	matView *= XMMatrixInverse(
		nullptr,
		matView
	);
}

void Camera::SetEye(XMFLOAT3 eye)
{
	Camera::eye = eye;
}

void Camera::SetTarget(XMFLOAT3 target)
{
	Camera::target = target;
}

void Camera::SetUp(XMFLOAT3 up)
{
	Camera::up = up;
}

void Camera::SetFov(float fov)
{
	Camera::fov = fov;
}

XMMATRIX Camera::FollowCamera(float dist, XMFLOAT3 Rotation, XMMATRIX matRot)
{
	//計算用のカメラ情報（XMVECTOR）
	XMVECTOR eye = { Camera::eye.x,Camera::eye.y,Camera::eye.z,0 };
	XMVECTOR target = { Camera::target.x,Camera::target.y,Camera::target.z,0 };
	XMVECTOR up = { Camera::up.x,Camera::up.y,Camera::up.z,0 };

	//1. 無回転状態のオフセットベクトル
	XMVECTOR v0 = { 0,0,-dist,0 };

	//2. X,Y,Z軸の回転行列計算
	XMMATRIX rotZ = XMMatrixRotationZ(XMConvertToRadians(Rotation.z));
	XMMATRIX rotX = XMMatrixRotationX(XMConvertToRadians(Rotation.x));
	XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(Rotation.y));

	//3. V0を回転したベクトルVの計算
	XMVECTOR v = XMVector3TransformNormal(v0, rotZ * rotX * rotY);

	//4. targetにVを加算
	eye = target + v;

	//上方向ベクトルに回転行列をかけると上手くいくと聞いたので計算
	up = { 0,1,0,0 };
	up = XMVector3Transform(up, matRot);

	//5. XMFLOAT3に直してビュー行列の再計算
	Camera::eye = { eye.m128_f32[0],eye.m128_f32[1], eye.m128_f32[2] };
	Camera::target = { target.m128_f32[0],target.m128_f32[1], target.m128_f32[2] };
	Camera::up = { up.m128_f32[0],up.m128_f32[1], up.m128_f32[2] };

	matView = XMMatrixLookAtLH(
		XMLoadFloat3(&Camera::eye),
		XMLoadFloat3(&Camera::target),
		XMLoadFloat3(&Camera::up));
	return matView;
}

XMMATRIX Camera::BillboardMatrix()
{
	XMVECTOR eye = XMLoadFloat3(&Camera::eye);
	XMVECTOR target = XMLoadFloat3(&Camera::target);
	XMVECTOR up = XMLoadFloat3(&Camera::up);
	XMVECTOR cameraAxisZ = XMVectorSubtract(target, eye);
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);
	XMVECTOR cameraAxisX = XMVector3Cross(up, cameraAxisZ);
	cameraAxisX = XMVector3Normalize(cameraAxisX);
	XMVECTOR cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	XMMATRIX matBillboard = XMMatrixIdentity();
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);

	return matBillboard;
}

XMMATRIX Camera::BillboardYMatrix()
{
	XMVECTOR eye = XMLoadFloat3(&Camera::eye);
	XMVECTOR target = XMLoadFloat3(&Camera::target);
	XMVECTOR up = XMLoadFloat3(&Camera::up);
	XMVECTOR cameraAxisZ = XMVectorSubtract(target, eye);
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);
	XMVECTOR cameraAxisX = XMVector3Cross(up, cameraAxisZ);
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	XMVECTOR yBillCameraAxisX = cameraAxisX;
	XMVECTOR yBillCameraAxisY = XMVector3Normalize(up);
	XMVECTOR yBillCameraAxisZ = XMVector3Cross(yBillCameraAxisX, yBillCameraAxisY);
	XMMATRIX matBillboardY = XMMatrixIdentity();
	matBillboardY.r[0] = yBillCameraAxisX;
	matBillboardY.r[1] = yBillCameraAxisY;
	matBillboardY.r[2] = yBillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);

	return matBillboardY;
}
