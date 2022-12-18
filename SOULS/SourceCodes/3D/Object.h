#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "../DirectX/d3dx12.h"
#include <string>
#include "Model.h"
#include "Light.h"

struct ObjectInitData
{
	std::string m_vsEntryPoint = "VSmain";
	std::string m_psEntryPoint = "PSmain";
};

//�p�C�v���C���Z�b�g
struct ObjPipelineSet
{
	//���[�g�V�O�l�`��
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate;
};

class Object
{
public:
	//�萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;		//�r���[�v���W�F�N�V�����s��
		DirectX::XMMATRIX world;		//���[���h�s��
		DirectX::XMFLOAT3 cameraPos;	//�J�������W�i���[���h���W�j
		float pad;
		DirectX::XMFLOAT4 color;
	};

private:
	//�f�o�C�X
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	//�R�}���h���X�g
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	//�p�C�v���C��
	static ObjPipelineSet pipelineSet;
	//���C�g
	static std::shared_ptr<Light> light;

protected:
	//�萔�o�b�t�@b0
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;
	//�F
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	//���[�J���X�P�[��
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	//X,Y,Z�����̃��[�J����]�p
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	//���[�J�����W
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//���[�J�����[���h�ϊ��s��
	DirectX::XMMATRIX matWorld = {};
	//�e�I�u�W�F�N�g
	std::shared_ptr<Object> parent = nullptr;
	//���f��
	std::shared_ptr<Model> model = nullptr;
	//�r���{�[�h
	bool isBillboard = false;

private:
	//�e�q�\���p
	DirectX::XMMATRIX matrix = {};
	bool isAffine = false;

	//��]�s��
	DirectX::XMMATRIX matRot = {};

public:
	//�ÓI������
	static void StaticInit(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	//�O���t�B�b�N�p�C�v���C���̐���
	static ObjPipelineSet CreateGraphicsPipeline(const ObjectInitData& objectInitData);
	//3D�I�u�W�F�N�g����
	static std::shared_ptr<Object> Create(std::shared_ptr<Model> model = nullptr);
	//���C�g
	static void SetLight(std::shared_ptr<Light> light) {
		Object::light = light;
	}

public:
	Object() {};
	~Object();

	//������
	bool Init();

	//���t���[������
	void Update();

	//�`��
	void Draw(const ObjPipelineSet& pipelineSet, bool isShadow = true);

public:
	//���W�̎擾
	const DirectX::XMFLOAT3& GetPosition() { return position; }
	//��]�p�̎擾
	const DirectX::XMFLOAT3& GetRotation() { return rotation; }
	//�X�P�[���̎擾
	const DirectX::XMFLOAT3& GetScale() { return scale; }
	//�F�̎擾
	const DirectX::XMFLOAT4& GetColor() { return color; }
	//���W�̐ݒ�
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }
	//��]�p�̐ݒ�
	void SetRotation(DirectX::XMFLOAT3 rotation) { this->rotation = rotation; }
	//�X�P�[���̐ݒ�
	void SetScale(DirectX::XMFLOAT3 scale) { this->scale = scale; }
	//���f���̃Z�b�g
	void SetModel(std::shared_ptr<Model> model) { this->model = model; }
	//�r���{�[�h�t���O�̃Z�b�g
	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }
	//�F�ݒ�
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	//���[���h�s��擾
	const DirectX::XMMATRIX& GetMatWorld() { return matWorld; }
	//���f�����擾
	inline std::shared_ptr<Model> GetModel() { return model; }
	//�A�t�B���ϊ��s�����Z����
	void MultiMatrix(DirectX::XMMATRIX matrix)
	{
		isAffine = true;
		this->matrix = matrix;
	}
	const DirectX::XMMATRIX GetMatRot() { return matRot; }	//�e�q�֌W�͖���������]�s��ł�

	//�X�P�[���������[���h�s��擾
	const DirectX::XMMATRIX GetMatrix()
	{
		DirectX::XMFLOAT3 l_scale = {};
		l_scale.x = 1 / scale.x;
		l_scale.y = 1 / scale.y;
		l_scale.z = 1 / scale.z;

		DirectX::XMMATRIX l_mat = matWorld;
		l_mat.r[0].m128_f32[0] *= l_scale.x;
		l_mat.r[0].m128_f32[1] *= l_scale.x;
		l_mat.r[0].m128_f32[2] *= l_scale.x;
		l_mat.r[1].m128_f32[0] *= l_scale.y;
		l_mat.r[1].m128_f32[1] *= l_scale.y;
		l_mat.r[1].m128_f32[2] *= l_scale.y;
		l_mat.r[2].m128_f32[0] *= l_scale.z;
		l_mat.r[2].m128_f32[1] *= l_scale.z;
		l_mat.r[2].m128_f32[2] *= l_scale.z;
		return l_mat;
	}
};

