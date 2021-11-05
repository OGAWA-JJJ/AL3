#include "DirectXBase.h"
#include "Dx12Wrapper.h"
#include "Model3DManager.h"
#include "DirectX2D.h"
#include "Shape.h"
#include "Music.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドウタイトル
	LPCTSTR WindowTitle = L"AL3";
	//背景の色
	float WindowColor[] = { 0.2f,0.2f,0.2f,0.0f };

#ifdef _DEBUG
	Window::Debuglayer();
#endif

	HWND hwnd = Window::Init(WindowTitle);
	DirectXBase::Init(hwnd);
	Input::Init(hwnd);
	Dx12Wrapper::ImguiInit();
	Dx12Wrapper::WindowsInit(hwnd);
	Dx12Wrapper::DirectXInit();

	//GameScene gameScene;

	/*----------宣言　ここから----------*/

	Shape shape;
	shape.CreateGeometry(L"Resources/Space.png");

	/*Model3D m[2];
	for (int i = 0; i < 2; i++) {
		m[i].CreateDescriptorHeap();
		m[i].CreateGraphicsPipeline();
		m[i].Init();
	}
	m[0].CreateModel("jihannki");
	m[1].CreateModel("coin");

	m[0].position = XMFLOAT3(-25, 0, 0);
	m[1].position = XMFLOAT3(25, 0, 0);

	m[0].scale = XMFLOAT3(10, 10, 10);
	m[1].scale = XMFLOAT3(10, 10, 10);*/

	/*----------宣言　ここまで----------*/

	while (Window::flag)
	{
		Window::Msg();
		Input::Update();

		DirectXBase::BeforeDraw(WindowColor);

		/*----------DirectX毎フレーム処理　ここから----------*/

		if (Input::isKey(DIK_D)) {
			Camera::eye.x += 2.0f;
		}
		if (Input::isKey(DIK_A)) {
			Camera::eye.x += -2.0f;
		}
		if (Input::isKey(DIK_W)) {
			Camera::eye.y += 2.0f;
		}
		if (Input::isKey(DIK_S)) {
			Camera::eye.y += -2.0f;
		}
		if (Input::isKey(DIK_Q)) {
			Camera::eye.z += -2.0f;
		}
		if (Input::isKey(DIK_E)) {
			Camera::eye.z -= -2.0f;
		}

		//gameScene.Update();
		//gameScene.Draw();
		shape.DrawGeometry();

		/*for (int i = 0; i < 2; i++) {
			m[i].Update();
			m[i].Draw();
		}*/


		/*----------DirextX毎フレーム処理　ここまで----------*/
		Dx12Wrapper::Draw(false);
		DirectXBase::AfterDraw();
		if (Input::isKeyTrigger(DIK_ESCAPE)) {
			break;
		}
	}
	//ウィンドウクラスを登録解除
	UnregisterClass(Window::windowClass.lpszClassName, Window::windowClass.hInstance);
}