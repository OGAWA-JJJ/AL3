#include "Light.h"
#include "../DirectX/ConstantBuffer.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/Camera.h"

Microsoft::WRL::ComPtr<ID3D12Device> Light::device = nullptr;

void Light::StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	//再初期化チェック
	assert(!Light::device);
	//nullptrチェック
	assert(device);
	//静的メンバ変数のセット
	Light::device = device;
}

std::shared_ptr<Light> Light::Create()
{
	//3Dオブジェクトのインスタンスを生成
	std::shared_ptr<Light> instance = std::make_shared<Light>();
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
	//影用
	DirectX::XMMATRIX matView = DirectX::XMMatrixLookAtLH(
		XMLoadFloat3(&shadowEye),
		XMLoadFloat3(&shadowTarget),
		XMLoadFloat3(&shadowUp));

	float l_width = static_cast<float>(WINDOW_WIDTH / 2);
	float l_height = static_cast<float>(WINDOW_HEIGHT / 2);
	float l_far = ImguiControl::Imgui_far_z;

	DirectX::XMMATRIX lightMatPerspective =
		DirectX::XMMatrixOrthographicOffCenterLH(
			-l_width, l_width,
			-l_height, l_height,
			1.0f, l_far);

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

void Light::Draw(
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList,
	UINT rootParameterIndex)
{
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(
		rootParameterIndex,
		constBuff->GetGPUVirtualAddress()
	);
}
