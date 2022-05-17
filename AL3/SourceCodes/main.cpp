#include "DirectX/DirectXBase.h"
#include "../imgui/Dx12Wrapper.h"
#include "../imgui/ImguiControl.h"
#include "Users/GameScene.h"
#include "3D/Light.h"
#include "2D/PostEffect.h"
#include "3D/FbxLoader.h"
#include "2D/RenderTarget.h"

void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma);

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドウタイトル
	LPCTSTR WindowTitle = L"OgwJ Engine";
	//背景の色
	float WindowColor[] = {
		ImguiControl::Imgui_backColor_r,
		ImguiControl::Imgui_backColor_g,
		ImguiControl::Imgui_backColor_b,
		1.0f };

#ifdef _DEBUG
	Window::Debuglayer();
#endif

	HWND hwnd = Window::Init(WindowTitle);
	DirectXBase::Init(hwnd);
	Input::Init(hwnd);
	Dx12Wrapper::ImguiInit();
	Dx12Wrapper::WindowsInit(hwnd);
	Dx12Wrapper::DirectXInit();
	Object::StaticInit(DirectXImportant::dev.Get());
	Light::StaticInit(DirectXImportant::dev.Get());
	if (!Sprite::StaticInitialize(DirectXImportant::dev.Get(), WINDOW_WIDTH, WINDOW_HEIGHT)) {
		assert(0);
		return 1;
	}
	FbxLoader::GetInstance()->Init(DirectXImportant::dev.Get());

	GameScene* Gamescene = nullptr;
	Gamescene = new GameScene();
	Gamescene->Init();

	//1,GameScene描画用のレンダーターゲットを作成
	RenderTarget mainRenderTarget;
	mainRenderTarget.Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		1,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_D32_FLOAT
	);

	//2,ガウスブラー用の重みテーブルを計算する
	const int NUM_HEIGHT = 8;

	//テーブルのサイズは8
	float weights[NUM_HEIGHT];

	//重みテーブルを計算する
	CalcWeightsTableFromGaussian(
		weights,		//格納先
		NUM_HEIGHT,		//重みテーブルのサイズ
		8.0f			//ボケ具合
	);

	//3,横ブラー用のレンダリングターゲットを作成
	RenderTarget xBlurRenderTarget;
	xBlurRenderTarget.Create(
		WINDOW_WIDTH / 2.0f,
		WINDOW_HEIGHT,
		1,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_D32_FLOAT
	);

	//4,横ブラー用のスプライトを初期化
	SpriteInitData xBlurSpriteInitData;
	xBlurSpriteInitData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	xBlurSpriteInitData.m_psShaderName = L"Resources/Shaders/PostEffectTestPS.hlsl";

	xBlurSpriteInitData.m_vsEntryPoint = "VSXmain";
	xBlurSpriteInitData.m_psEntryPoint = "PSXmain";

	xBlurSpriteInitData.m_vsEntryPoint = "VSmain";
	xBlurSpriteInitData.m_psEntryPoint = "PSmain";

	xBlurSpriteInitData.m_width = WINDOW_WIDTH;
	xBlurSpriteInitData.m_height = WINDOW_HEIGHT;

	//xBlurSpriteInitData.m_textures.push_back(&mainRenderTarget.GetRenderTargetTexture());
	//xBlurSpriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();	//←今何も使ってない

	//xBlurSpriteInitData.m_expandConstantBuffer = &weights;
	//xBlurSpriteInitData.m_expandConstantBufferSize = sizeof(weights);

	//PostEffect* xBlurSprite = nullptr;
	//xBlurSprite = new PostEffect();
	//xBlurSprite->Init(xBlurSpriteInitData);

	//5,縦ブラー用のレンダリングターゲットを作成
	RenderTarget yBlurRenderTarget;
	yBlurRenderTarget.Create(
		WINDOW_WIDTH,
		WINDOW_HEIGHT / 2.0f,
		1,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_D32_FLOAT
	);

	//6,縦ブラー用のスプライトを初期化
	SpriteInitData yBlurSpriteInitData;
	yBlurSpriteInitData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	yBlurSpriteInitData.m_psShaderName = L"Resources/Shaders/PostEffectTestPS.hlsl";

	yBlurSpriteInitData.m_vsEntryPoint = "VSYmain";
	yBlurSpriteInitData.m_psEntryPoint = "PSYmain";

	yBlurSpriteInitData.m_vsEntryPoint = "VSmain";
	yBlurSpriteInitData.m_psEntryPoint = "PSmain";

	yBlurSpriteInitData.m_width = WINDOW_WIDTH;
	yBlurSpriteInitData.m_height = WINDOW_HEIGHT;

	//yBlurSpriteInitData.m_textures.push_back(&xBlurRenderTarget.GetRenderTargetTexture());
	//yBlurSpriteInitData.m_textures[0] = &xBlurRenderTarget.GetRenderTargetTexture();	//←今何も使ってない

	yBlurSpriteInitData.m_expandConstantBuffer = &weights;
	yBlurSpriteInitData.m_expandConstantBufferSize = sizeof(weights);

	PostEffect* yBlurSprite = nullptr;
	yBlurSprite = new PostEffect();
	yBlurSprite->Init(yBlurSpriteInitData);

	//7,縦横ブラーをかけた絵をフレームバッファに貼り付ける為のスプライトの初期化
	SpriteInitData spriteInitData;
	//spriteInitData.m_textures.push_back(&yBlurRenderTarget.GetRenderTargetTexture());
	//spriteInitData.m_textures[0] = &yBlurRenderTarget.GetRenderTargetTexture();

	spriteInitData.m_width = WINDOW_WIDTH;
	spriteInitData.m_height = WINDOW_HEIGHT;

	spriteInitData.m_vsShaderName = L"Resources/Shaders/SpriteVertexShader.hlsl";
	spriteInitData.m_psShaderName = L"Resources/Shaders/SpritePixelShader.hlsl";

	spriteInitData.m_vsEntryPoint = "VSmain";
	spriteInitData.m_psEntryPoint = "PSmain";

	PostEffect* copyToFrameBufferSprite = nullptr;
	copyToFrameBufferSprite = new PostEffect();
	copyToFrameBufferSprite->Init(spriteInitData);


	/*----------宣言　ここから----------*/

	/*GameScene* Gamescene = nullptr;
	Gamescene = new GameScene();
	Gamescene->Init();*/

	/*----------宣言　ここまで----------*/

	while (Window::flag)
	{
		WindowColor[0] = ImguiControl::Imgui_backColor_r;
		WindowColor[1] = ImguiControl::Imgui_backColor_g;
		WindowColor[2] = ImguiControl::Imgui_backColor_b;

		Window::Msg();
		Input::Update();

		Gamescene->Update();
		//xBlurSprite->PreDrawScene(DirectXImportant::cmdList.Get());
		//Gamescene->Draw();
		//xBlurSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		//yBlurSprite->PreDrawScene(DirectXImportant::cmdList.Get());
		//xBlurSprite->Draw(DirectXImportant::cmdList.Get());
		//yBlurSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		//copyToFrameBufferSprite->PreDrawScene(DirectXImportant::cmdList.Get());
		//yBlurSprite->Draw(DirectXImportant::cmdList.Get());
		//copyToFrameBufferSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		yBlurSprite->PreDrawScene(DirectXImportant::cmdList.Get());
		xBlurSprite->Draw(DirectXImportant::cmdList.Get());
		yBlurSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		copyToFrameBufferSprite->PreDrawScene(DirectXImportant::cmdList.Get());
		yBlurSprite->Draw(DirectXImportant::cmdList.Get());
		copyToFrameBufferSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		DirectXBase::BeforeDraw(WindowColor);

		/*----------DirectX毎フレーム処理　ここから----------*/

		//8,レンダリングターゲットをmainRenderTargetに変更する(HLSLの魔導書の方法は謎)

		//9,mainRenderTargetに各種モデルを描画する

		//10,mainRenderTargetに描画された画像に横ブラーをかける

		//11,縦ブラーも行う

		//12,mainRenderTargetの絵をフレームバッファにコピー

		// ↓ HLSLの改良...


		//Gamescene->Draw();
		//xBlurSprite->Draw(DirectXImportant::cmdList.Get());
		//yBlurSprite->Draw(DirectXImportant::cmdList.Get());
		copyToFrameBufferSprite->Draw(DirectXImportant::cmdList.Get());

		/*----------DirextX毎フレーム処理　ここまで----------*/


		Dx12Wrapper::Draw(true);
		DirectXBase::AfterDraw();
		if (Input::isKeyTrigger(DIK_ESCAPE)) {
			break;
		}
	}

	FbxLoader::GetInstance()->Finalize();
	//ウィンドウクラスを登録解除
	UnregisterClass(Window::windowClass.lpszClassName, Window::windowClass.hInstance);
}

void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma)
{
	// 重みの合計を記録する変数を定義する
	float total = 0;

	// ここからガウス関数を用いて重みを計算している
	// ループ変数のxが基準テクセルからの距離
	for (int x = 0; x < sizeOfWeightsTbl; x++)
	{
		weightsTbl[x] = expf(-0.5f * (float)(x * x) / sigma);
		total += 2.0f * weightsTbl[x];
	}

	// 重みの合計で除算することで、重みの合計を1にしている
	for (int i = 0; i < sizeOfWeightsTbl; i++)
	{
		weightsTbl[i] /= total;
	}
}
