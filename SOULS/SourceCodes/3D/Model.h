#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"

class Model
{
private:
	static const std::string baseDirectory;

private:
	//�f�o�C�X
	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	//�f�X�N���v�^�̃T�C�Y
	static UINT descriptorHandleIncrementSize;

private:
	//���O
	std::string name;
	//���b�V���R���e�i
	std::vector<std::shared_ptr<Mesh>> meshes;
	//�}�e���A���R���e�i
	std::unordered_map<std::string, std::shared_ptr<Material>> materials;
	//�f�t�H���g�}�e���A��
	std::shared_ptr<Material> defaultMaterial = nullptr;
	//�ő�T�C�Y�̎擾�p(Scale�͖��l��)
	DirectX::XMFLOAT3 size = { 0,0,0 };

public:
	//�ÓI������
	static void StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device);
	//���b�V������
	static std::shared_ptr<Model> CreateFromObj(
		const std::string& modelname,
		bool smoothing = false);

public:
	Model();
	~Model();

	//������
	void Init(const std::string& modelname, bool smoothing);
	//�`��
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	//���b�V���R���e�i�̎擾
	inline const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return meshes; }
	//�ő�T�C�Y�̎擾�p(Scale�͖��l��)
	DirectX::XMFLOAT3 GetModelSize() { return size; }

private:
	//�}�e���A���Ǎ�
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//�}�e���A���o�^
	void AddMaterial(std::shared_ptr<Material> material);
	//�e�N�X�`���Ǎ�
	void LoadTextures();
};

