#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

const int WINDOW_WIDTH = 1280;  //横幅
const int WINDOW_HEIGHT = 720;  //縦幅

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//ウィンドウプロシージャ関数
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
private:
	static RECT wrc;               //ウィンドウサイズ
	static MSG msg;                //メッセージ

public:
	static WNDCLASSEX windowClass; //ウィンドウクラス
	static HWND hwnd;              //ウィンドウオブジェクト
	static bool flag;

public:
	Window();

	//デバッグレイヤー
	static void Debuglayer();
	//初期化処理
	static HWND Init(LPCTSTR& WindowTitle);
	//メッセージ
	static void Msg();
};