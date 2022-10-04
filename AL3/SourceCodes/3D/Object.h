#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include "Model.h"
#include "Light.h"
#include "../Math/CollisionInfo.h"

struct ObjectInitData
{
	const char* m_vsEntryPoint = "VSmain";
	const char* m_psEntryPoint = "PSmain";
};

//�p�C�v���C���Z�b�g
struct ObjPipelineSet
{
	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootsignature;
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	ID3D12PipelineState* pipelinestate;
};

class BaseCollider;

class Object
{
public:
	//�萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;		//�r���[�v���W�F�N�V�����s��
		DirectX::XMMATRIX world;		//���[���h�s��
		DirectX::XMFLOAT3 cameraPos;	//�J�������W�i���[���h���W�j
	};

private:
	//�f�o�C�X
	static ID3D12Device* device;
	//�R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	//�p�C�v���C��
	static ObjPipelineSet pipelineSet;
	//���C�g
	static Light* light;

protected:
	//�萔�o�b�t�@
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
	DirectX::XMMATRIX matWorld;
	//�e�I�u�W�F�N�g
	Object* parent = nullptr;
	//���f��
	Model* model = nullptr;
	//�r���{�[�h
	bool isBillboard = false;

	//�N���X��
	const char* name = nullptr;
	//�R���C�_�[
	BaseCollider* collider = nullptr;

private:
	//SRV�p�f�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* modelDescHeap;

	//�e�q�\���p
	DirectX::XMMATRIX matrix;
	bool isAffine = false;

	//texbuff��ǉ����邩
	bool isAddTexture = false;

	//��]�s��
	DirectX::XMMATRIX matRot;

public:
	//�ÓI������
	static void StaticInit(ID3D12Device* device);
	//�O���t�B�b�N�p�C�v���C���̐���
	static ObjPipelineSet CreateGraphicsPipeline(const ObjectInitData& objectInitData);
	//�`��O����
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	//�`��㏈��
	static void PostDraw();
	//3D�I�u�W�F�N�g����
	static Object* Create(Model* model = nullptr);
	//���C�g
	static void SetLight(Light* light) {
		Object::light = light;
	}

public:
	Object() = default;
	virtual ~Object();

	//������
	//bool Init();
	virtual bool Init();

	//���t���[������
	//void Update();
	virtual void Update(bool isShadowCamera = false);

	//�`��
	//void Draw();
	virtual void Draw(const ObjPipelineSet& pipelineSet);

public:
	//���W�̎擾
	const DirectX::XMFLOAT3& GetPosition() { return position; }
	//��]�p�̎擾
	const DirectX::XMFLOAT3& GetRotation() { return rotation; }
	//�X�P�[���̎擾
	const DirectX::XMFLOAT3& GetScale() { return scale; }
	//���W�̐ݒ�
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }
	//��]�p�̐ݒ�
	void SetRotation(DirectX::XMFLOAT3 rotation) { this->rotation = rotation; }
	//�X�P�[���̐ݒ�
	void SetScale(DirectX::XMFLOAT3 scale) { this->scale = scale; }
	//���f���̃Z�b�g
	void SetModel(Model* model) { this->model = model; }
	//�r���{�[�h�t���O�̃Z�b�g
	void SetBillboard(bool isBillboard) { this->isBillboard = isBillboard; }
	//�F�ݒ�
	void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

	//���[���h�s��擾
	const DirectX::XMMATRIX& GetMatWorld() { return matWorld; }
	//�R���C�_�[�Z�b�g
	void SetCollider(BaseCollider* collider);
	//�Փˎ��R�[���o�b�N
	virtual void OnCollision(const CollisionInfo& info) {}
	//���f�����擾
	inline Model* GetModel() { return model; }
	//�e�N�X�`���ǉ��i�}���`�e�N�X�`���j
	void AddTexture(ID3D12Resource* texbuff, ID3D12DescriptorHeap* srv);
	//�A�t�B���ϊ��s�����Z����(Update��)
	void MultiMatrix(DirectX::XMMATRIX matrix)
	{
		isAffine = true;
		this->matrix = matrix;
	}
	const DirectX::XMMATRIX GetMatRot() { return matRot; }	//�e�q�֌W�͖���������]�s��ł�
};

