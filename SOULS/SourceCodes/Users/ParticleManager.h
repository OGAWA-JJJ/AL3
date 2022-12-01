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
		int life = 60;								//���̎����t���[��
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
	int m_life;
	int m_createFrame;
	int m_loopNum;
	bool m_isDraw;

	//��
	DirectX::XMFLOAT3 m_scale = { 0,0,0 };

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

public:
	Object* GetModel() { return m_object; }
	bool GetIsDraw() { return m_isDraw; }
	DirectX::XMFLOAT3& GetPower() { return m_particleData.power; }
	void ResetLifeParticle();
};

class ParticleManager
{
private:
	const int C_MAX_PARTICLE = 200;
	const int C_INIT_CREATE_TIMER = 1;
	const int C_INIT_CREATE_NUM = 1;

private:
	std::vector<Particle> m_particles;
	int m_maxParticle;
	int m_createTimer;
	int m_maxCreateTimer;
	int m_createNum;
	int m_createCount;
	bool m_isCreate;
	bool m_isCreateStop;
	bool m_isArrivalCreateNum;

public:
	void Init();
	void Update();
	void Draw();

public:
	void SetParticle(const int num, const Particle::ParticleData& particleData);
	void SetMaxParticle(const int num);
	void SetMaxCreateTimer(const int num);
	void SetCreateNum(const int num);

	void SetLife(const int num, const int life);
	void SetPosition(const int num, const DirectX::XMFLOAT3 pos);
	void SetScale(const int num, const DirectX::XMFLOAT3 scale);
	void SetRotation(const int num, const DirectX::XMFLOAT3 rotation);
	void SetPower(const int num, const DirectX::XMFLOAT3 power);
	void SetVec(const int num, const DirectX::XMFLOAT3 vec);
	void SetColor(const int num, const DirectX::XMFLOAT4 color);

	void SetIsCreateStop(const bool isCreateStop);
	void MultiMatrix(const int num, DirectX::XMMATRIX matrix);
	void ResetLifeParticle();

public:
	bool IsMove(const int num) { return m_particles.at(num).GetIsDraw(); }
	bool IsArrivalCreateNum() { return m_isArrivalCreateNum; }
	int GetMaxParticle() { return m_maxParticle; }

	DirectX::XMFLOAT3& GetPower(const int num) { return m_particles.at(num).GetPower(); }
};

