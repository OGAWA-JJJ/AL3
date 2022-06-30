#include "Shape.h"
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <string>
#include <d3dx12.h>
#include "../Input/Input.h"
//#include <dinput.h>

#pragma comment(lib,"d3dcompiler.lib")

const DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	DirectX::XMFLOAT3 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

Shape::Shape()
{
	scale = { 1.0f,1.0f,1.0f };
	rotation = { 0.0f,0.0f,0.0f };  //XYZ軸を基準に何度回転するか
	position = { 0.0f,0.0f,0.0f };
}

void Shape::CreateCube(float r, const wchar_t* filename)
{
	HRESULT result;
	//前
	CubeVertices.push_back({ {-r, -r, -r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {-r,  r, -r}, {}, {0.0f, 0.0f} });
	CubeVertices.push_back({ { r, -r, -r}, {}, {1.0f, 1.0f} });
	CubeVertices.push_back({ { r,  r, -r}, {}, {1.0f, 0.0f} });
	//後
	CubeVertices.push_back({ {-r, -r, r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {-r,  r, r}, {}, {0.0f, 0.0f} });
	CubeVertices.push_back({ { r, -r, r}, {}, {1.0f, 1.0f} });
	CubeVertices.push_back({ { r,  r, r}, {}, {1.0f, 0.0f} });
	//左
	CubeVertices.push_back({ {-r, -r, -r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {-r, -r,  r}, {}, {0.0f, 0.0f} });
	CubeVertices.push_back({ {-r,  r, -r}, {}, {1.0f, 1.0f} });
	CubeVertices.push_back({ {-r,  r,  r}, {}, {1.0f, 0.0f} });
	//右
	CubeVertices.push_back({ { r, -r,  r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ { r, -r, -r}, {}, {0.0f, 0.0f} });
	CubeVertices.push_back({ {r, r, r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {r, r, -r}, {}, {0.0f, 1.0f} });
	//下
	CubeVertices.push_back({ {-r, -r, -r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {-r, -r, r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {r, -r, -r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {r, -r, r}, {}, {0.0f, 1.0f} });
	//上
	CubeVertices.push_back({ {-r, r, -r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {-r, r, r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {r, r, -r}, {}, {0.0f, 1.0f} });
	CubeVertices.push_back({ {r, r, r}, {}, {0.0f, 1.0f} });

	/*インデックスデータ*/
	CubeIndices = {
		//前
		0,1,2,    //三角形1つ目
		2,1,3,    //三角形2つ目

		//後
		5,4,6,    //三角形3つ目
		5,6,7,    //三角形4つ目

		//左
		8,9,10,   //三角形5つ目
		10,9,11,  //三角形6つ目

		//右
		12,13,14, //三角形7つ目
		14,13,15, //三角形8つ目

		//下
		16,17,18, //三角形9つ目
		18,17,19, //三角形10つ目

		//上
		20,21,22, //三角形11つ目
		22,21,23, //三角形12つ目
	};

	/*WICテクスチャのロード*/
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,
		DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0); //生データ抽出

	/*テクスチャバッファの生成*/
	D3D12_HEAP_PROPERTIES texHeapProp{};                       //テクスチャヒープ設定
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//D3D12_RESOURCE_DESC texresDesc{};                          //リソース設定
	//texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2Dテクスチャ用
	//texresDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;        //RGBAフォーマット
	//texresDesc.Width = metadata.width;
	//texresDesc.Height = metadata.width;
	//texresDesc.DepthOrArraySize = 1;
	//texresDesc.MipLevels = 1;
	//texresDesc.SampleDesc.Count = 1;

	D3D12_RESOURCE_DESC texresDesc{};
	texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = (UINT)metadata.height;
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	ID3D12Resource* texbuff = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	////テクスチャバッファへのデータ転送
	//result = texbuff->WriteToSubresource(
	//	0,
	//	nullptr,                        //全領域へコピー
	//	texturedata,                    //元データアドレス
	//	sizeof(XMFLOAT4) * metadata.width,    //1ラインサイズ
	//	sizeof(XMFLOAT4) * texDataCount //全サイズ
	//);

	//テクスチャバッファへのデータ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	//delete[] texturedata;               //元データ解放

	//法線の計算
	for (int i = 0; i < CubeIndices.size() / 3; i++)
	{
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = CubeIndices[i * 3 + 0];
		unsigned short index1 = CubeIndices[i * 3 + 1];
		unsigned short index2 = CubeIndices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		DirectX::XMVECTOR p0 = XMLoadFloat3(&CubeVertices[index0].pos);
		DirectX::XMVECTOR p1 = XMLoadFloat3(&CubeVertices[index1].pos);
		DirectX::XMVECTOR p2 = XMLoadFloat3(&CubeVertices[index2].pos);

		//p0→p1ベクトル、p0→p2ベクトルを計算（ベクトルの減算）
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p1, p0);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		DirectX::XMVECTOR normal = DirectX::XMVector3Cross(v1, v2);

		//正規化
		normal = DirectX::XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&CubeVertices[index0].normal, normal);
		XMStoreFloat3(&CubeVertices[index1].normal, normal);
		XMStoreFloat3(&CubeVertices[index2].normal, normal);
	}

	//UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * CubeVertices.size());
	//UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * (vertices.size()));

	/*頂点バッファの確保*/
	/*GPU側のメモリを確保して、頂点データ用の領域を設定。
	頂点バッファ用GPUリソースの生成*/

	//頂点ヒープ設定
	D3D12_HEAP_PROPERTIES heapprop{};

	//GPUへの転送用
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//深度バッファのリソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WINDOW_WIDTH;                            //レンダーターゲットに合わせる
	depthResDesc.Height = WINDOW_HEIGHT;                          //レンダーターゲットに合わせる
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;                  //深度値フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;      //深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット

	//リソース設定
	ID3D12Resource* depthBuffer = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = DirectXImportant::dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ステンシルビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//リソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	//頂点情報が入る分のサイズ
	resdesc.Width = sizeof(CubeVertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	/*インデックスバッファの確保*/
	//インデックス情報が入る分のサイズ
	resdesc.Width = sizeof(CubeIndices);

	//GPUリソースの生成
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	Vertex* vertMap = nullptr;
	unsigned short* indexMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < CubeVertices.size(); i++) {
		vertMap[i] = CubeVertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	for (int i = 0; i < CubeIndices.size(); i++) {
		indexMap[i] = CubeIndices[i];
	}
	indexBuff->Unmap(0, nullptr);

	/*頂点バッファビューの作成*/
	//頂点バッファをセットするためのGPUコマンド「頂点バッファビューを作成する

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * CubeVertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * CubeIndices.size());

	//頂点バッファビューの作成
	//D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);
	//vbView.SizeInBytes = vertices.size();
	//vbView.StrideInBytes = sizeof(XMFLOAT3);

	/*インデックスバッファビューの作成とセット*/
	//D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;
	//ibView.SizeInBytes = indices.size();

	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダーモデル指定
		/*BasicVS.hlslのmain関数をエントリーポイント
		とする為"main"と文字列指定している。*/
		"VSmain", "vs_5_0",
		//デバッグ用設定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	result = D3DCompileFromFile(
		//シェーダファイル名
		L"BasicGeometryShader.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダモデル指定
		"GSmain", "gs_5_0",
		//デバッグ指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&gsBlob, &errorBlob);

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		//シェーダファイル名
		L"BasicPS.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダモデル指定
		"PSmain", "ps_5_0",
		//デバッグ指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	/*シェーダのエラー内容を表示*/
	/*・頂点シェーダの読み込み
	・ピクセルシェーダの読み込み
	のあとにそれぞれ計2回↓この処理↓を入れる。

	これでhlslファイルに文法的な
	問題があった場合、
	「出力」ウィンドウに
	エラー内容が表示される。*/

	if (FAILED(result))
	{
		//errorBolbからのエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	/*頂点シェーダに渡すための頂点データを整える*/
	/*グラフィックスパイプラインで
	頂点1つ分のデータに何を
	持たせるか自分で
	決めることが出来る。

	ここでは最小限の3D座標だけを
	セットしている。
	今後必要になるごとに行を追加していく。*/
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	/*グラフィックスパイプラインの各ステージの設定をする構造体を用意する。*/
	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	/*頂点シェーダ、ピクセルシェーダをパイプラインに設定*/
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	/*サンプルマスクとラスタライザステートの設定*/

	//標準設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//ポリゴン内塗りつぶし
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//枠線のみ
	/*ワイヤーフレーム→描画が軽い、奥が見える*/
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//深度クリッピングを有効に
	gpipeline.RasterizerState.DepthClipEnable = true;

	/*ブレンドステートを設定*/
	/*gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;*/
		//レンダーターゲットのブレンド設定（8個あるが今は1つしか使わない）

	//デプスステンシルステートの設定
	gpipeline.DepthStencilState.DepthEnable = true;                          //深度テストを行う
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //書き込み許可
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;      //小さければ合格
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                             //深度値フォーマット

	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	//標準設定
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	/*共通設定*/
	//ブレンドを有効にする
	blenddesc.BlendEnable = true;
	//加算
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//ソースの値を100%使う
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//デストの値を0%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	/*加算合成*/
	////加算
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	////ソースの値を100%使う
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	////デストの値を0%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	/*減算合成*/
	////デストからソースを減算
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	////ソースの値を100%使う
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	////デストの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	/*色反転*/
	////加算
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	////1.0f - デストカラーの値
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	////使わない
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;

	/*半透明合成*/
	//加算
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	//ソースのアルファ値
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//1.0f - ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	/*定数バッファの初期化*/
	//定数バッファ用データ構造体
	//struct ConstBufferData {
	//	//色（RGBA）
	//	XMFLOAT4 color;
	//	XMMATRIX mat;   //3D変換行列
	//};

	/*バッファを確保するので、
	新たに専用のデスクリプタヒープを作成する。*/
	//ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};                   //設定構造体
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	descHeapDesc.NumDescriptors = 2;                                //バッファは2つ

	result = DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));               //生成

	//デスクリプタヒープの先頭ハンドルを取得しておく
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle =
		basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	/*定数バッファの作成*/
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbheapprop{};
	//GPUへの転送用
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//256バイトアラインメント
	cbresdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//GPUリソースの生成
	/*ID3D12Resource* constBuff = nullptr;*/
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&cbheapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&cbresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));


	/*定数バッファにデータを転送する*/
	/*ConstBufferData* constMap = nullptr;*/

	//マッピング
	result = constBuff->Map(0, nullptr, (void**)&constMap);

	//RGBA
	constMap->color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//平行投影変換
	/*constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f, WINDOW_WIDTH,
		WINDOW_HEIGHT, 0.0f,
		0.0f, 1.0f
	);*/

	//ワールド変換行列
	matWorld = DirectX::XMMatrixIdentity();

	/*スケーリング（拡縮）*/
	//XMMATRIX matScale;
	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	/*回転*/
	//XMMATRIX matRot; //回転行列
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z)); //Z軸回りに45度回転
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x)); //X軸回りに45度回転
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y)); //Y軸回りに45度回転

	/*平行移動*/
	//XMMATRIX matTrans;
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matPerspective;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;
	float fov;

	eye = { 45,0,-100 };
	target = { 0,0,0 };
	up = { 0,1,0 };
	fov = 60.0f;
	matView = DirectX::XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up));
	matPerspective = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov),
		(float)WINDOW_WIDTH / WINDOW_HEIGHT,
		0.1f, 1000.0f); //前端、奥端

	//Camera camera;
	//constMap->mat = matWorld * camera.ViewMatrix() * camera.PerspectiveMatrix();
	constMap->mat = matWorld * Camera::ViewMatrix() * Camera::PerspectiveMatrix();
	//constMap->mat = matWorld * matView * matPerspective;

	//マッピング解除
	constBuff->Unmap(0, nullptr);

	/*定数バッファビューの作成*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	DirectXImportant::dev->CreateConstantBufferView(
		&cbvDesc, basicHeapHandle);
	/*↑頂点バッファビューと違い、Createする必要がある。↑*/

	//消費した分だけアドレスをずらす
	basicHeapHandle.ptr +=
		DirectXImportant::dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

	/*シェーダーリソースビュー設定*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};             //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;       //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	/*ヒープの2番目にシェーダーリソースビュー作成*/
	DirectXImportant::dev->CreateShaderResourceView(
		texbuff,  //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		basicHeapHandle
	);

	D3D12_DESCRIPTOR_RANGE descRangeCBV{};                    //定数用
	descRangeCBV.NumDescriptors = 1;                          //定数は1つ
	descRangeCBV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descRangeCBV.BaseShaderRegister = 0;                      //0番スロットから
	descRangeCBV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descRangeSRV{};                    //テクスチャ用
	descRangeSRV.NumDescriptors = 1;                          //テクスチャ1つ
	descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //種別はテクスチャ
	descRangeSRV.BaseShaderRegister = 0;                      //0番スロットから
	descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparams[2] = {};
	/*定数用*/
	rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //種別
	rootparams[0].DescriptorTable.pDescriptorRanges = &descRangeCBV;
	rootparams[0].DescriptorTable.NumDescriptorRanges = 1;                    //デスクリプタレンジ数
	rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;             //全てのシェーダから見える
	/*テクスチャ用*/
	rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //種別
	rootparams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;
	rootparams[1].DescriptorTable.NumDescriptorRanges = 1;                    //デスクリプタレンジ数
	rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;             //全てのシェーダから見える

	/*デスクリプタテーブルの初期化*/
	/*定数バッファの情報をCPU/GPU間で整理する為の設定を行う。*/

	/*デスクリプタレンジの設定*/
	D3D12_DESCRIPTOR_RANGE descTblRange{};
	//定数は1つ
	descTblRange.NumDescriptors = 1;
	//種別は定数
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//0番スロットから
	descTblRange.BaseShaderRegister = 0;
	//標準
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/*ルートパラメータの設定*/
	D3D12_ROOT_PARAMETER rootparam = {};
	//種類
	rootparam.ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//デスクリプタレンジ
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
	//デスクリプタレンジ数
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	//全てのシェーダから見える
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	/*頂点レイアウトの設定*/
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	/*図形の形状を三角形に設定*/
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/*その他の設定*/
	//描画対象は1つ
	gpipeline.NumRenderTargets = 1;
	//0~255指定のRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//1ピクセルにつき1回サンプリング
	gpipeline.SampleDesc.Count = 1;

	/*ルートシグネチャの生成*/
	//ID3D12RootSignature* rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.pParameters = rootparams;             //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootparams); //ルートパラメータ数

	//D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle;
	/*定数バッファビュー（先頭）*/
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	//DirectXImportant::DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	/*D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle2;*/
	//DirectXImportant::DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //奥繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;                   //補間しない（ニアレストネイバー）
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                                             //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;          //ピクセルシェーダからのみ可視

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3D10Blob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	result = DirectXImportant::dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release();


	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature.Get();

	/*ルートシグネチャは、テクスチャや定数バッファなど
	シェーダに渡すリソースの情報えおまとめたオブジェクト。

	現時点では該当するリソースは使わないので、デフォルト設定で作成する。*/

	/*パイプラインステートの生成*/
	//ID3D12PipelineState* pipelinestate = nullptr;
	result = DirectXImportant::dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate));
}

void Shape::DrawCube()
{
	HRESULT result = S_OK;

	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));        //Z軸回転
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));        //X軸回転
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));        //Y軸回転
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z); //平行移動行列再計算

	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	//XMFLOAT3 Rotation = { rotation.x,rotation.y,rotation.z };

	if (Input::isKey(DIK_1)) {
		matWorld *= Camera::BillboardMatrix();
	}
	else if (Input::isKey(DIK_2)) {
		matWorld *= Camera::BillboardYMatrix();
	}

	//constMap->mat = matWorld * Camera::FollowCamera(50, Rotation, matRot) * Camera::PerspectiveMatrix();
	constMap->mat = matWorld * Camera::ViewMatrix() * Camera::PerspectiveMatrix();

	Vertex* vertMap = nullptr;
	unsigned short* indexMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < CubeVertices.size(); i++) {
		vertMap[i] = CubeVertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	for (int i = 0; i < CubeIndices.size(); i++) {
		indexMap[i] = CubeIndices[i];
	}
	indexBuff->Unmap(0, nullptr);

	DirectXImportant::cmdList->SetPipelineState(pipelinestate.Get());
	DirectXImportant::cmdList->SetGraphicsRootSignature(rootsignature.Get());

	/*定数バッファのデスクリプタヒープをセットするコマンド*/
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	DirectXImportant::cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//ヒープの先頭が定数バッファ
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(
		0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	//DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*頂点バッファの設定コマンド*/
	DirectXImportant::cmdList->IASetVertexBuffers(0, 1, &vbView);
	/*インデックスバッファの設定コマンド*/
	DirectXImportant::cmdList->IASetIndexBuffer(&ibView);

	/*定数バッファビュー（先頭）*/
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	/*シェーダリソースビュー（定数バッファの次）*/
	gpuDescHandle.ptr += DirectXImportant::dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	/*描画コマンド*/
	//頂点の数、インスタンス数、頂点データのオフセット、インスタンスのオフセット
	//DirectXImportant::cmdList->DrawIndexedInstanced(CubeIndices.size(), 1, 0, 0, 0);
	DirectXImportant::cmdList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Shape::CreateSphere(float r)
{
	HRESULT result = S_OK;

	const int m_iUMax = 24;
	const int m_iVMax = 12;
	const int m_iVertexNum = m_iUMax * (m_iVMax + 1);
	const int m_iIndexNum = 2 * m_iVMax * (m_iUMax + 1);

	for (int v = 0; v <= m_iVMax; v++)
	{
		for (int u = 0; u < m_iUMax; u++)
		{
			double dTheta = DirectX::XMConvertToRadians(180.0f * v / m_iVMax);
			double dPhi = DirectX::XMConvertToRadians(360.0f * u / m_iUMax);
			float fX = static_cast<FLOAT>(sin(dTheta) * cos(dPhi)) * r;
			float fY = static_cast<FLOAT>(cos(dTheta)) * r;
			float fZ = static_cast<FLOAT>(sin(dTheta) * sin(dPhi)) * r;
			SphereVertices.push_back({ {fX, fY, fZ}, {}, {} });
		}
	}

	/*インデックスデータ*/
	int i = 0;
	for (int v = 0; v < m_iVMax; v++)
	{
		for (int u = 0; u <= m_iUMax; u++)
		{
			if (u == m_iUMax)
			{
				SphereIndices[i] = v * m_iUMax;
				i++;
				SphereIndices[i] = (v + 1) * m_iUMax;
				i++;
			}
			else
			{
				SphereIndices[i] = (v * m_iUMax) + u;
				i++;
				SphereIndices[i] = SphereIndices[i - 1] + m_iUMax;
				i++;
			}
		}
	}

	/*WICテクスチャのロード*/
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resources/Slime2.png",
		DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0); //生データ抽出

	/*テクスチャバッファの生成*/
	D3D12_HEAP_PROPERTIES texHeapProp{};                       //テクスチャヒープ設定
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//D3D12_RESOURCE_DESC texresDesc{};                          //リソース設定
	//texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2Dテクスチャ用
	//texresDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;        //RGBAフォーマット
	//texresDesc.Width = texWidth;
	//texresDesc.Height = texWidth;
	//texresDesc.DepthOrArraySize = 1;
	//texresDesc.MipLevels = 1;
	//texresDesc.SampleDesc.Count = 1;

	D3D12_RESOURCE_DESC texresDesc{};
	texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = (UINT)metadata.height;
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	ID3D12Resource* texbuff = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	////テクスチャバッファへのデータ転送
	//result = texbuff->WriteToSubresource(
	//	0,
	//	nullptr,                        //全領域へコピー
	//	texturedata,                    //元データアドレス
	//	sizeof(XMFLOAT4) * texWidth,    //1ラインサイズ
	//	sizeof(XMFLOAT4) * texDataCount //全サイズ
	//);

	//テクスチャバッファへのデータ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	//delete[] texturedata;               //元データ解放

	//法線の計算
	for (int i = 0; i < SphereIndices.size() / 3; i++)
	{
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = SphereIndices[i * 3 + 0];
		unsigned short index1 = SphereIndices[i * 3 + 1];
		unsigned short index2 = SphereIndices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		DirectX::XMVECTOR p0 = XMLoadFloat3(&SphereVertices[index0].pos);
		DirectX::XMVECTOR p1 = XMLoadFloat3(&SphereVertices[index1].pos);
		DirectX::XMVECTOR p2 = XMLoadFloat3(&SphereVertices[index2].pos);

		//p0→p1ベクトル、p0→p2ベクトルを計算（ベクトルの減算）
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p1, p0);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		DirectX::XMVECTOR normal = DirectX::XMVector3Cross(v1, v2);

		//正規化
		normal = DirectX::XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&SphereVertices[index0].normal, normal);
		XMStoreFloat3(&SphereVertices[index1].normal, normal);
		XMStoreFloat3(&SphereVertices[index2].normal, normal);
	}

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * SphereVertices.size());
	//UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * (vertices.size()));

	/*頂点バッファの確保*/
	/*GPU側のメモリを確保して、頂点データ用の領域を設定。
	頂点バッファ用GPUリソースの生成*/

	//頂点ヒープ設定
	D3D12_HEAP_PROPERTIES heapprop{};

	//GPUへの転送用
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//深度バッファのリソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WINDOW_WIDTH;                            //レンダーターゲットに合わせる
	depthResDesc.Height = WINDOW_HEIGHT;                          //レンダーターゲットに合わせる
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;                  //深度値フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;      //深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット

	//リソース設定
	ID3D12Resource* depthBuffer = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = DirectXImportant::dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ステンシルビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//リソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	//頂点情報が入る分のサイズ
	resdesc.Width = sizeof(SphereVertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	//ID3D12Resource* vertBuff = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	/*インデックスバッファの確保*/
	//ID3D12Resource* indexBuff = nullptr;
	//インデックス情報が入る分のサイズ
	resdesc.Width = sizeof(SphereIndices);

	//GPUリソースの生成
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	Vertex* vertMap = nullptr;
	unsigned short* indexMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < SphereVertices.size(); i++) {
		vertMap[i] = SphereVertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	for (int i = 0; i < SphereIndices.size(); i++) {
		indexMap[i] = SphereIndices[i];
	}
	indexBuff->Unmap(0, nullptr);

	/*頂点バッファビューの作成*/
	//頂点バッファをセットするためのGPUコマンド「頂点バッファビューを作成する」

	//頂点バッファビューの作成
	//D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = SphereVertices.size() * sizeof(Vertex);
	vbView.StrideInBytes = sizeof(Vertex);
	//vbView.SizeInBytes = vertices.size();
	//vbView.StrideInBytes = sizeof(XMFLOAT3);

	/*インデックスバッファビューの作成とセット*/
	//D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = SphereIndices.size() * sizeof(unsigned short);
	//ibView.SizeInBytes = indices.size();

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダーモデル指定
		/*BasicVS.hlslのmain関数をエントリーポイント
		とする為"main"と文字列指定している。*/
		"VSmain", "vs_5_0",
		//デバッグ用設定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		//シェーダファイル名
		L"BasicPS.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダモデル指定
		"PSmain", "ps_5_0",
		//デバッグ指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	/*シェーダのエラー内容を表示*/
	/*・頂点シェーダの読み込み
	・ピクセルシェーダの読み込み
	のあとにそれぞれ計2回↓この処理↓を入れる。

	これでhlslファイルに文法的な
	問題があった場合、
	「出力」ウィンドウに
	エラー内容が表示される。*/

	if (FAILED(result))
	{
		//errorBolbからのエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	/*頂点シェーダに渡すための頂点データを整える*/
	/*グラフィックスパイプラインで
	頂点1つ分のデータに何を
	持たせるか自分で
	決めることが出来る。

	ここでは最小限の3D座標だけを
	セットしている。
	今後必要になるごとに行を追加していく。*/
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	/*グラフィックスパイプラインの各ステージの設定をする構造体を用意する。*/
	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	/*頂点シェーダ、ピクセルシェーダをパイプラインに設定*/
	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

	/*サンプルマスクとラスタライザステートの設定*/

	//標準設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	//ポリゴン内塗りつぶし
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//枠線のみ
	/*ワイヤーフレーム→描画が軽い、奥が見える*/
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//深度クリッピングを有効に
	gpipeline.RasterizerState.DepthClipEnable = true;

	/*ブレンドステートを設定*/
	/*gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;*/
		//レンダーターゲットのブレンド設定（8個あるが今は1つしか使わない）

	//デプスステンシルステートの設定
	gpipeline.DepthStencilState.DepthEnable = true;                          //深度テストを行う
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //書き込み許可
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;      //小さければ合格
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                             //深度値フォーマット

	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	//標準設定
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	/*共通設定*/
	//ブレンドを有効にする
	blenddesc.BlendEnable = true;
	//加算
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//ソースの値を100%使う
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//デストの値を0%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	/*加算合成*/
	////加算
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	////ソースの値を100%使う
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	////デストの値を0%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	/*減算合成*/
	////デストからソースを減算
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	////ソースの値を100%使う
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	////デストの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	/*色反転*/
	////加算
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	////1.0f - デストカラーの値
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	////使わない
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;

	/*半透明合成*/
	//加算
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	//ソースのアルファ値
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//1.0f - ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	/*定数バッファの初期化*/
	//定数バッファ用データ構造体
	//struct ConstBufferData {
	//	//色（RGBA）
	//	XMFLOAT4 color;
	//	XMMATRIX mat;   //3D変換行列
	//};

	/*バッファを確保するので、
	新たに専用のデスクリプタヒープを作成する。*/
	//ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};                   //設定構造体
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	descHeapDesc.NumDescriptors = 2;                                //バッファは2つ

	result = DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));               //生成

	//デスクリプタヒープの先頭ハンドルを取得しておく
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle =
		basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	/*定数バッファの作成*/
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbheapprop{};
	//GPUへの転送用
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//256バイトアラインメント
	cbresdesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//GPUリソースの生成
	//ID3D12Resource* constBuff = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&cbheapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&cbresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));


	/*定数バッファにデータを転送する*/
	//ConstBufferData* constMap = nullptr;

	//マッピング
	result = constBuff->Map(0, nullptr, (void**)&constMap);

	//RGBA
	constMap->color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//平行投影変換
	/*constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f, WINDOW_WIDTH,
		WINDOW_HEIGHT, 0.0f,
		0.0f, 1.0f
	);*/

	//ワールド変換行列
	matWorld = DirectX::XMMatrixIdentity();

	/*スケーリング（拡縮）*/
	//XMMATRIX matScale;
	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	/*回転*/
	//XMMATRIX matRot; //回転行列
	matRot = DirectX::XMMatrixIdentity();
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z)); //Z軸回りに45度回転
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x)); //X軸回りに45度回転
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y)); //Y軸回りに45度回転

	/*平行移動*/
	//XMMATRIX matTrans;
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	//->mat = matWorld * Camera::ViewMatrix() * Camera::PerspectiveMatrix();

	//マッピング解除
	constBuff->Unmap(0, nullptr);

	/*定数バッファビューの作成*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	DirectXImportant::dev->CreateConstantBufferView(
		&cbvDesc, basicHeapHandle);
	/*↑頂点バッファビューと違い、Createする必要がある。↑*/

	//消費した分だけアドレスをずらす
	basicHeapHandle.ptr +=
		DirectXImportant::dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

	/*シェーダーリソースビュー設定*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};             //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;       //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	/*ヒープの2番目にシェーダーリソースビュー作成*/
	DirectXImportant::dev->CreateShaderResourceView(
		texbuff,  //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		basicHeapHandle
	);

	D3D12_DESCRIPTOR_RANGE descRangeCBV{};                    //定数用
	descRangeCBV.NumDescriptors = 1;                          //定数は1つ
	descRangeCBV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descRangeCBV.BaseShaderRegister = 0;                      //0番スロットから
	descRangeCBV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descRangeSRV{};                    //テクスチャ用
	descRangeSRV.NumDescriptors = 1;                          //テクスチャ1つ
	descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //種別はテクスチャ
	descRangeSRV.BaseShaderRegister = 0;                      //0番スロットから
	descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparams[2] = {};
	/*定数用*/
	rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //種別
	rootparams[0].DescriptorTable.pDescriptorRanges = &descRangeCBV;
	rootparams[0].DescriptorTable.NumDescriptorRanges = 1;                    //デスクリプタレンジ数
	rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;             //全てのシェーダから見える
	/*テクスチャ用*/
	rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //種別
	rootparams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;
	rootparams[1].DescriptorTable.NumDescriptorRanges = 1;                    //デスクリプタレンジ数
	rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;             //全てのシェーダから見える

	/*デスクリプタテーブルの初期化*/
	/*定数バッファの情報をCPU/GPU間で整理する為の設定を行う。*/

	/*デスクリプタレンジの設定*/
	D3D12_DESCRIPTOR_RANGE descTblRange{};
	//定数は1つ
	descTblRange.NumDescriptors = 1;
	//種別は定数
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//0番スロットから
	descTblRange.BaseShaderRegister = 0;
	//標準
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/*ルートパラメータの設定*/
	D3D12_ROOT_PARAMETER rootparam = {};
	//種類
	rootparam.ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//デスクリプタレンジ
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
	//デスクリプタレンジ数
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	//全てのシェーダから見える
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	/*頂点レイアウトの設定*/
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	/*図形の形状を三角形に設定*/
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/*その他の設定*/
	//描画対象は1つ
	gpipeline.NumRenderTargets = 1;
	//0~255指定のRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//1ピクセルにつき1回サンプリング
	gpipeline.SampleDesc.Count = 1;

	/*ルートシグネチャの生成*/
	//ID3D12RootSignature* rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.pParameters = rootparams;             //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootparams); //ルートパラメータ数

	//D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle;
	/*定数バッファビュー（先頭）*/
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	//DirectXImportant::DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	/*D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle2;*/
	//DirectXImportant::DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //奥繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;                   //補間しない（ニアレストネイバー）
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                                             //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;          //ピクセルシェーダからのみ可視

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3D10Blob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	result = DirectXImportant::dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release();


	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature.Get();

	/*ルートシグネチャは、テクスチャや定数バッファなど
	シェーダに渡すリソースの情報えおまとめたオブジェクト。

	現時点では該当するリソースは使わないので、デフォルト設定で作成する。*/

	/*パイプラインステートの生成*/
	//ID3D12PipelineState* pipelinestate = nullptr;
	result = DirectXImportant::dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate));
}

void Shape::DrawSphere()
{
	HRESULT result = S_OK;

	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));        //Z軸回転
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));        //X軸回転
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));        //Y軸回転
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z); //平行移動行列再計算

	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	constMap->mat = matWorld * Camera::ViewMatrix() * Camera::PerspectiveMatrix();

	Vertex* vertMap = nullptr;
	unsigned short* indexMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < SphereVertices.size(); i++) {
		vertMap[i] = SphereVertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	for (int i = 0; i < SphereIndices.size(); i++) {
		indexMap[i] = SphereIndices[i];
	}
	indexBuff->Unmap(0, nullptr);

	DirectXImportant::cmdList->SetPipelineState(pipelinestate.Get());
	DirectXImportant::cmdList->SetGraphicsRootSignature(rootsignature.Get());

	/*定数バッファのデスクリプタヒープをセットするコマンド*/
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	DirectXImportant::cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//ヒープの先頭が定数バッファ
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(
		0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*頂点バッファの設定コマンド*/
	DirectXImportant::cmdList->IASetVertexBuffers(0, 1, &vbView);
	/*インデックスバッファの設定コマンド*/
	DirectXImportant::cmdList->IASetIndexBuffer(&ibView);

	/*定数バッファビュー（先頭）*/
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	/*シェーダリソースビュー（定数バッファの次）*/
	gpuDescHandle.ptr += DirectXImportant::dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	/*描画コマンド*/
	//頂点の数、インスタンス数、頂点データのオフセット、インスタンスのオフセット
	DirectXImportant::cmdList->DrawIndexedInstanced(SphereIndices.size(), 1, 0, 0, 0);
}

void Shape::CreateGeometry(const wchar_t* filename)
{
	HRESULT result;

	/*PointVertices.push_back({ {
			0.0f,
			0.0f,
			0.0f
		} });*/

	for (int i = 0; i < MAX_VERTEX_COUNT; i++) {
		const float md_width = 10.0f;
		PointVertices.push_back({ {
				(float)rand() / RAND_MAX * md_width - md_width / 2.0f,
				(float)rand() / RAND_MAX * md_width - md_width / 2.0f,
				(float)rand() / RAND_MAX * md_width - md_width / 2.0f
			},
			{0},
			{}
			});
	}

	/*for (int i = 0; i < 10; i++) {
		const float md_pos = 10.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		const float md_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		const float md_acc = 0.001f;
		XMFLOAT3 acc{};
		acc.y = (float)rand() / RAND_MAX * md_acc;
		Add(600, pos, vel, acc);
	}*/

	particles.resize(10);

	/*WICテクスチャのロード*/
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,
		DirectX::WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const DirectX::Image* img = scratchImg.GetImage(0, 0, 0); //生データ抽出

	/*テクスチャバッファの生成*/
	D3D12_HEAP_PROPERTIES texHeapProp{};                       //テクスチャヒープ設定
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//D3D12_RESOURCE_DESC texresDesc{};                          //リソース設定
	//texresDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //2Dテクスチャ用
	//texresDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;        //RGBAフォーマット
	//texresDesc.Width = metadata.width;
	//texresDesc.Height = metadata.width;
	//texresDesc.DepthOrArraySize = 1;
	//texresDesc.MipLevels = 1;
	//texresDesc.SampleDesc.Count = 1;

	D3D12_RESOURCE_DESC texresDesc{};
	texresDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texresDesc.Format = metadata.format;
	texresDesc.Width = metadata.width;
	texresDesc.Height = (UINT)metadata.height;
	texresDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	texresDesc.MipLevels = (UINT16)metadata.mipLevels;
	texresDesc.SampleDesc.Count = 1;

	ID3D12Resource* texbuff = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));

	////テクスチャバッファへのデータ転送
	//result = texbuff->WriteToSubresource(
	//	0,
	//	nullptr,                        //全領域へコピー
	//	texturedata,                    //元データアドレス
	//	sizeof(XMFLOAT4) * metadata.width,    //1ラインサイズ
	//	sizeof(XMFLOAT4) * texDataCount //全サイズ
	//);

	//テクスチャバッファへのデータ転送
	result = texbuff->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	//delete[] texturedata;               //元データ解放

	//UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * CubeVertices.size());
	//UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * (vertices.size()));

	/*頂点バッファの確保*/
	/*GPU側のメモリを確保して、頂点データ用の領域を設定。
	頂点バッファ用GPUリソースの生成*/

	//頂点ヒープ設定
	D3D12_HEAP_PROPERTIES heapprop{};

	//GPUへの転送用
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//深度バッファのリソース設定
	D3D12_RESOURCE_DESC depthResDesc{};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = WINDOW_WIDTH;                            //レンダーターゲットに合わせる
	depthResDesc.Height = WINDOW_HEIGHT;                          //レンダーターゲットに合わせる
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;                  //深度値フォーマット
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; //デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;      //深度値1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; //深度値フォーマット

	//リソース設定
	ID3D12Resource* depthBuffer = nullptr;
	result = DirectXImportant::dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = DirectXImportant::dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//深度ステンシルビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//リソース設定
	D3D12_RESOURCE_DESC resdesc{};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	//頂点情報が入る分のサイズ
	resdesc.Width = sizeof(PointVertices);
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//GPUリソースの生成
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	GSVertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < PointVertices.size(); i++) {
		vertMap[i] = PointVertices[i];
	}
	vertBuff->Unmap(0, nullptr);

	/*頂点バッファビューの作成*/
	//頂点バッファをセットするためのGPUコマンド「頂点バッファビューを作成する
	UINT sizeVB = static_cast<UINT>(sizeof(GSVertex) * PointVertices.size());

	//頂点バッファビューの作成
	//D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(GSVertex);
	//vbView.SizeInBytes = vertices.size();
	//vbView.StrideInBytes = sizeof(XMFLOAT3);

	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> gsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダーモデル指定
		/*BasicVS.hlslのmain関数をエントリーポイント
		とする為"main"と文字列指定している。*/
		"VSmain", "vs_5_0",
		//デバッグ用設定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	result = D3DCompileFromFile(
		//シェーダファイル名
		L"BasicGeometryShader.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダモデル指定
		"GSmain", "gs_5_0",
		//デバッグ指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&gsBlob, &errorBlob);

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		//シェーダファイル名
		L"BasicPS.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダモデル指定
		"PSmain", "ps_5_0",
		//デバッグ指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	/*シェーダのエラー内容を表示*/
	/*・頂点シェーダの読み込み
	・ピクセルシェーダの読み込み
	のあとにそれぞれ計2回↓この処理↓を入れる。

	これでhlslファイルに文法的な
	問題があった場合、
	「出力」ウィンドウに
	エラー内容が表示される。*/

	if (FAILED(result))
	{
		//errorBolbからのエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	/*頂点シェーダに渡すための頂点データを整える*/
	/*グラフィックスパイプラインで
	頂点1つ分のデータに何を
	持たせるか自分で
	決めることが出来る。

	ここでは最小限の3D座標だけを
	セットしている。
	今後必要になるごとに行を追加していく。*/
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	/*グラフィックスパイプラインの各ステージの設定をする構造体を用意する。*/
	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	/*頂点シェーダ、ピクセルシェーダをパイプラインに設定*/
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	/*サンプルマスクとラスタライザステートの設定*/

	//標準設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//ポリゴン内塗りつぶし
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//枠線のみ
	/*ワイヤーフレーム→描画が軽い、奥が見える*/
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//深度クリッピングを有効に
	gpipeline.RasterizerState.DepthClipEnable = true;

	/*ブレンドステートを設定*/
	/*gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;*/
		//レンダーターゲットのブレンド設定（8個あるが今は1つしか使わない）

	//デプスステンシルステートの設定
	gpipeline.DepthStencilState.DepthEnable = true;                          //深度テストを行う
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //書き込み許可
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;      //小さければ合格
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;                             //深度値フォーマット

	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	//標準設定
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	/*共通設定*/
	//ブレンドを有効にする
	blenddesc.BlendEnable = true;
	//加算
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//ソースの値を100%使う
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//デストの値を0%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	/*加算合成*/
	//加算
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	//ソースの値を100%使う
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	//デストの値を0%使う
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	/*減算合成*/
	////デストからソースを減算
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	////ソースの値を100%使う
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;
	////デストの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;

	/*色反転*/
	////加算
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	////1.0f - デストカラーの値
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	////使わない
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;

	/*半透明合成*/
	////加算
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	////ソースのアルファ値
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	////1.0f - ソースのアルファ値
	//blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	/*定数バッファの初期化*/
	//定数バッファ用データ構造体
	//struct ConstBufferData {
	//	//色（RGBA）
	//	XMFLOAT4 color;
	//	XMMATRIX mat;   //3D変換行列
	//};

	/*バッファを確保するので、
	新たに専用のデスクリプタヒープを作成する。*/
	//ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};                   //設定構造体
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	descHeapDesc.NumDescriptors = 2;                                //バッファは2つ

	result = DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));               //生成

	//デスクリプタヒープの先頭ハンドルを取得しておく
	D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle =
		basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	/*定数バッファの作成*/
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbheapprop{};
	//GPUへの転送用
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//256バイトアラインメント
	cbresdesc.Width = (sizeof(GSConstBufferData) + 0xff) & ~0xff;
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//GPUリソースの生成
	/*ID3D12Resource* constBuff = nullptr;*/
	result = DirectXImportant::dev->CreateCommittedResource(
		//ヒープ設定
		&cbheapprop,
		D3D12_HEAP_FLAG_NONE,
		//リソース設定
		&cbresdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));


	/*定数バッファにデータを転送する*/
	//ConstBufferData* constMap = nullptr;

	//マッピング
	//result = constBuff->Map(0, nullptr, (void**)&GSconstMap);

	//マッピング解除
	//constBuff->Unmap(0, nullptr);

	/*定数バッファビューの作成*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;

	DirectXImportant::dev->CreateConstantBufferView(
		&cbvDesc, basicHeapHandle);
	/*↑頂点バッファビューと違い、Createする必要がある。↑*/

	//消費した分だけアドレスをずらす
	basicHeapHandle.ptr +=
		DirectXImportant::dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);

	/*シェーダーリソースビュー設定*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};             //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;       //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	/*ヒープの2番目にシェーダーリソースビュー作成*/
	DirectXImportant::dev->CreateShaderResourceView(
		texbuff,  //ビューと関連付けるバッファ
		&srvDesc, //テクスチャ設定情報
		basicHeapHandle
	);

	D3D12_DESCRIPTOR_RANGE descRangeCBV{};                    //定数用
	descRangeCBV.NumDescriptors = 1;                          //定数は1つ
	descRangeCBV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descRangeCBV.BaseShaderRegister = 0;                      //0番スロットから
	descRangeCBV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descRangeSRV{};                    //テクスチャ用
	descRangeSRV.NumDescriptors = 1;                          //テクスチャ1つ
	descRangeSRV.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //種別はテクスチャ
	descRangeSRV.BaseShaderRegister = 0;                      //0番スロットから
	descRangeSRV.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparams[2] = {};
	/*定数用*/
	rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //種別
	rootparams[0].DescriptorTable.pDescriptorRanges = &descRangeCBV;
	rootparams[0].DescriptorTable.NumDescriptorRanges = 1;                    //デスクリプタレンジ数
	rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;             //全てのシェーダから見える
	/*テクスチャ用*/
	rootparams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //種別
	rootparams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV;
	rootparams[1].DescriptorTable.NumDescriptorRanges = 1;                    //デスクリプタレンジ数
	rootparams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;             //全てのシェーダから見える

	/*デスクリプタテーブルの初期化*/
	/*定数バッファの情報をCPU/GPU間で整理する為の設定を行う。*/

	/*デスクリプタレンジの設定*/
	D3D12_DESCRIPTOR_RANGE descTblRange{};
	//定数は1つ
	descTblRange.NumDescriptors = 1;
	//種別は定数
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//0番スロットから
	descTblRange.BaseShaderRegister = 0;
	//標準
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/*ルートパラメータの設定*/
	D3D12_ROOT_PARAMETER rootparam = {};
	//種類
	rootparam.ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//デスクリプタレンジ
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
	//デスクリプタレンジ数
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	//全てのシェーダから見える
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	/*頂点レイアウトの設定*/
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	/*図形の形状をPointに設定*/
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	/*その他の設定*/
	//描画対象は1つ
	gpipeline.NumRenderTargets = 1;
	//0~255指定のRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//1ピクセルにつき1回サンプリング
	gpipeline.SampleDesc.Count = 1;

	/*ルートシグネチャの生成*/
	//ID3D12RootSignature* rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignatureDesc.pParameters = rootparams;             //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootparams); //ルートパラメータ数

	//D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle;
	/*定数バッファビュー（先頭）*/
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	//DirectXImportant::DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	/*D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle2;*/
	//DirectXImportant::DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //奥繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;                   //補間しない（ニアレストネイバー）
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                                             //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;          //ピクセルシェーダからのみ可視

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	ID3D10Blob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	result = DirectXImportant::dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release();


	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature.Get();

	/*ルートシグネチャは、テクスチャや定数バッファなど
	シェーダに渡すリソースの情報えおまとめたオブジェクト*/

	/*パイプラインステートの生成*/
	//ID3D12PipelineState* pipelinestate = nullptr;
	result = DirectXImportant::dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&pipelinestate));
}

void Shape::DrawGeometry()
{
	HRESULT result = S_OK;

	matScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation.z));        //Z軸回転
	matRot *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rotation.x));        //X軸回転
	matRot *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rotation.y));        //Y軸回転
	matTrans = DirectX::XMMatrixTranslation(position.x, position.y, position.z); //平行移動行列再計算

	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	if (Input::isKey(DIK_1)) {
		matWorld *= Camera::BillboardMatrix();
	}
	else if (Input::isKey(DIK_2)) {
		matWorld *= Camera::BillboardYMatrix();
	}

	//寿命が尽きたパーティクルを削除
	particles.remove_if(
		[](Particle& x) {
			return x.frame >= x.num_frame;
		}
	);

	//パーティクル更新
	for (std::forward_list<Particle>::iterator it = particles.begin();
		it != particles.end();
		it++) {
		it->frame++;
		it->vel = it->vel + it->accel;
		it->pos = it->pos + it->vel;

		float f = (float)it->num_frame / it->frame;
		it->scale = (it->e_scale - it->s_scale) / f;
		it->scale += it->s_scale;
	}

	//頂点バッファへデータ転送
	GSVertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		for (std::forward_list<Particle>::iterator it = particles.begin();
			it != particles.end();
			it++) {
			vertMap->pos = it->pos;
			vertMap->scale = it->scale;
			vertMap++;
		}
		vertBuff->Unmap(0, nullptr);
	}

	//定数バッファへデータ転送
	GSConstBufferData* GSconstMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&GSconstMap);
	DirectX::XMMATRIX camera = Camera::ViewMatrix();
	DirectX::XMMATRIX shisuidai = Camera::PerspectiveMatrix();
	/*GSconstMap->mat = Camera::ViewMatrix() * Camera::PerspectiveMatrix();*/
	GSconstMap->mat = camera * shisuidai;
	GSconstMap->matBillBoard = Camera::BillboardMatrix();
	constBuff->Unmap(0, nullptr);

	/*GSVertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	for (int i = 0; i < PointVertices.size(); i++) {
		vertMap[i] = PointVertices[i];
	}
	vertBuff->Unmap(0, nullptr);*/

	DirectXImportant::cmdList->SetPipelineState(pipelinestate.Get());
	DirectXImportant::cmdList->SetGraphicsRootSignature(rootsignature.Get());

	/*定数バッファのデスクリプタヒープをセットするコマンド*/
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap.Get() };
	DirectXImportant::cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//ヒープの先頭が定数バッファ
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(
		0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	//DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	/*頂点バッファの設定コマンド*/
	DirectXImportant::cmdList->IASetVertexBuffers(0, 1, &vbView);
	/*インデックスバッファの設定コマンド*/
	//DirectXImportant::cmdList->IASetIndexBuffer(&ibView);

	/*定数バッファビュー（先頭）*/
	gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();

	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);

	/*シェーダリソースビュー（定数バッファの次）*/
	gpuDescHandle.ptr += DirectXImportant::dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

	/*描画コマンド*/
	//頂点の数、インスタンス数、頂点データのオフセット、インスタンスのオフセット
	//DirectXImportant::cmdList->DrawInstanced(PointVertices.size(), 1, 0, 0);
	DirectXImportant::cmdList->DrawInstanced((UINT)std::distance(particles.begin(), particles.end()), 1, 0, 0);
}

void Shape::Add(int life, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 velocity, DirectX::XMFLOAT3 accel, float start_scale, float end_scale)
{
	particles.emplace_front();
	Particle& p = particles.front();
	p.pos = position;
	p.vel = velocity;
	p.accel = accel;
	p.num_frame = life;
}

Shape* Shape::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Shape* shape = new Shape();
	if (shape == nullptr) {
		return nullptr;
	}

	return shape;
}
