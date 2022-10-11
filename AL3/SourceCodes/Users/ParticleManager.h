#pragma once
#include "../3D/Object.h"
#include <DirectXMath.h>
#include <vector>
#include <array>

class Particle
{
public:
	const int MAX_PARTICLE = 100;

public:
	struct  ParticleData
	{
		bool isRandVec = true;					//�����_�������ɔ�΂���
		bool isRandColor = false;				//�����_���F�ɂ��邩
		int value = 1;							//1�t���[���̏o����
		int frameRate = 1;						//���t���[���ŏo�����邩
		DirectX::XMFLOAT2 life = { 60,60 };		//���̎����t���[��(�����\)
		DirectX::XMFLOAT3 pos = { 0,0,0 };		//�o�����W
		DirectX::XMFLOAT3 scale = { 1,1,1 };	//�傫��
		DirectX::XMFLOAT3 rotation = { 0,0,0 };	//��](�p�x)
		DirectX::XMFLOAT3 power = { 10,10,10 };	//��΂���
		DirectX::XMFLOAT3 vec = { 1,1,1 };		//�����_�������łȂ������ۂ̕���
		DirectX::XMFLOAT4 color = { 1,1,1,1 };	//�F
	};

private:
	ParticleData m_particleData;
	std::vector<Object*> m_objects;
	//std::array<Object*, m_maxParticle> m_objects;
	std::vector<bool> m_isDraws;
	int m_frameRate;

public:
	void Init();
	void Update();
	void Draw();

public:
	void SetParticle(const Particle::ParticleData& particleData);
};

class ParticleManager
{
private:
	std::vector<Particle> m_particles;

public:
	void Init();
	void Update();

public:
	void SetParticle(const Particle::ParticleData& particleData);
	void ChangeParticleData(const int& num, const Particle::ParticleData& particleData);
	void EndParticle(const int& num);
	void EndAllParticle();
};

