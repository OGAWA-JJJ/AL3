#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>

const int WINDOW_WIDTH = 1280;  //����
const int WINDOW_HEIGHT = 720;  //�c��

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//�E�B���h�E�v���V�[�W���֐�
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
private:
	static RECT wrc;               //�E�B���h�E�T�C�Y
	static MSG msg;                //���b�Z�[�W

public:
	static WNDCLASSEX windowClass; //�E�B���h�E�N���X
	static HWND hwnd;              //�E�B���h�E�I�u�W�F�N�g
	static bool flag;

public:
	Window();

	//�f�o�b�O���C���[
	static void Debuglayer();
	//����������
	static HWND Init(LPCTSTR& WindowTitle);
	//���b�Z�[�W
	static void Msg();
};