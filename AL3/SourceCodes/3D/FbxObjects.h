#pragma once
#include "FbxModels.h"
#include "Light.h"

//Struct2つ必要(FbxObject3D参照)

class FbxObjects
{
private:
	static const int MAX_BONES = 32;

	static ID3D12Device* device;
	static ID3D12GraphicsCommandList* cmdList;
	//static ObjPipelineSet pipelineSet;
	static Light* light;

private:
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;		//ビュープロジェクション行列
		DirectX::XMMATRIX world;		//ワールド行列
		DirectX::XMFLOAT3 cameraPos;	//カメラ座標（ワールド座標）
	};

public:
	static void StaticInit(ID3D12Device* dev);
	static void SetDevice(ID3D12Device* device) { FbxObjects::device = device; }
	//static FbxPipelineSet CreateGraphicsPipeline(const FbxInitData& fbxInitdata);
	static void SetLight(Light* light) { FbxObjects::light = light; }

private:
	FbxModels* model = nullptr;
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffTransform;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffSkin;

	FbxTime frameTime;
	FbxTime startTime;
	FbxTime endTime;
	FbxTime currentTime;
	bool isPlay = false;
};
