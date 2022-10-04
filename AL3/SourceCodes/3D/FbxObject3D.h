#pragma once
#include "FbxModel.h"
#include "../3D/Light.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

struct FbxInitData
{
	const char* m_vsEntryPoint = "VSmain";
	const char* m_psEntryPoint = "PSmain";
};

//�p�C�v���C���Z�b�g
struct FbxPipelineSet
{
	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	ID3D12PipelineState* pipelinestate;
};

class FbxObject3D
{
protected:
	//WRL�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�{�[���̍ő吔
	static const int MAX_BONES = 32;

public:
	//�萔�o�b�t�@
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;	//�r���[�v���W�F�N�V�����s��
		XMMATRIX world;		//���[���h�s��
		XMFLOAT3 cameraPos;	//�J�������W(���[���h���W)
	};

	//�萔�o�b�t�@�p�f�[�^�\����(�X�L�j���O)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

	struct ConstBufferData1
	{
		XMFLOAT3 ambient; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		XMFLOAT3 diffuse; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		XMFLOAT3 specular; // �X�y�L�����[�W��
		float alpha;	// �A���t�@
	};

private:
	//�f�o�C�X
	static ID3D12Device* device;
	//���[�g�V�O�l�`��
	//static ComPtr<ID3D12RootSignature> rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	//static ComPtr<ID3D12PipelineState> pipelinestate;
	//�p�C�v���C��
	static FbxPipelineSet pipelineSet;
	//���C�g
	static Light* light;

public:
	//Setter
	static void SetDevice(ID3D12Device* device) { FbxObject3D::device = device; }
	//�O���t�B�b�N�X�p�C�v���C���̐���
	static FbxPipelineSet CreateGraphicsPipeline(const FbxInitData& fbxInitdata);
	//���C�g�̃Z�b�g
	static void SetLight(Light* light) { FbxObject3D::light = light; }

private:
	//�A���r�G���g�e���x
	XMFLOAT3 ambient = { 0.8f, 0.8f, 0.8f };
	//�f�B�t���[�Y�e���x
	XMFLOAT3 diffuse = { 0.8f, 0.8f, 0.8f };
	//�X�y�L�����[�e���x
	XMFLOAT3 specular = { 0.8f, 0.8f, 0.8f };
	//�A���t�@
	float alpha = 1.0f;

protected:
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffTransform;
	//�萔�o�b�t�@�i���ˌ��j
	ComPtr<ID3D12Resource> constBuffData1;
	//���[�J���X�P�[��
	XMFLOAT3 scale = { 10,10,10 };
	//X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	//���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	//���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld = DirectX::XMMatrixIdentity();
	//���f��
	FbxModel* model = nullptr;
	//�萔�o�b�t�@(�X�L��)
	ComPtr<ID3D12Resource> constBuffSkin;
	//1�t���[���̎���
	FbxTime frameTime;
	//�A�j���[�V�����J�n����
	FbxTime startTime;
	//�A�j���[�V�����I������
	FbxTime endTime;
	//���ݎ���(�A�j���[�V����)
	FbxTime currentTime;
	//�A�j���[�V�����Đ���
	bool isPlay = false;

	//SRV�p�f�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* fbxDescHeap = {};

	//�{�[���̖��O�ƍs��(Update��ɍX�V)
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> affineTrans;
	//��̃��[���h�s��
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	//�{�[���S���̉�]�s��
	std::vector<DirectX::XMMATRIX> matRots;

public:
	//������
	void Init();
	//���t���[������
	void Update(bool isShadowCamera = false);
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList, const FbxPipelineSet& pipelineSet);
	//�A�j���[�V�����J�n
	void PlayAnimation();
	//�e�N�X�`���ǉ�
	void AddTexture(ID3D12Resource* texbuff, ID3D12DescriptorHeap* srv);
	//�����`��p(AddTexture��)
	void ShadowDraw(ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* srv);

public:
	void SetModel(FbxModel* model) { this->model = model; }
	//Setter�����I
	void SetScale(const XMFLOAT3& scale) { this->scale = scale; }
	void SetRotation(const XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetPosition(const XMFLOAT3& position) { this->position = position; }
	const XMFLOAT3& GetScale() { return scale; }
	const XMFLOAT3& GetRotation() { return rotation; }
	const XMFLOAT3& GetPosition() { return position; }

	void StopAnimation() { isPlay = false; }			//�A�j���[�V�����̒�~
	void ResetAnimation() { currentTime = startTime; }	//�A�j���[�V�����̃��Z�b�g
	void ReplayAnimation() { isPlay = true; }			//�A�j���[�V�����̍ĊJ

	const FbxTime& GetStartTime() { return startTime; }	//�A�j���[�V�����̊J�n���Ԃ̎擾
	const FbxTime& GetEndTime() { return endTime; }		//�A�j���[�V�����̏I�����Ԃ̎擾
	const FbxTime& GetNowTime() { return currentTime; }	//���݂̃A�j���[�V�����̎��Ԃ̎擾
	void SetStartTime(FbxTime& startTime) { this->startTime = startTime; }
	void SetEndTime(FbxTime& endTime) { this->endTime = endTime; }
	void SetNowTime(FbxTime& currentTime) { this->currentTime = currentTime; }

	const std::vector<std::pair<std::string, DirectX::XMMATRIX>>& GetAffineTrans() { return affineTrans; }	//�{�[���̖��O�ƃ��[���h�s��
	XMMATRIX GetMatrix() { return matrix; }	//��Œ�
	const std::vector<DirectX::XMMATRIX>& GetMatRots() { return matRots; }	//�{�[���̉�]�s��
};

