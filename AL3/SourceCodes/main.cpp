#include "DirectX/DirectXBase.h"
#include "../imgui/Dx12Wrapper.h"
#include "Users/GameScene.h"
#include "3D/Light.h"
#include "2D/PostEffect.h"
#include "3D/FbxLoader.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドウタイトル
	LPCTSTR WindowTitle = L"OgwJ Engine";
	//背景の色
	float WindowColor[] = { 0.2f,0.2f,0.2f,1.0f };

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

	PostEffect* postEffect = nullptr;
	//Sprite::LoadTexture(100, L"Resources/white1280x720.png");
	//Spriteの継承を外して独立させる
	postEffect = new PostEffect();
	postEffect->Init();

	/*----------宣言　ここから----------*/

	GameScene* Gamescene = nullptr;
	Gamescene = new GameScene();
	Gamescene->Init();

	/*----------宣言　ここまで----------*/

	while (Window::flag)
	{
		Window::Msg();
		Input::Update();

		Gamescene->Update();
		postEffect->PreDrawScene(DirectXImportant::cmdList.Get());
		Gamescene->Draw();
		postEffect->PostDrawScene(DirectXImportant::cmdList.Get());

		DirectXBase::BeforeDraw(WindowColor);
		//Gamescene->Draw();

		/*----------DirectX毎フレーム処理　ここから----------*/

		postEffect->Draw(DirectXImportant::cmdList.Get());

		/*----------DirextX毎フレーム処理　ここまで----------*/


		Dx12Wrapper::Draw(false);
		DirectXBase::AfterDraw();
		if (Input::isKeyTrigger(DIK_ESCAPE)) {
			break;
		}
	}

	FbxLoader::GetInstance()->Finalize();
	//ウィンドウクラスを登録解除
	UnregisterClass(Window::windowClass.lpszClassName, Window::windowClass.hInstance);
}