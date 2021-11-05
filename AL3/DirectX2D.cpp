#include "DirectX2D.h"
#include <cassert>

DirectX2D::DirectX2D()
{
	/*SpriteDraw 初期化*/
	sprData.spriteRotation = 0.0f;
	sprData.spriteMatProjection = XMMatrixOrthographicOffCenterLH(
		0.0f,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0.0f,
		0.0f,
		1.0f);
	//sprData.spriteMatProjection= XMMatrixPerspectiveFovLH(
	//	XMConvertToRadians(60.0f),
	//	(float)WINDOW_WIDTH / WINDOW_HEIGHT,
	//	0.1f, 1000.0f); //前端、奥端
	sprData.spritePosition = { 0.0f,0.0f,0.0f };
	sprData.spriteColor = { 1.0f,1.0f,1.0f,1.0f };
}

void DirectX2D::VSShaderCompile()
{
	HRESULT result;

	/*頂点シェーダの読み込みとコンパイル*/
	result = D3DCompileFromFile(
		L"SpriteVertexShader.hlsl",
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
}

void DirectX2D::PSShaderCompile()
{
	HRESULT result;

	/*ピクセルシェーダの読み込みとコンパイル*/
	result = D3DCompileFromFile(
		//シェーダファイル名
		L"SpritePixelShader.hlsl",
		nullptr,
		//インクルード可能にする
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//エントリーポイント名、シェーダモデル指定
		"PSmain", "ps_5_0",
		//デバッグ指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

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
}

HRESULT DirectX2D::SpriteLoadTexture(UINT texnumber, const wchar_t* filename)
{
	HRESULT result;

	/*WICテクスチャのロード*/
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const Image* img = scratchImg.GetImage(0, 0, 0); //生データ抽出

	/*リソース設定*/
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	/*テクスチャ用バッファの生成*/
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, //テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&spriteTexBuff[texnumber]));

	/*テクスチャバッファへのデータ転送*/
	result = spriteTexBuff[texnumber]->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	/*シェーダーリソースビュー設定*/
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};             //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;       //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	/*シェーダーリソースビュー作成*/
	DirectXImportant::dev->CreateShaderResourceView(
		spriteTexBuff[texnumber].Get(), //ビューと関連付けるバッファ
		&srvDesc,                       //テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			spriteDescHeap->GetCPUDescriptorHandleForHeapStart(),
			texnumber,
			DirectXImportant::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	return S_OK;
}

void DirectX2D::InputLayout()
{
	/*頂点レイアウト*/
	inputLayout[0] = {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
	};
	inputLayout[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0
	};

	/*頂点レイアウトの設定*/
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	/*図形の形状を三角形に設定*/
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/*その他の設定*/
	gpipeline.NumRenderTargets = 1;                       //描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; //0~255指定のRGBA
	gpipeline.SampleDesc.Count = 1;                       //1ピクセルにつき1回サンプリング
}

void DirectX2D::SettingPipelineState()
{
	/*頂点シェーダ、ピクセルシェーダをパイプラインに設定*/
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	/*サンプルマスクとラスタライザステートの設定*/
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;                        //標準設定
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);      //標準設定（背面カリング、塗りつぶし、深度クリッピング有効）
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;               //背面カリング
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); //標準設定（深度テストを行う、書き込み許可、深度が小さければ合格）
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;    //常に上書きルール

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

	gpipeline.BlendState.AlphaToCoverageEnable = true;
}

void DirectX2D::RootSignature()
{
	HRESULT result;

	/*デスクリプタレンジ*/
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};                   //設定構造体
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	descHeapDesc.NumDescriptors = 3;                                //バッファは2つ

	result = DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapDesc, IID_PPV_ARGS(&spriteDescHeap));              //生成

	//デスクリプタヒープの先頭ハンドルを取得しておく
	/*D3D12_CPU_DESCRIPTOR_HANDLE */basicHeapHandle =
		spriteDescHeap->GetCPUDescriptorHandleForHeapStart();

	/*ルートパラメータの生成*/
	CD3DX12_ROOT_PARAMETER rootparams[2];

	/*定数用*/
	rootparams[0].InitAsConstantBufferView(
		0,
		0,
		D3D12_SHADER_VISIBILITY_ALL);

	/*テクスチャ用*/
	rootparams[1].InitAsDescriptorTable(
		1,
		&descRangeSRV,
		D3D12_SHADER_VISIBILITY_ALL);

	/*デスクリプタテーブルの初期化*/
	/*定数バッファの情報をCPU/GPU間で整理する為の設定を行う。*/

	/*デスクリプタレンジの設定*/
	CD3DX12_DESCRIPTOR_RANGE descTblRange{};
	descTblRange.NumDescriptors = 1;                          //定数は1つ
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descTblRange.BaseShaderRegister = 0;                      //0番スロットから
	descTblRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;                 //標準

	/*ルートパラメータの設定*/
	CD3DX12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;              //種類
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange; //デスクリプタレンジ
	rootparam.DescriptorTable.NumDescriptorRanges = 1;           //デスクリプタレンジ数
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;    //全てのシェーダから見える

	/*サンプラーデスク*/
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                //奥繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;                   //補間しない（ニアレストネイバー）
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                                             //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;          //ピクセルシェーダからのみ可視
	//CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	/*ルートシグネチャの生成*/
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc; //ルートシグネチャの設定
	rootSignatureDesc.Init_1_0(
		_countof(rootparams),
		rootparams,
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;

	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);                                         //バージョン自動判定でのシリアライズ
	//assert(SUCCEEDED(DirectXImportant::result));

	result = DirectXImportant::dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&spriteRootSignature));              //ルートシグネチャの生成
	//assert(SUCCEEDED(DirectXImportant::result));

	gpipeline.pRootSignature = spriteRootSignature.Get(); //パイプラインにルートシグネチャをセット
}

void DirectX2D::CreatePipelineState()
{
	HRESULT result;

	//ID3D12Device* d = DirectXImportant::dev.Get();

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC lpipeline{};

	//HRESULT result = DirectXImportant::dev->CreateGraphicsPipelineState(
	//	&lpipeline,
	//	IID_PPV_ARGS(&pipelinestate));


	result = DirectXImportant::dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&spritePipelineState));
	//assert(SUCCEEDED(DirectXImportant::result));

	const int spriteSRVCount = 512;
	//デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = DirectXImportant::dev->CreateDescriptorHeap(
		&descHeapDesc,
		IID_PPV_ARGS(&spriteDescHeap));
}

void DirectX2D::PipelineStateObjInit()
{
	VSShaderCompile();
	PSShaderCompile();
	//SpriteLoadTexture();
	InputLayout();
	SettingPipelineState();
	RootSignature();
	CreatePipelineState();
}

HRESULT DirectX2D::CreateSpriteInit()
{
	HRESULT result = S_FALSE;

	/*左下、左上、右下、右上*/
	enum { LB, LT, RB, RT };

	/*テクスチャ情報取得*/
	D3D12_RESOURCE_DESC resDesc =
		spriteTexBuff[sprData.texNumber]->GetDesc();
	float WIDTH = (float)resDesc.Width;
	float HEIGHT = (float)resDesc.Height;

	float left = (0.0f - anchorpoint.x) * WIDTH;
	float right = (1.0f - anchorpoint.x) * WIDTH;
	float top = (0.0f - anchorpoint.y) * HEIGHT;
	float bottom = (1.0f - anchorpoint.y) * HEIGHT;

	if (isFlipX) {
		left = -left;
		right = -right;
	}
	if (isFlipY) {
		top = -top;
		bottom = -bottom;
	}

	/*頂点データ*/
	VertexPosUv vertices[] = {
		{{  left, bottom, 0.0f},{0.0f,1.0f}}, //左下
		{{  left,    top, 0.0f},{0.0f,0.0f}}, //左上
		{{ right, bottom, 0.0f},{1.0f,1.0f}}, //右下
		{{ right,    top, 0.0f},{1.0f,0.0f}}, //右上
	};

	/*頂点バッファ生成*/
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&spriteVertBuff));
	//assert(SUCCEEDED(result));

	/*バッファへのデータ転送*/
	VertexPosUv* vertMap = nullptr;
	result = spriteVertBuff->Map(
		0,
		nullptr,
		(void**)&vertMap);
	//assert(SUCCEEDED(result));
	memcpy(vertMap, vertices, sizeof(vertices));
	spriteVertBuff->Unmap(0, nullptr);

	//リソース設定
	ComPtr<ID3D12Resource> depthBuffer;
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	//リソース生成
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer));

	/*深度ビュー用デスクリプタヒープ作成*/
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	result = DirectXImportant::dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	/*深度ステンシルビューの作成*/
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DirectXImportant::dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	/*頂点バッファビューの作成*/
	spriteVBView.BufferLocation = spriteVertBuff->GetGPUVirtualAddress();
	spriteVBView.SizeInBytes = sizeof(vertices);
	spriteVBView.StrideInBytes = sizeof(vertices[0]);

	//D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};                   //設定構造体
	//descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	//descHeapDesc.NumDescriptors = 3;                                //バッファは2つ

	//result = DirectXImportant::dev->CreateDescriptorHeap(
	//	&descHeapDesc, IID_PPV_ARGS(&spriteDescHeap));               //生成

	////デスクリプタヒープの先頭ハンドルを取得しておく
	///*D3D12_CPU_DESCRIPTOR_HANDLE */basicHeapHandle =
	//	spriteDescHeap->GetCPUDescriptorHandleForHeapStart();

	/*定数バッファの生成*/
	result = DirectXImportant::dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&spriteConstBuff));
	//assert(SUCCEEDED(result));

	/*定数バッファにデータ転送*/
	ConstBufferData* constMap = nullptr;
	result = spriteConstBuff->Map(
		0,
		nullptr,
		(void**)&constMap);
	//assert(SUCCEEDED(result));
	constMap->color = XMFLOAT4(1, 1, 1, 1);
	constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f,
		1280, //要修正
		720,
		0.0f,
		0.0f,
		1.0f); //平行投影行列の合成
	spriteConstBuff->Unmap(0, nullptr);

	return result;
}

void DirectX2D::SpriteUpdate()
{
	/*パイプラインステートの設定*/
	DirectXImportant::cmdList->SetPipelineState(spritePipelineState.Get());

	/*ルートシグネチャの設定*/
	DirectXImportant::cmdList->SetGraphicsRootSignature(spriteRootSignature.Get());

	/*プリミティブ形状を設定*/
	DirectXImportant::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void DirectX2D::SpriteDraw(bool isDraw)
{
	if (!isDraw) {
		return;
	}

	/*ワールド行列の更新*/
	sprData.spriteMatWorld = XMMatrixIdentity();
	sprData.spriteMatWorld *= XMMatrixRotationZ(XMConvertToRadians(sprData.spriteRotation));
	sprData.spriteMatWorld *= XMMatrixTranslationFromVector(sprData.spritePosition);

	/*行列の転送*/
	ConstBufferData* constMap = nullptr;
	HRESULT result = spriteConstBuff->Map(
		0,
		nullptr,
		(void**)&constMap);
	constMap->color = sprData.spriteColor;
	constMap->mat = sprData.spriteMatWorld * sprData.spriteMatProjection;
	spriteConstBuff->Unmap(0, nullptr);

	/*デスクリプタヒープの配列*/
	ID3D12DescriptorHeap* ppHeaps[] = { spriteDescHeap.Get() };
	DirectXImportant::cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	/*頂点バッファをセット*/
	DirectXImportant::cmdList->IASetVertexBuffers(0, 1, &spriteVBView);

	/*定数バッファをセット*/
	DirectXImportant::cmdList->SetGraphicsRootConstantBufferView(
		0,
		spriteConstBuff->GetGPUVirtualAddress());

	/*シェーダリソースビューをセット*/
	DirectXImportant::cmdList->SetGraphicsRootDescriptorTable(
		1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			spriteDescHeap->GetGPUDescriptorHandleForHeapStart(),
			sprData.texNumber,
			DirectXImportant::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	/*描画コマンド*/
	DirectXImportant::cmdList->DrawInstanced(4, 1, 0, 0);
}
