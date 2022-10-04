#include "DirectX/DirectXBase.h"
#include "../imgui/Dx12Wrapper.h"
#include "../imgui/ImguiControl.h"
#include "Users/GameScene.h"
#include "3D/Light.h"
#include "2D/PostEffect.h"
#include "3D/FbxLoader.h"
#include "2D/RenderTarget.h"
#include "2D/ShadowMap.h"
#include "Input/Input.h"
#include "Users/ModelManager.h"
#include "Users/SpriteManager.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ReportLiveObjects();

	//ウィンドウタイトル
	LPCTSTR WindowTitle = L"OgwJ Engine";
	//背景の色
	float WindowColor[] = {
		0.0f,
		0.0f,
		0.0f,
		0.0f };

#ifdef _DEBUG
	Window::Debuglayer();

	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		//debugController->EnableDebugLayer();
		//debugController->SetEnableGPUBasedValidation(TRUE); // NEW!
	}
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
	FbxObject3D::SetDevice(DirectXImportant::dev.Get());

	//モデルのロード
	ModelManager::Init();

	//画像のロード
	SpriteManager::Init();

	//輝度抽出用
	SpriteInitData luminanceData;
	luminanceData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	luminanceData.m_psShaderName = L"Resources/Shaders/LuminancePS.hlsl";

	luminanceData.m_vsEntryPoint = "VSmain";
	luminanceData.m_psEntryPoint = "PSmain";

	luminanceData.m_width = WINDOW_WIDTH;
	luminanceData.m_height = WINDOW_HEIGHT;

	PostEffect* luminanceSprite = nullptr;
	luminanceSprite = new PostEffect();
	luminanceSprite->Init(luminanceData);

	//重み係数
	const float gaussSigma = 8.0f;

	//横ブラー用のスプライトを初期化
	SpriteInitData xBlurSpriteInitData;
	xBlurSpriteInitData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	xBlurSpriteInitData.m_psShaderName = L"Resources/Shaders/PostEffectTestPS.hlsl";

	xBlurSpriteInitData.m_vsEntryPoint = "VSXmain";
	xBlurSpriteInitData.m_psEntryPoint = "PSBlur";

	xBlurSpriteInitData.m_gaussianSigma = gaussSigma;

	xBlurSpriteInitData.m_width = WINDOW_WIDTH;
	xBlurSpriteInitData.m_height = WINDOW_HEIGHT;

	PostEffect* xBlurSprite = nullptr;
	xBlurSprite = new PostEffect();
	xBlurSprite->Init(xBlurSpriteInitData);

	//縦ブラー用のスプライトを初期化
	SpriteInitData yBlurSpriteInitData;
	yBlurSpriteInitData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	yBlurSpriteInitData.m_psShaderName = L"Resources/Shaders/PostEffectTestPS.hlsl";

	yBlurSpriteInitData.m_vsEntryPoint = "VSYmain";
	yBlurSpriteInitData.m_psEntryPoint = "PSBlur";

	yBlurSpriteInitData.m_gaussianSigma = gaussSigma;

	yBlurSpriteInitData.m_width = WINDOW_WIDTH / 2;
	yBlurSpriteInitData.m_height = WINDOW_HEIGHT;

	PostEffect* yBlurSprite = nullptr;
	yBlurSprite = new PostEffect();
	yBlurSprite->Init(yBlurSpriteInitData);

	//縦横ブラーをかけた絵をフレームバッファに貼り付ける為のスプライトの初期化
	SpriteInitData spriteInitData;

	spriteInitData.m_width = WINDOW_WIDTH;
	spriteInitData.m_height = WINDOW_HEIGHT;

	spriteInitData.m_vsShaderName = L"Resources/Shaders/SpriteVertexShader.hlsl";
	spriteInitData.m_psShaderName = L"Resources/Shaders/SpritePixelShader.hlsl";

	spriteInitData.m_vsEntryPoint = "VSmain";
	spriteInitData.m_psEntryPoint = "PSmain";

	spriteInitData.m_alphaBlendMode = AlphaBlendMode::ALPHA_BLENDMODE_ADD;

	PostEffect* copyToFrameBufferSprite = nullptr;
	copyToFrameBufferSprite = new PostEffect();
	copyToFrameBufferSprite->Init(spriteInitData);

	//影
	ShadowMap* shadow = nullptr;
	shadow = new ShadowMap();
	shadow->Init();

	//メイン
	GameScene* Gamescene = nullptr;
	Gamescene = new GameScene();
	Gamescene->Init(shadow->GetTexbuffer());


	/*----------宣言　ここから----------*/



	/*----------宣言　ここまで----------*/

	while (Window::flag)
	{
		Window::Msg();
		Input::Update();

		Gamescene->Update();

		luminanceSprite->PreDrawScene(DirectXImportant::cmdList.Get(), luminanceData, WindowColor);
		Gamescene->LuminanceDraw();
		luminanceSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		xBlurSprite->PreDrawScene(DirectXImportant::cmdList.Get(), xBlurSpriteInitData, WindowColor);
		luminanceSprite->Draw(DirectXImportant::cmdList.Get());
		xBlurSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		yBlurSprite->PreDrawScene(DirectXImportant::cmdList.Get(), yBlurSpriteInitData, WindowColor);
		xBlurSprite->Draw(DirectXImportant::cmdList.Get());
		yBlurSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		copyToFrameBufferSprite->PreDrawScene(DirectXImportant::cmdList.Get(), spriteInitData, WindowColor);
		yBlurSprite->Draw(DirectXImportant::cmdList.Get());
		copyToFrameBufferSprite->PostDrawScene(DirectXImportant::cmdList.Get());

		shadow->PreDraw(DirectXImportant::cmdList.Get());
		Gamescene->ShadowDraw();
		shadow->PostDraw(DirectXImportant::cmdList.Get());

		DirectXBase::BeforeDraw(WindowColor);

		/*----------DirectX毎フレーム処理　ここから----------*/

		//luminanceSprite->Draw(DirectXImportant::cmdList.Get());
		//Gamescene->Draw();
		//xBlurSprite->Draw(DirectXImportant::cmdList.Get());
		//yBlurSprite->Draw(DirectXImportant::cmdList.Get());
		Gamescene->Draw();
		copyToFrameBufferSprite->Draw(DirectXImportant::cmdList.Get());
		//shadow->Draw(DirectXImportant::cmdList.Get());
		//Gamescene->Draw();

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

	return 0;
}
