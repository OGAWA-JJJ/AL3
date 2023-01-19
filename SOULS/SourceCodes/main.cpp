#include "../imgui/Dx12Wrapper.h"
#include "../imgui/ImguiControl.h"
#include "2D/PostEffect.h"
#include "2D/ShadowMap.h"
#include "3D/Light.h"
#include "DirectX/DirectXBase.h"
#include "Input/Input.h"
#include "Users/GameScene.h"
#include "Users/ModelManager.h"
#include "Users/SpriteManager.h"
#include "2D/TexManager.h"

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//�E�B���h�E�^�C�g��
	LPCTSTR WindowTitle = L"SOULS";
	//�w�i�̐F
	float WindowColor[] = {
		0.0f,
		0.0f,
		0.0f,
		0.0f };

#ifdef _DEBUG
	Window::Debuglayer();

	//Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	//{
	//	debugController->SetEnableGPUBasedValidation(TRUE); // NEW!
	//}
#endif

	HWND hwnd = Window::Init(WindowTitle);
	DirectXBase::CreateDevice();
	TexManager::StaticInit();
	DirectXBase::Init(hwnd);
	Input::Init(hwnd);
	Dx12Wrapper::ImguiInit();
	Dx12Wrapper::WindowsInit(hwnd);
	Dx12Wrapper::DirectXInit();
	Object::StaticInit(DirectXImportant::dev.Get(), DirectXImportant::cmdList.Get());
	Light::StaticInit(DirectXImportant::dev.Get());
	if (!Sprite::StaticInitialize(DirectXImportant::dev.Get(), WINDOW_WIDTH, WINDOW_HEIGHT)) {
		assert(0);
		return 1;
	}
	FbxObjects::StaticInit(DirectXImportant::dev.Get(), DirectXImportant::cmdList.Get());
	PostEffect::StaticInit(DirectXImportant::cmdList.Get());
	ShadowMap::StaticInit(DirectXImportant::cmdList.Get());

	//���f���̃��[�h
	ModelManager::Init();

	//�摜�̃��[�h
	SpriteManager::StaticInit();

	/*----------�錾�@��������----------*/
#pragma region �錾
	//�d�݌W��
	const float gaussSigma = 8.0f;

	//�P�x���o�p
	PostEffect::SpriteInitData luminanceData;
	luminanceData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	luminanceData.m_psShaderName = L"Resources/Shaders/LuminancePS.hlsl";

	luminanceData.m_vsEntryPoint = "VSmain";
	luminanceData.m_psEntryPoint = "PSmain";

	luminanceData.m_width = WINDOW_WIDTH;
	luminanceData.m_height = WINDOW_HEIGHT;

	//���u���[�p�̃X�v���C�g��������
	PostEffect::SpriteInitData xBlurSpriteInitData;
	xBlurSpriteInitData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	xBlurSpriteInitData.m_psShaderName = L"Resources/Shaders/PostEffectTestPS.hlsl";

	xBlurSpriteInitData.m_vsEntryPoint = "VSXmain";
	xBlurSpriteInitData.m_psEntryPoint = "PSBlur";

	xBlurSpriteInitData.m_gaussianSigma = gaussSigma;

	xBlurSpriteInitData.m_width = WINDOW_WIDTH;
	xBlurSpriteInitData.m_height = WINDOW_HEIGHT;

	//�c�u���[�p�̃X�v���C�g��������
	PostEffect::SpriteInitData yBlurSpriteInitData;
	yBlurSpriteInitData.m_vsShaderName = L"Resources/Shaders/PostEffectTestVS.hlsl";
	yBlurSpriteInitData.m_psShaderName = L"Resources/Shaders/PostEffectTestPS.hlsl";

	yBlurSpriteInitData.m_vsEntryPoint = "VSYmain";
	yBlurSpriteInitData.m_psEntryPoint = "PSBlur";

	yBlurSpriteInitData.m_gaussianSigma = gaussSigma;

	yBlurSpriteInitData.m_width = WINDOW_WIDTH / 2;
	yBlurSpriteInitData.m_height = WINDOW_HEIGHT;

	//�c���u���[���������G���t���[���o�b�t�@�ɓ\��t����ׂ̃X�v���C�g�̏�����
	PostEffect::SpriteInitData spriteInitData;

	spriteInitData.m_width = WINDOW_WIDTH;
	spriteInitData.m_height = WINDOW_HEIGHT;

	spriteInitData.m_vsShaderName = L"Resources/Shaders/SpriteVertexShader.hlsl";
	spriteInitData.m_psShaderName = L"Resources/Shaders/SpritePixelShader.hlsl";

	spriteInitData.m_vsEntryPoint = "VSmain";
	spriteInitData.m_psEntryPoint = "PSmain";

	spriteInitData.m_alphaBlendMode = PostEffect::AlphaBlendMode::ALPHA_BLENDMODE_ADD;
#pragma endregion
	/*----------�錾�@�����܂�----------*/


	/*----------�������@��������----------*/
#pragma region ������
	//�u���[��
	std::unique_ptr<PostEffect> luminanceSprite = nullptr;
	luminanceSprite = std::make_unique<PostEffect>();
	luminanceSprite->Init(luminanceData);

	std::unique_ptr<PostEffect> xBlurSprite = nullptr;
	xBlurSprite = std::make_unique<PostEffect>();
	xBlurSprite->Init(xBlurSpriteInitData);

	std::unique_ptr<PostEffect> yBlurSprite = nullptr;
	yBlurSprite = std::make_unique<PostEffect>();
	yBlurSprite->Init(yBlurSpriteInitData);

	std::unique_ptr<PostEffect> copyToFrameBufferSprite = nullptr;
	copyToFrameBufferSprite = std::make_unique<PostEffect>();
	copyToFrameBufferSprite->Init(spriteInitData);

	//�e
	std::unique_ptr<ShadowMap> shadow = nullptr;
	shadow = std::make_unique<ShadowMap>();
	shadow->Init();

	//���C��
	std::unique_ptr<GameScene> Gamescene = nullptr;
	Gamescene = std::make_unique<GameScene>();
	Gamescene->Init();
#pragma endregion

	//��(GodRay)
	/*PostEffect::SpriteInitData highLumi;
	highLumi.m_vsEntryPoint = "VSmain";
	highLumi.m_psEntryPoint = "GetHighLuminance";
	std::unique_ptr<PostEffect> highLumiSprite = nullptr;
	highLumiSprite = std::make_unique<PostEffect>();
	highLumiSprite->Init(highLumi);

	PostEffect::SpriteInitData radial;
	radial.m_vsEntryPoint = "VSmain";
	radial.m_psEntryPoint = "PSRadialBlur";
	radial.m_alphaBlendMode = PostEffect::AlphaBlendMode::ALPHA_BLENDMODE_ADD;
	std::unique_ptr<PostEffect> radialSprite = nullptr;
	radialSprite = std::make_unique<PostEffect>();
	radialSprite->Init(radial);

	std::unique_ptr<PostEffect> radialSprite2 = nullptr;
	radialSprite2 = std::make_unique<PostEffect>();
	radialSprite2->Init(radial);*/

	/*----------�������@�����܂�----------*/

	//�Q�[�����[�v
	while (Window::flag)
	{
		Window::Msg();
		Input::Update();

		Gamescene->Update();

		/*highLumiSprite->PreDrawScene(WindowColor);
		Gamescene->ShaftOfLightDraw();
		highLumiSprite->PostDrawScene();

		radialSprite->PreDrawScene(WindowColor);
		highLumiSprite->Draw();
		radialSprite->PostDrawScene();

		radialSprite2->PreDrawScene(WindowColor);
		radialSprite->Draw();
		radialSprite2->PostDrawScene();*/

		luminanceSprite->PreDrawScene(WindowColor);
		Gamescene->LuminanceDraw();
		luminanceSprite->PostDrawScene();

		xBlurSprite->PreDrawScene(WindowColor);
		luminanceSprite->Draw();
		xBlurSprite->PostDrawScene();

		yBlurSprite->PreDrawScene(WindowColor);
		xBlurSprite->Draw();
		yBlurSprite->PostDrawScene();

		copyToFrameBufferSprite->PreDrawScene(WindowColor);
		yBlurSprite->Draw();
		copyToFrameBufferSprite->PostDrawScene();

		shadow->PreDraw();
		Gamescene->ShadowDraw();
		shadow->PostDraw();

		DirectXBase::BeforeDraw(WindowColor);

		/*----------DirectX���t���[�������@��������----------*/

		//luminanceSprite->Draw();
		//Gamescene->Draw();
		//xBlurSprite->Draw();
		//yBlurSprite->Draw();

		//Gamescene->ShadowDraw();

		Gamescene->Draw();
		copyToFrameBufferSprite->Draw();
		SpriteManager::SceneTransDraw();
		 
		//radialSprite2->Draw();
		//shadow->Draw();

		/*----------DirextX���t���[�������@�����܂�----------*/

		Dx12Wrapper::Draw(true);

		DirectXBase::AfterDraw();
		if (Input::isKeyTrigger(DIK_ESCAPE)) {
			break;
		}
	}

	/*ID3D12DebugDevice* debugInterface;
	if (SUCCEEDED(DirectXImportant::dev->QueryInterface(&debugInterface)))
	{
		debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
		debugInterface->Release();
	}*/

	//�E�B���h�E�N���X��o�^����
	UnregisterClass(Window::windowClass.lpszClassName, Window::windowClass.hInstance);

	return 0;
}
