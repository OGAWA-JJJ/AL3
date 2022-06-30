#pragma once
#include "../DirectX/DirectXImportant.h"
#include <dinput.h>

//DirectInputのバージョン指定
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Input
{
private:
	static IDirectInputDevice8* devkeyboard;
	static BYTE keys[256];
	static BYTE oldKeys[256];

public:
	Input();
	static void Init(HWND hwnd);
	static void Update();
	static bool isKey(int key);
	static bool isKeyTrigger(int key);
	static bool isKeyRelease(int key);
};

