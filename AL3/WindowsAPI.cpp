#include "WindowsAPI.h"

WNDCLASSEX Window::windowClass = {};
RECT Window::wrc = {};
HWND Window::hwnd = {};
MSG Window::msg = {};
bool Window::flag = true;
//Window StaticWindow::window;

Window::Window()
{
}

void Window::Debuglayer()
{
	//デバッグレイヤー
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
}

HWND Window::Init(LPCTSTR& WindowTitle)
{
	//WNDCLASSEX windowClass = {};
	//RECT wrc = {};
	//HWND hwnd = {};

	//ウィンドウクラスを設定
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProc;       // ウィンドウプロシージャを設定
	windowClass.lpszClassName = L"DirectX";              // ウィンドウクラス名
	windowClass.hInstance = GetModuleHandle(nullptr);    // ウィンドウハンドル
	//windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);   // カーソル指定
	windowClass.hCursor = LoadCursor(NULL, IDC_HAND);   // カーソル指定

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&windowClass);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);    // 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow(windowClass.lpszClassName,        // クラス名
		WindowTitle,                                        // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,                            // 標準的なウィンドウスタイル
		//WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT,                                    // 表示X座標（OSに任せる）
		CW_USEDEFAULT,                                    // 表示Y座標（OSに任せる）
		wrc.right - wrc.left,                            // ウィンドウ横幅
		wrc.bottom - wrc.top,                            // ウィンドウ縦幅
		nullptr,                                        // 親ウィンドウハンドル
		nullptr,                                        // メニューハンドル
		windowClass.hInstance,                            // 呼び出しアプリケーションハンドル
		nullptr);                                        // オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}

void Window::Msg()
{
	//MSG msg = {};

	//メッセージがある？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

		//キー入力メッセージの処理
		TranslateMessage(&msg);

		//プロシージャにメッセージを送る
		DispatchMessage(&msg);
	}

	//終了メッセージが来たらループを抜ける
	if (msg.message == WM_QUIT) {
		flag = false;
	}
}

//ウィンドウプロシージャ関数
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// メッセージで分岐
	switch (msg) {
	case WM_DESTROY: // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}