#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "../DirectX/d3dx12.h"
#include <string>
#include "Model.h"
#include "Light.h"

struct ObjectInitData
{
	std::string m_vsEntryPoint = "VSmain";
	std::string m_psEntryPoint = "PSmain";
};

//パイプラインセット
struct ObjPipelineSet
{
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
	//パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate;
};

class Object
{
public:
	//定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;		//ビュープロジェクション行列
		DirectX::XMMATRIX world;		//ワールド行列
		DirectX::XMFLOAT3 cameraPos;	//カメラ座標（ワールド座標）
		float pad;
		DirectX::XMFLOAT4 color;
	};

private:
	//デバイス
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	//コマンドリスト
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	//パイプライン
	static ObjPipelineSet pipelineSet;
	//ライト
	static std::shared_ptr<Light> light;

protected:
	//定数バッファb0
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;
	//色
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	//ローカルスケール
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	//X,Y,Z軸回りのローカル回転角
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	//ローカル座標
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//ローカルワールド変換行列
	DirectX::XMMATRIX matWorld = {};
	//親オブジェクト
	std::shared_ptr<Object> parent = nullptr;
	//モデル
	std::shared_ptr<Model> model = nullptr;
	//ビルボード
	bool isBillboard = false;

private:
	//親子構造用
	DirectX::XMMATRIX matrix = {};
	bool isAffine = false;

	//回転行列
	DirectX::XMMATRIX matRot = {};

public:
	//静的初期化
	static void StaticInit(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	//グラフィックパイプラインの生成
	static ObjPipelineSet CreateGraphicsPipeline(const ObjectInitData& objectInitData);
	//3Dオブジェクト生成
	static std::shared_ptr<Object> Create(std::shared_ptr<Model> model = nullptr);
	//ライト
	static void SetLight(std::shared_ptr<Light> light) {
		Object::light = light;
	}

public:
	Object() {};
	~Object();

	//初期化
	bool Init();

	//毎フレーム処理
	void Update();

	//描画
	void Draw(const ObjPipelineSet& pipelineSet, bool isShadow = true);

public:
	//座標の取得
	const DirectX::XMFLOAT3& GetPosition() { return position; }
	//回転角の取得
	const DirectX::XMFLOAT3& GetRotation() { return rotation; }
	//スケールの取得
	const DirectX::XMFLOAT3& GetScale() { return scale; }
	//色の取得
	const DirectX::XMFLOAT4& GetColor() { return color; }
	//座標の設定
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }
	//回転角の設定
	void SetRotation(DirectX::XMFLOAT3 rotation) { this->rotation = rotation; }
	//スケールの設定
	void SetScale(DirectX::XMFLOAT3 scale) { this->scale = scale; }
	//モデルのセット
	void SetModel(std::shared_ptr<Model> model) { this->model = model; }
	//ビルボードフラグのセット
	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }
	//色設定
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	//ワールド行列取得
	const DirectX::XMMATRIX& GetMatWorld() { return matWorld; }
	//モデルを取得
	inline std::shared_ptr<Model> GetModel() { return model; }
	//アフィン変換行列を乗算する
	void MultiMatrix(DirectX::XMMATRIX matrix)
	{
		isAffine = true;
		this->matrix = matrix;
	}
	const DirectX::XMMATRIX GetMatRot() { return matRot; }	//親子関係は無視した回転行列です

	//スケール抜きワールド行列取得
	const DirectX::XMMATRIX GetMatrix()
	{
		DirectX::XMFLOAT3 l_scale = {};
		l_scale.x = 1 / scale.x;
		l_scale.y = 1 / scale.y;
		l_scale.z = 1 / scale.z;

		DirectX::XMMATRIX l_mat = matWorld;
		l_mat.r[0].m128_f32[0] *= l_scale.x;
		l_mat.r[0].m128_f32[1] *= l_scale.x;
		l_mat.r[0].m128_f32[2] *= l_scale.x;
		l_mat.r[1].m128_f32[0] *= l_scale.y;
		l_mat.r[1].m128_f32[1] *= l_scale.y;
		l_mat.r[1].m128_f32[2] *= l_scale.y;
		l_mat.r[2].m128_f32[0] *= l_scale.z;
		l_mat.r[2].m128_f32[1] *= l_scale.z;
		l_mat.r[2].m128_f32[2] *= l_scale.z;
		return l_mat;
	}
};

