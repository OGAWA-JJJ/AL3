#include "Model.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../2D/TexManager.h"
#include "../DirectX/DirectXImportant.h"

const std::string Model::baseDirectory = "Resources/";
Microsoft::WRL::ComPtr<ID3D12Device> Model::device = nullptr;
UINT Model::descriptorHandleIncrementSize = 0;

Model::Model()
{
}

Model::~Model()
{
}

void Model::StaticInit(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	// �ď������`�F�b�N
	assert(!Model::device);

	Model::device = device;

	// ���b�V���̐ÓI������
	Mesh::StaticInit(device);
}

std::shared_ptr<Model> Model::CreateFromObj(const std::string& modelname, bool smoothing)
{
	//�������m��
	std::shared_ptr<Model> instance = std::make_shared<Model>();
	instance->Init(modelname, smoothing);

	return instance;
}

void Model::Init(const std::string& modelname, bool smoothing)
{
	const std::string filename = modelname + ".obj";
	const std::string directoryPath = baseDirectory + modelname + "/";

	//�t�@�C���X�g���[��
	std::ifstream file;
	//.obj�t�@�C�����J��
	file.open(directoryPath + filename);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	name = modelname;

	//���b�V������
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	std::vector<DirectX::XMFLOAT3> positions;	//���_���W
	std::vector<DirectX::XMFLOAT3> normals;		//�@���x�N�g��
	std::vector<DirectX::XMFLOAT2> texcoords;	//�e�N�X�`��UV
	//1�s���ǂݍ���
	std::string line;
	while (getline(file, line)) {

		//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		//�}�e���A��
		if (key == "mtllib")
		{
			// �}�e���A���̃t�@�C�����ǂݍ���
			std::string filename;
			line_stream >> filename;
			// �}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}
		// �擪������g�Ȃ�O���[�v�̊J�n
		if (key == "g") {
			// �J�����g���b�V���̏�񂪑����Ă���Ȃ�
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				//���_�@���̕��ςɂ��G�b�W�̕�����
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				// �R���e�i�ɓo�^
				meshes.emplace_back(mesh);
				// ���̃��b�V������
				mesh = std::make_shared<Mesh>();
				indexCountTex = 0;
			}

			// �O���[�v���ǂݍ���
			std::string groupName;
			line_stream >> groupName;

			// ���b�V���ɖ��O���Z�b�g
			mesh->SetName(groupName);
		}
		// �擪������v�Ȃ璸�_���W
		if (key == "v") {
			// X,Y,Z���W�ǂݍ���
			DirectX::XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);

			//�T�C�Y�̔�r
			if (position.x > size.x) { size.x = position.x; }
			if (position.y > size.y) { size.y = position.y; }
			if (position.z > size.z) { size.z = position.z; }
		}
		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			// U,V�����ǂݍ���
			DirectX::XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V�������]
			texcoord.y = 1.0f - texcoord.y;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			DirectX::XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// �@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		// �擪������usemtl�Ȃ�}�e���A�������蓖�Ă�
		if (key == "usemtl")
		{
			if (mesh->GetMaterial() == nullptr) {
				// �}�e���A���̖��ǂݍ���
				std::string materialName;
				line_stream >> materialName;

				// �}�e���A�����Ō������A�}�e���A�������蓖�Ă�
				auto itr = materials.find(materialName);
				if (itr != materials.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f")
		{
			int faceIndexCount = 0;
			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			std::string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// ���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// ���_�ԍ�
				index_stream >> indexPosition;

				std::shared_ptr<Material> material = mesh->GetMaterial();
				index_stream.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
				// �}�e���A���A�e�N�X�`��������ꍇ
				if (material && material->textureFilename.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
					index_stream >> indexNormal;
					// ���_�f�[�^�̒ǉ�
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					//�G�b�W�������p�̃f�[�^��ǉ�
					if (smoothing) {
						//v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else {
					char c;
					index_stream >> c;
					// �X���b�V��2�A���̏ꍇ�A���_�ԍ��̂�
					if (c == '/') {
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
						//�G�b�W�������p�̃f�[�^��ǉ�
						if (smoothing) {
							//v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
					else {
						index_stream.seekg(-1, std::ios_base::cur); // 1�����߂�
						index_stream >> indexTexcoord;
						index_stream.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
						index_stream >> indexNormal;
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
						//�G�b�W�������p�̃f�[�^��ǉ�
						if (smoothing) {
							//v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}
				// �C���f�b�N�X�f�[�^�̒ǉ�
				if (faceIndexCount >= 3) {
					// �l�p�`�|���S����4�_�ڂȂ̂ŁA
					// �l�p�`��0,1,2,3�̓� 2,3,0�ŎO�p�`���\�z����
					mesh->AddIndex(indexCountTex - 1);
					mesh->AddIndex(indexCountTex);
					mesh->AddIndex(indexCountTex - 3);
				}
				else
				{
					mesh->AddIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	file.close();

	//���_�@���̕��ςɂ��G�b�W�̕�����
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}

	//�R���e�i�ɓo�^
	meshes.emplace_back(mesh);

	// ���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes) {
		// �}�e���A���̊��蓖�Ă��Ȃ�
		if (m->GetMaterial() == nullptr) {
			if (defaultMaterial == nullptr) {
				// �f�t�H���g�}�e���A���𐶐�
				defaultMaterial = Material::Create();
				defaultMaterial->name = "no material";
				materials.emplace(defaultMaterial->name, defaultMaterial);
			}
			// �f�t�H���g�}�e���A�����Z�b�g
			m->SetMaterial(defaultMaterial);
		}
	}

	//���b�V���̃o�b�t�@����
	for (auto& m : meshes) {
		m->CreateBuffers();
	}

	//�}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials) {
		m.second->Update();
	}

	descriptorHandleIncrementSize =
		DirectXImportant::dev->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//�e�N�X�`���̓ǂݍ���
	LoadTextures();
}

void Model::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	//�f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { TexManager::GetHeapAdressSRV() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//�S���b�V����`��
	for (auto& mesh : meshes)
	{
		mesh->Draw(cmdList);
	}
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	//�}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	std::shared_ptr<Material> material = nullptr;

	//1�s���ǂݍ��ށ�fgets�ɂ��������y��??
	std::string line;
	while (getline(file, line)) {

		//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;
		getline(line_stream, key, ' ');

		//�擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin()); // �擪�̕������폜
		}

		//�擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {

			//���Ƀ}�e���A���������
			if (material) {
				//�}�e���A�����R���e�i�ɓo�^
				AddMaterial(material);
			}

			//�V�����}�e���A���𐶐�
			material = Material::Create();
			//�}�e���A�����ǂݍ���
			line_stream >> material->name;
		}
		//�擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		//�擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}
		//�擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}
		//�擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd") {
			//�e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material->textureFilename;

			//�t���p�X����t�@�C���������o��
			size_t pos1;
			pos1 = material->textureFilename.rfind('\\');
			if (pos1 != std::string::npos) {
				material->textureFilename = material->textureFilename.substr(pos1 + 1, material->textureFilename.size() - pos1 - 1);
			}

			pos1 = material->textureFilename.rfind('/');
			if (pos1 != std::string::npos) {
				material->textureFilename = material->textureFilename.substr(pos1 + 1, material->textureFilename.size() - pos1 - 1);
			}
		}
	}
	//�t�@�C�������
	file.close();

	if (material) {
		//�}�e���A����o�^
		AddMaterial(material);
	}
}

void Model::AddMaterial(std::shared_ptr<Material> material)
{
	//�R���e�i�ɓo�^
	materials.emplace(material->name, material);
}

void Model::LoadTextures()
{
	std::string directoryPath = baseDirectory + name + "/";

	for (auto& m : materials) {
		std::weak_ptr<Material> material = m.second;

		//�e�N�X�`������
		if (material.lock()->textureFilename.size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
				CD3DX12_CPU_DESCRIPTOR_HANDLE(
					TexManager::GetCpuHeapStartSRV(),
					TexManager::GetOffsetSRV(),
					descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
				CD3DX12_GPU_DESCRIPTOR_HANDLE(
					TexManager::GetGpuHeapStartSRV(),
					TexManager::GetOffsetSRV(),
					descriptorHandleIncrementSize);
			//�}�e���A���Ƀe�N�X�`���ǂݍ���
			material.lock()->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);
		}
		//�e�N�X�`���Ȃ�
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
				CD3DX12_CPU_DESCRIPTOR_HANDLE(
					TexManager::GetCpuHeapStartSRV(),
					TexManager::GetOffsetSRV(),
					descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
				CD3DX12_GPU_DESCRIPTOR_HANDLE(
					TexManager::GetGpuHeapStartSRV(),
					TexManager::GetOffsetSRV(),
					descriptorHandleIncrementSize);
			//�}�e���A���Ƀe�N�X�`���ǂݍ���
			material.lock()->LoadTexture(baseDirectory, cpuDescHandleSRV, gpuDescHandleSRV);
		}
	}
}
