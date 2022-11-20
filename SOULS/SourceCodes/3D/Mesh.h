#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <vector>
#include <unordered_map>
#include "Material.h"

class Mesh
{
public:
	struct VertexPosNormalUv
	{
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT3 normal; // �@���x�N�g��
		DirectX::XMFLOAT2 uv;  // uv���W
	};

private:
	//�f�o�C�X
	static ID3D12Device* device;

private:
	//���O
	std::string name;
	//���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
	//�C���f�b�N�X�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	//���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	//�}�e���A��
	Material* material = nullptr;
	//���_�@���X���[�V���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;

public:
	//�ÓI������
	static void StaticInit(ID3D12Device* device);

public:
	Mesh();
	~Mesh();

	//���O���Z�b�g
	void SetName(const std::string& name);
	//���_�f�[�^�̒ǉ�
	void AddVertex(const VertexPosNormalUv& vertex);
	//���_�C���f�b�N�X�̒ǉ�
	void AddIndex(unsigned short index);
	//�}�e���A���̊��蓖��
	void SetMaterial(Material* material);
	//�o�b�t�@�̐���
	void CreateBuffers();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//�G�b�W�������f�[�^�̒ǉ�
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);
	//���������ꂽ���_�@���̌v�Z
	void CalculateSmoothedVertexNormals();

public:
	//���O���擾
	const std::string& GetName() { return name; }
	//���_�f�[�^�̐����擾
	inline size_t GetVertexCount() { return vertices.size(); }
	//�}�e���A���̎擾
	Material* GetMaterial() { return material; }
	//���_�o�b�t�@�擾
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }
	//�C���f�b�N�X�o�b�t�@�擾
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
	//���_�z��̎擾
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }
	//�C���f�b�N�X�z��̎擾
	inline const std::vector<unsigned short>& GetIndices() { return indices; }
};

