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

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ReportLiveObjects();

	//ウィンドウタイトル
	LPCTSTR WindowTitle = L"OgwJ Engine";
	//背景の色
	float WindowColor[] = {
		0.6f,
		0.6f,
		0.6f,
		0.0f };

#ifdef _DEBUG
	Window::Debuglayer();

	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE); // NEW!
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

	//メイン
	GameScene* Gamescene = nullptr;
	Gamescene = new GameScene();
	Gamescene->Init();


	/*----------宣言　ここから----------*/



	/*----------宣言　ここまで----------*/

	while (Window::flag)
	{
		Window::Msg();
		Input::Update();

		Gamescene->Update();

		DirectXBase::BeforeDraw(WindowColor);

		/*----------DirectX毎フレーム処理　ここから----------*/

		//luminanceSprite->Draw(DirectXImportant::cmdList.Get());
		//Gamescene->Draw();
		//xBlurSprite->Draw(DirectXImportant::cmdList.Get());
		//yBlurSprite->Draw(DirectXImportant::cmdList.Get());
		Gamescene->Draw();
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
