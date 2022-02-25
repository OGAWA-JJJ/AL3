#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <vector>
#include <unordered_map>
#include "Material.h"

class Mesh
{
public:
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};

private:
	//デバイス
	static ID3D12Device* device;

private:
	//名前
	std::string name;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	//頂点インデックス配列
	std::vector<unsigned short> indices;
	//マテリアル
	Material* material = nullptr;
	//頂点法線スムーシング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;

public:
	//静的初期化
	static void StaticInit(ID3D12Device* device);

public:
	Mesh();
	~Mesh();

	//名前をセット
	void SetName(const std::string& name);
	//頂点データの追加
	void AddVertex(const VertexPosNormalUv& vertex);
	//頂点インデックスの追加
	void AddIndex(unsigned short index);
	//マテリアルの割り当て
	void SetMaterial(Material* material);
	//バッファの生成
	void CreateBuffers();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//エッジ平滑化データの追加
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);
	//平滑化された頂点法線の計算
	void CalculateSmoothedVertexNormals();

public:
	//名前を取得
	const std::string& GetName() { return name; }
	//頂点データの数を取得
	inline size_t GetVertexCount() { return vertices.size(); }
	//マテリアルの取得
	Material* GetMaterial() { return material; }
	//頂点バッファ取得
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }
	//インデックスバッファ取得
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
	//頂点配列の取得
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }
	//インデックス配列の取得
	inline const std::vector<unsigned short>& GetIndices() { return indices; }
};

