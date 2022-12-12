#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"

class Model
{
private:
	static const std::string baseDirectory;

private:
	//デバイス
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	//デスクリプタのサイズ
	static UINT descriptorHandleIncrementSize;

private:
	//名前
	std::string name;
	//メッシュコンテナ
	std::vector<std::shared_ptr<Mesh>> meshes;
	//マテリアルコンテナ
	std::unordered_map<std::string, std::shared_ptr<Material>> materials;
	//デフォルトマテリアル
	std::shared_ptr<Material> defaultMaterial = nullptr;
	//最大サイズの取得用(Scaleは未考慮)
	DirectX::XMFLOAT3 size = { 0,0,0 };

public:
	//静的初期化
	static void StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device);
	//メッシュ生成
	static std::shared_ptr<Model> CreateFromObj(
		const std::string& modelname,
		bool smoothing = false);

public:
	Model();
	~Model();

	//初期化
	void Init(const std::string& modelname, bool smoothing);
	//描画
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	//メッシュコンテナの取得
	inline const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return meshes; }
	//最大サイズの取得用(Scaleは未考慮)
	DirectX::XMFLOAT3 GetModelSize() { return size; }

private:
	//マテリアル読込
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//マテリアル登録
	void AddMaterial(std::shared_ptr<Material> material);
	//テクスチャ読込
	void LoadTextures();
};

