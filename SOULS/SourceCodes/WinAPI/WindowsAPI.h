#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

// ウィンドウサイズ
//const int WINDOW_WIDTH = 1920 / 1.25;  //横幅
//const int WINDOW_HEIGHT = 1080 / 1.25;  //縦幅

const int WINDOW_WIDTH = 1280;  //横幅
const int WINDOW_HEIGHT = 720;  //縦幅

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//using namespace Microsoft::WRL;

//ウィンドウプロシージャ関数
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
private:
	//RECT wrc;               //ウィンドウサイズ
	//MSG msg;                //メッセージ

	static RECT wrc;               //ウィンドウサイズ
	static MSG msg;                //メッセージ

public:
	//WNDCLASSEX windowClass; //ウィンドウクラス
	//HWND hwnd;              //ウィンドウオブジェクト
	//bool flag = true;

	static WNDCLASSEX windowClass; //ウィンドウクラス
	static HWND hwnd;              //ウィンドウオブジェクト
	static bool flag;

public:
	//コンストラクタ
	Window();
	//デバッグレイヤー
	//void Debuglayer();
	//初期化処理
	//HWND Init(LPCTSTR& WindowTitle);
	//メッセージ
	//void Msg();

	//デバッグレイヤー
	static void Debuglayer();
	//初期化処理
	static HWND Init(LPCTSTR& WindowTitle);
	//メッセージ
	static void Msg();
};

//class StaticWindow {
//public:
//	static Window window;;
//};