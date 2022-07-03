#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定

#include "../DirectX/DirectXImportant.h"

#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

#define XINPUT_THUMB_LEFTVERT 0
#define XINPUT_THUMB_LEFTSIDE 1
#define XINPUT_THUMB_RIGHTVERT 2
#define XINPUT_THUMB_RIGHTSIDE 3

#define XINPUT_TRIGGER_LEFT 0
#define XINPUT_TRIGGER_RIGHT 1

class Input
{
private:
	static IDirectInputDevice8* devkeyboard;
	static BYTE keys[256];
	static BYTE oldKeys[256];

	static XINPUT_STATE state;					//現在のフレームのゲームパッド情報
	static XINPUT_STATE oldstate;				//前のフレームのゲームパッド情報
	static XINPUT_VIBRATION vibration;			//振動用
	static bool isConnect;

public:
	Input();
	static void Init(HWND hwnd);
	static void Update();
	static bool isKey(int key);
	static bool isKeyTrigger(int key);
	static bool isKeyRelease(int key);

	//XINPUT_GAMEPAD_DPAD_UP          0x0001		デジタル方向ボタン上
	//XINPUT_GAMEPAD_DPAD_DOWN        0x0002		デジタル方向ボタン下
	//XINPUT_GAMEPAD_DPAD_LEFT        0x0004		デジタル方向ボタン左
	//XINPUT_GAMEPAD_DPAD_RIGHT       0x0008		デジタル方向ボタン右
	//XINPUT_GAMEPAD_START            0x0010		STARTボタン
	//XINPUT_GAMEPAD_BACK             0x0020		BACKボタン
	//XINPUT_GAMEPAD_LEFT_THUMB       0x0040		LTボタン(判定のみ)
	//XINPUT_GAMEPAD_RIGHT_THUMB      0x0080		RTボタン(判定のみ)
	//XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100		LBボタン
	//XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200		RBボタン
	//XINPUT_GAMEPAD_A                0x1000		Aボタン
	//XINPUT_GAMEPAD_B                0x2000		Bボタン
	//XINPUT_GAMEPAD_X                0x4000		Xボタン
	//XINPUT_GAMEPAD_Y                0x8000		Yボタン

	//XINPUT_THUMB_LEFTVERT		左スティックの縦方向
	//XINPUT_THUMB_LEFTSIDE		左スティックの横方向
	//XINPUT_THUMB_RIGHTVERT	右スティックの縦方向
	//XINPUT_THUMB_RIGHTSIDE	右スティックの横方向

	//XINPUT_TRIGGER_LEFT		左のトリガー
	//XINPUT_TRIGGER_RIGHT		右のトリガー

	static bool isPadConnect();
	static bool isPad(int pad);
	static bool isPadTrigger(int pad);
	static bool isPadEnd(int pad);
	static float isPadThumb(int pad);
	static float isOldPadThumb(int pad);
	static float isPadThumbTrigger(int pad);
	static float isPadTri(int pad);
	static float isPadTriTrigger(int pad);
	static float isPadTriEnd(int pad);
	static float isPadTriEndDefValue(int pad, float value);
	static void StartVibration(int LeftMotorRate, int RightMotorRate);
};

enum InputMouseButton {
	DIM_LEFT,
	DIM_RIGHT,
	DIM_CENTER
};

