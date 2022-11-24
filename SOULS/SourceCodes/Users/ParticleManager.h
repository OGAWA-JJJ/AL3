#pragma once
#include "../3D/Object.h"
#include <DirectXMath.h>
#include <vector>
#include <array>

class Particle
{
public:
	struct  ParticleData
	{
		bool isRandVec = true;						//�����_�������ɔ�΂���
		bool isRandColor = false;					//�����_���F�ɂ��邩
		int life = 60;								//���̎����t���[��(�����\)
		DirectX::XMFLOAT3 pos = { 0,50,0 };			//�o�����W
		DirectX::XMFLOAT3 scale = { 1.0,1.0,1.0 };	//�傫��
		DirectX::XMFLOAT3 rotation = { 0,0,0 };		//��](�p�x)
		DirectX::XMFLOAT3 power = { 1,1,1 };		//��΂���
		DirectX::XMFLOAT3 vec = { 1,1,1 };			//�����_�������łȂ������ۂ̕���
		DirectX::XMFLOAT4 color = { 1,1,1,1 };		//�F
	};

private:
	ParticleData m_particleData;
	Object* m_object = nullptr;
	int m_life = 0;
	int m_createFrame = 0;
	bool m_isDraw = false;

public:
	void Init();
	bool Update(bool isCreate);
	void Draw();

private:
	void Create();
	void Move();

public:
	void SetParticle(const Particle::ParticleData& particleData);

	void SetLife(const int life);
	void SetPosition(const DirectX::XMFLOAT3 pos);
	void SetScale(const DirectX::XMFLOAT3 scale);
	void SetRotation(const DirectX::XMFLOAT3 rotation);
	void SetPower(const DirectX::XMFLOAT3 power);
	void SetVec(const DirectX::XMFLOAT3 vec);
	void SetColor(const DirectX::XMFLOAT4 color);

	bool GetIsDraw() { return m_isDraw; }

	Object* GetModel() { return m_object; }
};

class ParticleManager
{
public:
	const int MAX_PARTICLE = 100;
	const int MAX_CREATE_TIMER = 1;

private:
	std::vector<Particle> m_particles;
	int isCreateTimer = 0;
	bool isCreate = false;

public:
	void Init();
	void Update();
	void Draw();

public:
	void SetParticle(const int num, const Particle::ParticleData& particleData);

	void SetLife(const int num, const int life);
	void SetPosition(const int num, const DirectX::XMFLOAT3 pos);
	void SetScale(const int num, const DirectX::XMFLOAT3 scale);
	void SetRotation(const int num, const DirectX::XMFLOAT3 rotation);
	void SetPower(const int num, const DirectX::XMFLOAT3 power);
	void SetVec(const int num, const DirectX::XMFLOAT3 vec);
	void SetColor(const int num, const DirectX::XMFLOAT3 color);

	void MultiMatrix(const int num, DirectX::XMMATRIX matrix)
	{
		m_particles.at(num).GetModel()->MultiMatrix(matrix);
	}

	bool IsMove(const int num);

	void EndAllParticle();
};

