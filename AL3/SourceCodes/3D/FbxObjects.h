#pragma once
#include "FbxModels.h"
#include "Light.h"

class FbxObjects
{
public:
	struct FbxInitData
	{
		const char* m_vsEntryPoint = "VSmain";
		const char* m_psEntryPoint = "PSmain";
	};

	struct FbxPipelineSet
	{
		ID3D12RootSignature* rootsignature;
		ID3D12PipelineState* pipelinestate;
	};

private:
	static const int MAX_BONES = 32;

	static ID3D12Device* device;
	static ID3D12GraphicsCommandList* cmdList;
	static FbxPipelineSet pipelineSet;
	static Light* light;

private:
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;
		DirectX::XMMATRIX world;
		DirectX::XMFLOAT3 cameraPos;
	};

	struct ConstBufferDataSkin
	{
		DirectX::XMMATRIX bones[MAX_BONES];
	};

public:
	static FbxObjects* Create(FbxModels* model = nullptr);

public:
	static void StaticInit(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList);
	//static void SetDevice(ID3D12Device* device) { FbxObjects::device = device; }
	static FbxPipelineSet CreateGraphicsPipeline(const FbxInitData& fbxInitdata);
	static void SetLight(Light* light) { FbxObjects::light = light; }

private:
	FbxModels* model = nullptr;
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();

	Microsoft::WRL::ComPtr<ID3D12Resource> constBufferDataB0;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffSkin;

	FbxTime frameTime;
	FbxTime startTime;
	FbxTime endTime;
	FbxTime currentTime;
	bool isPlay = false;

	ID3D12DescriptorHeap* fbxDescHeap = {};
	//ボーンの名前と行列(Update後に更新)
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> affineTrans;
	//手のワールド行列
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	//ボーン全部の回転行列
	std::vector<DirectX::XMMATRIX> matRots;

public:
	bool Init();
	void Update(bool isShadowCamera = false);
	void Draw(const FbxPipelineSet& pipelineSet);
	void PlayAnimation();

public:
	void SetModel(FbxModels* model) { this->model = model; }

	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	void SetRotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	const DirectX::XMFLOAT3& GetScale() { return scale; }
	const DirectX::XMFLOAT3& GetRotation() { return rotation; }
	const DirectX::XMFLOAT3& GetPosition() { return position; }
};
