#pragma once
#include "FbxModel.h"
#include "../3D/Light.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

struct FbxInitData
{
	const char* m_vsEntryPoint = "VSmain";
	const char* m_psEntryPoint = "PSmain";
};

//パイプラインセット
struct FbxPipelineSet
{
	//ルートシグネチャ
	ID3D12RootSignature* rootsignature;
	//パイプラインステートオブジェクト
	ID3D12PipelineState* pipelinestate;
};

class FbxObject3D
{
protected:
	//WRL省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//ボーンの最大数
	static const int MAX_BONES = 32;

public:
	//定数バッファ
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;	//ビュープロジェクション行列
		XMMATRIX world;		//ワールド行列
		XMFLOAT3 cameraPos;	//カメラ座標(ワールド座標)
	};

	//定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	struct ConstBufferData1
	{
		XMFLOAT3 ambient;	//アンビエント係数
		float pad1;			//パディング
		XMFLOAT3 diffuse;	//ディフューズ係数
		float pad2;			//パディング
		XMFLOAT3 specular;	//スペキュラー係数
		float alpha;		//アルファ
		XMFLOAT4 color;
	};

private:
	//デバイス
	static ID3D12Device* device;
	//ルートシグネチャ
	//static ComPtr<ID3D12RootSignature> rootsignature;
	//パイプラインステートオブジェクト
	//static ComPtr<ID3D12PipelineState> pipelinestate;
	//パイプライン
	static FbxPipelineSet pipelineSet;
	//ライト
	static Light* light;

public:
	//Setter
	static void SetDevice(ID3D12Device* device) { FbxObject3D::device = device; }
	//グラフィックスパイプラインの生成
	static FbxPipelineSet CreateGraphicsPipeline(const FbxInitData& fbxInitdata);
	//ライトのセット
	static void SetLight(Light* light) { FbxObject3D::light = light; }

private:
	//アンビエント影響度
	XMFLOAT3 ambient = { 0.8f, 0.8f, 0.8f };
	//ディフューズ影響度
	XMFLOAT3 diffuse = { 0.8f, 0.8f, 0.8f };
	//スペキュラー影響度
	XMFLOAT3 specular = { 0.8f, 0.8f, 0.8f };
	//アルファ
	float alpha = 1.0f;

protected:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファ（反射光）
	ComPtr<ID3D12Resource> constBuffData1;
	//ローカルスケール
	XMFLOAT3 scale = { 10,10,10 };
	//X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	//ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	//モデル
	FbxModel* model = nullptr;
	//定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBuffSkin;
	//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//現在時間(アニメーション)
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;

	//SRV用デスクリプタヒープ
	ID3D12DescriptorHeap* fbxDescHeap = {};

	//ボーンの名前と行列(Update後に更新)
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> affineTrans;
	//手のワールド行列
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	//ボーン全部の回転行列
	std::vector<DirectX::XMMATRIX> matRots;
	//回転行列
	DirectX::XMMATRIX m_matRot = DirectX::XMMatrixIdentity();
	//色
	DirectX::XMFLOAT4 m_color = { 1.0f,1.0f,1.0f,1.0f };

public:
	//初期化
	void Init();
	//毎フレーム処理
	void Update(bool isShadowCamera = false);
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList, const FbxPipelineSet& pipelineSet);
	//アニメーション開始
	void PlayAnimation();
	//テクスチャ追加
	void AddTexture(ID3D12Resource* texbuff, ID3D12DescriptorHeap* srv);
	//強制描画用(AddTexture後)
	void ShadowDraw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* srv);

public:
	void SetModel(FbxModel* model) { this->model = model; }
	//Setterを作れ！
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetPosition(const XMFLOAT3& position) { this->position = position; }
	void SetColor(const XMFLOAT4& color) { m_color = color; }
	const XMFLOAT3& GetScale() { return scale; }
	const XMFLOAT3& GetRotation() { return rotation; }
	const XMFLOAT3& GetPosition() { return position; }

	void StopAnimation() { isPlay = false; }			//アニメーションの停止
	void ResetAnimation() { currentTime = startTime; }	//アニメーションのリセット
	void ReplayAnimation() { isPlay = true; }			//アニメーションの再開

	const FbxTime& GetStartTime() { return startTime; }	//アニメーションの開始時間の取得
	const FbxTime& GetEndTime() { return endTime; }		//アニメーションの終了時間の取得
	const FbxTime& GetNowTime() { return currentTime; }	//現在のアニメーションの時間の取得
	const FbxTime& GetFrameTime() { return frameTime; }	//1フレームで加算される時間の取得
	void SetStartTime(FbxTime& startTime) { this->startTime = startTime; }
	void SetEndTime(FbxTime& endTime) { this->endTime = endTime; }
	void SetNowTime(FbxTime& currentTime) { this->currentTime = currentTime; }

	const std::vector<std::pair<std::string, DirectX::XMMATRIX>>& GetAffineTrans() { return affineTrans; }	//ボーンの名前とワールド行列
	XMMATRIX GetMatrix() { return matrix; }	//手固定
	const std::vector<DirectX::XMMATRIX>& GetMatRots() { return matRots; }	//ボーンの回転行列
	const XMMATRIX& GetMatRot() { return m_matRot; }
};

