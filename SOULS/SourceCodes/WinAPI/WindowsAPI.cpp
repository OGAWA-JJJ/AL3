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
	//�f�o�b�O���C���[
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
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

	//�E�B���h�E�N���X��ݒ�
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProc;       // �E�B���h�E�v���V�[�W����ݒ�
	windowClass.lpszClassName = L"DirectX";              // �E�B���h�E�N���X��
	windowClass.hInstance = GetModuleHandle(nullptr);    // �E�B���h�E�n���h��
	//windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);   // �J�[�\���w��
	windowClass.hCursor = LoadCursor(NULL, IDC_HAND);   // �J�[�\���w��

	// �E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&windowClass);
	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);    // �����ŃT�C�Y�␳

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(windowClass.lpszClassName,        // �N���X��
		WindowTitle,                                        // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,                            // �W���I�ȃE�B���h�E�X�^�C��
		//WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT,                                    // �\��X���W�iOS�ɔC����j
		CW_USEDEFAULT,                                    // �\��Y���W�iOS�ɔC����j
		wrc.right - wrc.left,                            // �E�B���h�E����
		wrc.bottom - wrc.top,                            // �E�B���h�E�c��
		nullptr,                                        // �e�E�B���h�E�n���h��
		nullptr,                                        // ���j���[�n���h��
		windowClass.hInstance,                            // �Ăяo���A�v���P�[�V�����n���h��
		nullptr);                                        // �I�v�V����

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}

void Window::Msg()
{
	//MSG msg = {};

	//���b�Z�[�W������H
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

		//�L�[���̓��b�Z�[�W�̏���
		TranslateMessage(&msg);

		//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		DispatchMessage(&msg);
	}

	//�I�����b�Z�[�W�������烋�[�v�𔲂���
	if (msg.message == WM_QUIT) {
		flag = false;
	}
}

//�E�B���h�E�v���V�[�W���֐�
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// ���b�Z�[�W�ŕ���
	switch (msg) {
	case WM_DESTROY: // �E�B���h�E���j�����ꂽ
		PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	return DefWindowProc(hwnd, msg, wparam, lparam); // �W���̏������s��
}