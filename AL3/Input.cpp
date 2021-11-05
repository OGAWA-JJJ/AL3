#include "Input.h"

BYTE Input::keys[256] = { 0 };
BYTE Input::oldKeys[256] = { 0 };
IDirectInputDevice8* Input::devkeyboard = nullptr;

Input::Input()
{
	devkeyboard = nullptr;
}

void Input::Init(HWND hwnd)
{
	HRESULT result;

	IDirectInput8* dinput = nullptr;
	result = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dinput,
		nullptr);

	//キーボードデバイスの生成
	result = dinput->CreateDevice(
		GUID_SysKeyboard,
		&devkeyboard,
		NULL);

	//入力データ形式のセット
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);

	//排他的制御レベルのセット
	result = devkeyboard->SetCooperativeLevel(
		hwnd,
		//画面が手前にある場合のみ入力を受け付ける
		DISCL_FOREGROUND |
		//デバイスをこのアプリだけで専有しない
		DISCL_NONEXCLUSIVE |
		//Windowsキーを無効にする
		DISCL_NOWINKEY);
}

void Input::Update()
{
	HRESULT result;

	result = devkeyboard->Acquire();

	//前フレームのキー情報を保存する。
	for (int i = 0; i < 256; i++) {
		oldKeys[i] = keys[i];
	}

	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(keys), keys);
}

bool Input::isKey(int key)
{
	return keys[key];
}

bool Input::isKeyTrigger(int key)
{
	return keys[key] && !oldKeys[key];
}

bool Input::isKeyRelease(int key)
{
	return !keys[key] && oldKeys[key];
}
