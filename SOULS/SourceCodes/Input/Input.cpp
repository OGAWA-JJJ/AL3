#include "Input.h"

IDirectInputDevice8* Input::devkeyboard = nullptr;
BYTE Input::keys[256] = { 0 };
BYTE Input::oldKeys[256] = { 0 };
XINPUT_STATE Input::state = {};
XINPUT_STATE Input::oldstate = {};
XINPUT_VIBRATION Input::vibration = {};
bool Input::isConnect = false;

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

	oldstate = state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD dwResult = XInputGetState(0, &state);
	if (state.dwPacketNumber >= 1) { isConnect = true; }
	else { isConnect = false; }

	//振動情報の取得
	XInputSetState(0, &vibration);
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	//関数が呼ばれてないときは振動が止まるようにする
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
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

bool Input::isPadConnect()
{
	return isConnect;
}

bool Input::isPad(int pad)
{
	return state.Gamepad.wButtons & pad;
}

bool Input::isPadTrigger(int pad)
{
	return (state.Gamepad.wButtons & pad) && !(oldstate.Gamepad.wButtons & pad);
}

bool Input::isPadEnd(int pad)
{
	return !(state.Gamepad.wButtons & pad) && (oldstate.Gamepad.wButtons & pad);
}

float Input::isPadThumb(int pad)
{
	switch (pad)
	{
	case XINPUT_THUMB_LEFTVERT:
		return (float)state.Gamepad.sThumbLY / (32767.0f);
	case XINPUT_THUMB_LEFTSIDE:
		return (float)state.Gamepad.sThumbLX / 32767.0f;
	case XINPUT_THUMB_RIGHTVERT:
		return (float)state.Gamepad.sThumbRY / 32767.0f;
	case XINPUT_THUMB_RIGHTSIDE:
		return (float)state.Gamepad.sThumbRX / 32767.0f;
	default: return 0;
	}
}

float Input::isOldPadThumb(int pad)
{
	switch (pad)
	{
	case XINPUT_THUMB_LEFTVERT:
		return (float)oldstate.Gamepad.sThumbLY / (32767.0f);
	case XINPUT_THUMB_LEFTSIDE:
		return (float)oldstate.Gamepad.sThumbLX / 32767.0f;
	case XINPUT_THUMB_RIGHTVERT:
		return (float)oldstate.Gamepad.sThumbRY / 32767.0f;
	case XINPUT_THUMB_RIGHTSIDE:
		return (float)oldstate.Gamepad.sThumbRX / 32767.0f;
	default: return 0;
	}
}

float Input::isPadThumbTrigger(int pad)
{
	switch (pad)
	{
	case XINPUT_THUMB_LEFTVERT:
		return ((float)state.Gamepad.sThumbLY / 32767) && !((float)oldstate.Gamepad.sThumbLY / 32767);
	case XINPUT_THUMB_LEFTSIDE:
		return ((float)state.Gamepad.sThumbLX / 32767) && !((float)oldstate.Gamepad.sThumbLX / 32767);
	case XINPUT_THUMB_RIGHTVERT:
		return ((float)state.Gamepad.sThumbRY / 32767) && !((float)oldstate.Gamepad.sThumbRY / 32767);
	case XINPUT_THUMB_RIGHTSIDE:
		return ((float)state.Gamepad.sThumbRX / 32767) && !((float)oldstate.Gamepad.sThumbRX / 32767);
	default: return 0;
	}
}

float Input::isPadTri(int pad)
{
	switch (pad)
	{
	case XINPUT_TRIGGER_LEFT:
		return (float)state.Gamepad.bLeftTrigger / 255;
	case XINPUT_TRIGGER_RIGHT:
		return (float)state.Gamepad.bRightTrigger / 255;
	default: return 0;
	}
}

float Input::isPadTriTrigger(int pad)
{
	switch (pad)
	{
	case XINPUT_TRIGGER_LEFT:
		return ((float)state.Gamepad.bLeftTrigger / 255) && !((float)oldstate.Gamepad.bLeftTrigger / 255);
	case XINPUT_TRIGGER_RIGHT:
		return ((float)state.Gamepad.bRightTrigger / 255) && !((float)oldstate.Gamepad.bRightTrigger / 255);
	default: return 0;
	}
}

float Input::isPadTriEnd(int pad)
{
	switch (pad)
	{
	case XINPUT_TRIGGER_LEFT:
		return !((float)state.Gamepad.bLeftTrigger / 255) && ((float)oldstate.Gamepad.bLeftTrigger / 255);
	case XINPUT_TRIGGER_RIGHT:
		return !((float)state.Gamepad.bRightTrigger / 255) && ((float)oldstate.Gamepad.bRightTrigger / 255);
	default: return 0;
	}
}

float Input::isPadTriEndDefValue(int pad, float value)
{
	switch (pad)
	{
	case XINPUT_TRIGGER_LEFT:
		return !((float)state.Gamepad.bLeftTrigger / 255 > value) && ((float)oldstate.Gamepad.bLeftTrigger / 255 > value);
	case XINPUT_TRIGGER_RIGHT:
		return !((float)state.Gamepad.bRightTrigger / 255 > value) && ((float)oldstate.Gamepad.bRightTrigger / 255 > value);
	default: return 0;
	}
}

void Input::StartVibration(int LeftMotorRate, int RightMotorRate)
{
	if (LeftMotorRate < 0 || LeftMotorRate > 100 || RightMotorRate < 0 || RightMotorRate > 100) {
		return;
	}

	vibration.wLeftMotorSpeed = (int)(655.35 * LeftMotorRate);
	vibration.wRightMotorSpeed = (int)(655.35 * RightMotorRate);
	//0～100の範囲で指定
}
