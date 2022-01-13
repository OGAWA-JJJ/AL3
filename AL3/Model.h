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
	static ID3D12Device* device;
	//デスクリプタのサイズ
	static UINT descriptorHandleIncrementSize;

private:
	//名前
	std::string name;
	//メッシュコンテナ
	std::vector<Mesh*> meshes;
	//マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials;
	//デフォルトマテリアル
	Material* defaultMaterial = nullptr;
	//デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;

public:
	//静的初期化
	static void StaticInit(ID3D12Device* device);
	//メッシュ生成
	static Model* CreateFromObj(const std::string& modelname, bool smoothing = false);

public:
	Model();
	~Model();

	//初期化
	void Init(const std::string& modelname, bool smoothing);
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	//マテリアル読込
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//マテリアル登録
	void AddMaterial(Material* material);
	//デスクリプタヒープ生成
	void CreateDescriptorHeap();
	//テクスチャ読込
	void LoadTextures();
};

