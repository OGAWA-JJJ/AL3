#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��

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

	static XINPUT_STATE state;					//���݂̃t���[���̃Q�[���p�b�h���
	static XINPUT_STATE oldstate;				//�O�̃t���[���̃Q�[���p�b�h���
	static XINPUT_VIBRATION vibration;			//�U���p
	static bool isConnect;

public:
	Input();
	static void Init(HWND hwnd);
	static void Update();
	static bool isKey(int key);
	static bool isKeyTrigger(int key);
	static bool isKeyRelease(int key);

	//XINPUT_GAMEPAD_DPAD_UP          0x0001		�f�W�^�������{�^����
	//XINPUT_GAMEPAD_DPAD_DOWN        0x0002		�f�W�^�������{�^����
	//XINPUT_GAMEPAD_DPAD_LEFT        0x0004		�f�W�^�������{�^����
	//XINPUT_GAMEPAD_DPAD_RIGHT       0x0008		�f�W�^�������{�^���E
	//XINPUT_GAMEPAD_START            0x0010		START�{�^��
	//XINPUT_GAMEPAD_BACK             0x0020		BACK�{�^��
	//XINPUT_GAMEPAD_LEFT_THUMB       0x0040		LT�{�^��(����̂�)
	//XINPUT_GAMEPAD_RIGHT_THUMB      0x0080		RT�{�^��(����̂�)
	//XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100		LB�{�^��
	//XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200		RB�{�^��
	//XINPUT_GAMEPAD_A                0x1000		A�{�^��
	//XINPUT_GAMEPAD_B                0x2000		B�{�^��
	//XINPUT_GAMEPAD_X                0x4000		X�{�^��
	//XINPUT_GAMEPAD_Y                0x8000		Y�{�^��

	//XINPUT_THUMB_LEFTVERT		���X�e�B�b�N�̏c����
	//XINPUT_THUMB_LEFTSIDE		���X�e�B�b�N�̉�����
	//XINPUT_THUMB_RIGHTVERT	�E�X�e�B�b�N�̏c����
	//XINPUT_THUMB_RIGHTSIDE	�E�X�e�B�b�N�̉�����

	//XINPUT_TRIGGER_LEFT		���̃g���K�[
	//XINPUT_TRIGGER_RIGHT		�E�̃g���K�[

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

