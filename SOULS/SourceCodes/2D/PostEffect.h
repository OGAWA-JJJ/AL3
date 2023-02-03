#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <DirectXMath.h>
#include <string>

class PostEffect
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//AlphaBlendMode
	enum AlphaBlendMode
	{
		ALPHA_BLENDMODE_NONE,	//����
		ALPHA_BLENDMODE_TRANS,	//����������
		ALPHA_BLENDMODE_ADD,	//���Z����
	};

	//���̍\����
	struct SpriteInitData
	{
		std::string m_vsEntryPoint = "VSmain";								//���_�V�F�[�_�[�̃G���g���[�|�C���g
		std::string m_psEntryPoint = "PSmain";								//�s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
		const wchar_t* m_vsShaderName =
			L"Resources/Shaders/PostEffectTestVS.hlsl";						//�ǂݍ���vs�̖��O
		const wchar_t* m_psShaderName =
			L"Resources/Shaders/PostEffectTestPS.hlsl";						//�ǂݍ���ps�̖��O
		UINT m_width = 1280;												//�X�v���C�g�̕��B
		UINT m_height = 720;												//�X�v���C�g�̍����B
		AlphaBlendMode m_alphaBlendMode = ALPHA_BLENDMODE_TRANS;			//AlphaBlendMode
		float m_size = 1.0f;												//���g���ĂȂ�
		float m_gaussianSigma = 8.0f;										//�K�E�V�A���u���[�̏d��
	};

private:
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos;			//xyz���W
		DirectX::XMFLOAT2 uv;			//uv���W
	};

	struct ConstantBuffer_b0
	{
		DirectX::XMFLOAT4 color;		//�F
		DirectX::XMMATRIX mat;			//�ϊ��s��
		DirectX::XMMATRIX viewproj;
	};

	struct ConstantBuffer_b1//���]��
	{
		float weights[8];
	};

public:
	static float clearColor[4];

private:
	//���_��
	static const int vertNum = 4;
	static const int NUM_HEIGHT = 8;
	static ComPtr<ID3D12GraphicsCommandList> cmdList;

private:
	float weight[NUM_HEIGHT];

private:
	//�O���t�B�b�N�X�p�C�v���C��
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature;

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	//�萔�o�b�t�@�i�d�ݓ]���p�j
	ComPtr<ID3D12Resource> constBuff_b1;
	//�F
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
	//���������
	SpriteInitData spriteInitData;

	UINT incrementSizeSRV = 0;
	UINT incrementSizeRTV = 0;
	UINT incrementSizeDSV = 0;
	UINT srvIndex = 0;
	UINT rtvIndex = 0;
	UINT dsvIndex = 0;

public:
	static void StaticInit(ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
		PostEffect::cmdList = cmdList;
	}

public:
	PostEffect();
	void Init(const SpriteInitData& spriteInitData);
	void Draw();
	//�V�[���`��O����
	void PreDrawScene(const float* clearColor);
	//�V�[���`��㏈��
	void PostDrawScene();

private:
	//�p�C�v���C������(���g�ėp�����߂čׂ����֐�������������������)
	void CreateGraphicsPipelineState(const SpriteInitData& spriteInitData);

	void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma);

	void CreateTexBuff();
	void CreateSRV();
	void CreateRTV();
	void CreateDSV();
};