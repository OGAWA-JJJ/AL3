#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "Model.h"
#include "Light.h"
#include "../Math/CollisionInfo.h"

class BaseCollider;

class Object
{
public:
	//パイプラインセット
	struct PipelineSet
	{
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootsignature;
		//パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelinestate;
	};

	//定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX viewproj; //ビュープロジェクション行列
		XMMATRIX world; //ワールド行列
		XMFLOAT3 cameraPos; //カメラ座標（ワールド座標）
	};

private:
	//デバイス
	static ID3D12Device* device;
	//コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	//パイプライン
	static PipelineSet pipelineSet;
	//ライト
	static Light* light;

protected:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;
	//色
	XMFLOAT4 color = { 1,1,1,1 };
	//ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	//X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	//ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクト
	Object* parent = nullptr;
	//モデル
	Model* model = nullptr;
	//ビルボード
	bool isBillboard = false;

	//クラス名
	const char* name = nullptr;
	//コライダー
	BaseCollider* collider = nullptr;

public:
	//静的初期化
	static void StaticInit(ID3D12Device* device);
	//グラフィックパイプラインの生成
	static void CreateGraphicsPipeline();
	//描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	//描画後処理
	static void PostDraw();
	//3Dオブジェクト生成
	static Object* Create(Model* model = nullptr);
	//ライト
	static void SetLight(Light* light) {
		Object::light = light;
	}

public:
	Object() = default;
	virtual ~Object();

	//初期化
	//bool Init();
	virtual bool Init();

	//毎フレーム処理
	//void Update();
	virtual void Update();

	//描画
	//void Draw();
	virtual void Draw();

public:
	//座標の取得
	const XMFLOAT3& GetPosition() { return position; }
	//回転角の取得
	const XMFLOAT3& GetRotation() { return rotation; }
	//座標の設定
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	//回転角の設定
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	//スケールの設定
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	//モデルのセット
	void SetModel(Model* model) { this->model = model; }
	//ビルボードフラグのセット
	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }
	//色設定
	void SetColor(XMFLOAT4 color) { this->color = color; }

	//ワールド行列取得
	const XMMATRIX& GetMatWorld() { return matWorld; }
	//コライダーセット
	void SetCollider(BaseCollider* collider);
	//衝突時コールバック
	virtual void OnCollision(const CollisionInfo& info) {}
	//モデルを取得
	inline Model* GetModel() { return model; }
};

