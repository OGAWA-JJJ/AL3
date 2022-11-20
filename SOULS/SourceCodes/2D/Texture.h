#pragma once
#include "IShaderResource.h"

class Texture :public  IShaderResource
{
private:
	ID3D12Resource* m_texture = nullptr;	//テクスチャ。
	D3D12_RESOURCE_DESC m_textureDesc = {};	//テクスチャ情報

public:
	Texture() {}
	~Texture();

	//ファイルからテクスチャをロードするコンストラクタ→Load処理を行う場合
	//explicit Texture(const wchar_t* filePath);
	//メモリからテクスチャを初期化
	//void InitFromMemory(const char* memory, unsigned int size);
	//D3Dリソースからテクスチャを初期化する
	void InitFromD3DResource(ID3D12Resource* texture);
	//SRVに登録
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo);

public:
	//テクスチャが有効か判定
	bool IsValid() const { return m_texture != nullptr; }
	//ポインタの取得
	ID3D12Resource* Get() { return m_texture; }
	//横サイズ取得
	int GetWidth() const { return static_cast<int>(m_textureDesc.Width); }
	//縦サイズ取得
	int GetHeight() const { return static_cast<int>(m_textureDesc.Height); }
	//テクスチャのフォーマットを取得
	DXGI_FORMAT GetFormat() const { return m_textureDesc.Format; }

private:
	//メモリからテクスチャをロード
	//void LoadTextureFromMemory(const char* memory, unsigned int size);
};

