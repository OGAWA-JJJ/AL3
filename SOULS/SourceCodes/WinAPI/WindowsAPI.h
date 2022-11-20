#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

// �E�B���h�E�T�C�Y
//const int WINDOW_WIDTH = 1920 / 1.25;  //����
//const int WINDOW_HEIGHT = 1080 / 1.25;  //�c��

const int WINDOW_WIDTH = 1280;  //����
const int WINDOW_HEIGHT = 720;  //�c��

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//using namespace Microsoft::WRL;

//�E�B���h�E�v���V�[�W���֐�
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
private:
	//RECT wrc;               //�E�B���h�E�T�C�Y
	//MSG msg;                //���b�Z�[�W

	static RECT wrc;               //�E�B���h�E�T�C�Y
	static MSG msg;                //���b�Z�[�W

public:
	//WNDCLASSEX windowClass; //�E�B���h�E�N���X
	//HWND hwnd;              //�E�B���h�E�I�u�W�F�N�g
	//bool flag = true;

	static WNDCLASSEX windowClass; //�E�B���h�E�N���X
	static HWND hwnd;              //�E�B���h�E�I�u�W�F�N�g
	static bool flag;

public:
	//�R���X�g���N�^
	Window();
	//�f�o�b�O���C���[
	//void Debuglayer();
	//����������
	//HWND Init(LPCTSTR& WindowTitle);
	//���b�Z�[�W
	//void Msg();

	//�f�o�b�O���C���[
	static void Debuglayer();
	//����������
	static HWND Init(LPCTSTR& WindowTitle);
	//���b�Z�[�W
	static void Msg();
};

//class StaticWindow {
//public:
//	static Window window;;
//};