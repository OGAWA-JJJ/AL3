#include "Light.h"
#include "../DirectX/ConstantBuffer.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/Camera.h"

ID3D12Device* Light::device = nullptr;

void Light::StaticInit(ID3D12Device* device)
{
	//再初期化チェック
	assert(!Light::device);
	//nullptrチェック
	assert(device);
	//静的メンバ変数のセット
	Light::device = device;
}

Light* Light::Create()
{
	//3Dオブジェクトのインスタンスを生成
	Light* instance = new Light();
	//初期化
	instance->Init();
	//生成したインスタンスを返す
	return instance;
}

void Light::Init()
{
	HRESULT result;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result)) { assert(0); }

	//定数バッファへデータ転送
	TransferConstBuffer();
}

void Light::TransferConstBuffer()
{
	//HRESULT result;

	//影用
	DirectX::XMMATRIX matView = DirectX::XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up));

	float fov = ImguiControl::Imgui_fov;
	DirectX::XMMATRIX lightMatPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov),
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,
		0.1f, ImguiControl::Imgui_far_z); //前端、奥端

	const DirectX::XMMATRIX& lightMatViewProjection = matView * lightMatPerspective;

	//定数バッファへデータ転送
	ConstBufferData constMap;
	constMap.lightv = lightdir;
	constMap.lightcolor = lightcolor;
	constMap.lightViewproj = lightMatViewProjection;

	ConstantBuffer::CopyToVRAM(constBuff, &constMap, sizeof(ConstBufferData));
}

void Light::SetLightDir(const DirectX::XMVECTOR& lightdir)
{
	//正規化してセット
	this->lightdir = DirectX::XMVector3Normalize(lightdir);
	dirty = true;
}

void Light::SetLightColor(const DirectX::XMFLOAT3& lightcolor)
{
	this->lightcolor = lightcolor;
	dirty = true;
}

void Light::Update()
{
	//値の更新があった時だけ定数バッファに転送する
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(
		rootParameterIndex,
		constBuff->GetGPUVirtualAddress()
	);
}
