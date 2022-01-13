#include "DirectX/DirectXBase.h"
#include "../imgui/Dx12Wrapper.h"
#include "Users/GameScene.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドウタイトル
	LPCTSTR WindowTitle = L"MOCHI_TUKI";
	//背景の色
	float WindowColor[] = { 0.4f,0.4f,0.4f,1.0f };

#ifdef _DEBUG
	Window::Debuglayer();
#endif

	HWND hwnd = Window::Init(WindowTitle);
	DirectXBase::Init(hwnd);
	Input::Init(hwnd);
	Dx12Wrapper::ImguiInit();
	Dx12Wrapper::WindowsInit(hwnd);
	Dx12Wrapper::DirectXInit();

	/*----------宣言　ここから----------*/

	//Shape shape;
	//shape.CreateGeometry(L"Resources/hamurabyss.png");
	GameScene Gamescene;
	Gamescene.Init();

	/*----------宣言　ここまで----------*/

	while (Window::flag)
	{
		Window::Msg();
		Input::Update();

		DirectXBase::BeforeDraw(WindowColor);


		/*----------DirectX毎フレーム処理　ここから----------*/

		Gamescene.Update();
		Gamescene.Draw();

		/*for (int i = 0; i < 2; i++) {
			const float md_pos = 5.0f;
			XMFLOAT3 pos{};
			pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

			const float md_vel = 0.05f;
			XMFLOAT3 vel{};
			vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

			const float md_acc = 0.001f;
			XMFLOAT3 acc{};
			acc.y = (float)rand() / RAND_MAX * md_acc;
			shape.Add(60, pos, vel, acc, 1.0f, 0.0f);
		}*/
		//shape.DrawGeometry();

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