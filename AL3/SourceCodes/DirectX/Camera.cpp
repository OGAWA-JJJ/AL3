#include "Camera.h"
#include "../../imgui/ImguiControl.h"

DirectX::XMMATRIX Camera::matView;
DirectX::XMMATRIX Camera::matPerspective;
DirectX::XMFLOAT3 Camera::eye = { 0,20,-50.0f };
DirectX::XMFLOAT3 Camera::target = { 0,0,0 };
DirectX::XMFLOAT3 Camera::up = { 0,1,0 };
float Camera::fov = 60.0f;

DirectX::XMMATRIX Camera::ViewMatrix()
{
	//eye.x = ImguiControl::Imgui_eye_x;
	//eye.y = ImguiControl::Imgui_eye_y;
	//eye.z = ImguiControl::Imgui_eye_z;

	matView = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&eye),
		DirectX::XMLoadFloat3(&target),
		DirectX::XMLoadFloat3(&up));
	return matView;
}

DirectX::XMMATRIX Camera::PerspectiveMatrix()
{
	fov = ImguiControl::Imgui_fov;

	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov),
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,
		0.1f, ImguiControl::Imgui_far_z); //前端、奥端
	return matPerspective;
}

void Camera::InverseMatrix()
{
	matView *= DirectX::XMMatrixInverse(
		nullptr,
		matView
	);
}

void Camera::SetEye(DirectX::XMFLOAT3 eye)
{
	Camera::eye = eye;
}

void Camera::SetTarget(DirectX::XMFLOAT3 target)
{
	Camera::target = target;
}

void Camera::SetUp(DirectX::XMFLOAT3 up)
{
	Camera::up = up;
}

void Camera::SetFov(float fov)
{
	Camera::fov = fov;
}

DirectX::XMMATRIX Camera::FollowCamera(float dist, DirectX::XMFLOAT3 Rotation, DirectX::XMMATRIX matRot)
{
	//計算用のカメラ情報（XMVECTOR）
	DirectX::XMVECTOR eye = { Camera::eye.x,Camera::eye.y,Camera::eye.z,0 };
	DirectX::XMVECTOR target = { Camera::target.x,Camera::target.y,Camera::target.z,0 };
	DirectX::XMVECTOR up = { Camera::up.x,Camera::up.y,Camera::up.z,0 };

	//1. 無回転状態のオフセットベクトル
	DirectX::XMVECTOR v0 = { 0,0,-dist,0 };

	//2. X,Y,Z軸の回転行列計算
	DirectX::XMMATRIX rotZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(Rotation.z));
	DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(Rotation.x));
	DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(Rotation.y));

	//3. V0を回転したベクトルVの計算
	DirectX::XMVECTOR v = DirectX::XMVector3TransformNormal(v0, rotZ * rotX * rotY);

	//4. targetにVを加算
	//eye = target + v;
	eye = DirectX::XMVectorAdd(target, v);

	//上方向ベクトルに回転行列をかけると上手くいくと聞いたので計算
	up = { 0,1,0,0 };
	up = DirectX::XMVector3Transform(up, matRot);

	//5. XMFLOAT3に直してビュー行列の再計算
	Camera::eye = { eye.m128_f32[0],eye.m128_f32[1], eye.m128_f32[2] };
	Camera::target = { target.m128_f32[0],target.m128_f32[1], target.m128_f32[2] };
	Camera::up = { up.m128_f32[0],up.m128_f32[1], up.m128_f32[2] };

	matView = DirectX::XMMatrixLookAtLH(
		XMLoadFloat3(&Camera::eye),
		XMLoadFloat3(&Camera::target),
		XMLoadFloat3(&Camera::up));
	return matView;
}

DirectX::XMMATRIX Camera::BillboardMatrix()
{
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&Camera::eye);
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&Camera::target);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&Camera::up);
	DirectX::XMVECTOR cameraAxisZ = DirectX::XMVectorSubtract(target, eye);
	cameraAxisZ = DirectX::XMVector3Normalize(cameraAxisZ);
	DirectX::XMVECTOR cameraAxisX = DirectX::XMVector3Cross(up, cameraAxisZ);
	cameraAxisX = DirectX::XMVector3Normalize(cameraAxisX);
	DirectX::XMVECTOR cameraAxisY = DirectX::XMVector3Cross(cameraAxisZ, cameraAxisX);
	cameraAxisY = DirectX::XMVector3Normalize(cameraAxisY);

	DirectX::XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);

	return matBillboard;
}

DirectX::XMMATRIX Camera::BillboardYMatrix()
{
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&Camera::eye);
	DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&Camera::target);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&Camera::up);
	DirectX::XMVECTOR cameraAxisZ = DirectX::XMVectorSubtract(target, eye);
	cameraAxisZ = DirectX::XMVector3Normalize(cameraAxisZ);
	DirectX::XMVECTOR cameraAxisX = DirectX::XMVector3Cross(up, cameraAxisZ);
	cameraAxisX = DirectX::XMVector3Normalize(cameraAxisX);

	DirectX::XMVECTOR yBillCameraAxisX = cameraAxisX;
	DirectX::XMVECTOR yBillCameraAxisY = DirectX::XMVector3Normalize(up);
	DirectX::XMVECTOR yBillCameraAxisZ = DirectX::XMVector3Cross(yBillCameraAxisX, yBillCameraAxisY);
	DirectX::XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	matBillboardY.r[0] = yBillCameraAxisX;
	matBillboardY.r[1] = yBillCameraAxisY;
	matBillboardY.r[2] = yBillCameraAxisZ;
	matBillboardY.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);

	return matBillboardY;
}
