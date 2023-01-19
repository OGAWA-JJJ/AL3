#pragma once
#include "../3D/Object.h"
#include <array>

class Particle
{
public:
	struct  ParticleData
	{
		bool isRandVec = true;						//ランダム方向に飛ばすか
		bool isRandColor = false;					//ランダム色にするか
		int life = 60;								//粒の寿命フレーム
		DirectX::XMFLOAT3 pos = { 0,50,0 };			//出現座標
		DirectX::XMFLOAT3 scale = { 1.0,1.0,1.0 };	//大きさ
		DirectX::XMFLOAT3 rotation = { 0,0,0 };		//回転(角度)
		DirectX::XMFLOAT3 power = { 1,1,1 };		//飛ばす力
		DirectX::XMFLOAT3 vec = { 1,1,1 };			//ランダム方向でなかった際の方向
		DirectX::XMFLOAT4 color = { 1,1,1,1 };		//色
	};

private:
	ParticleData m_particleData = {};
	std::shared_ptr<Object> m_object = nullptr;
	int m_life = 0;
	int m_createFrame = 0;
	int m_loopNum = 0;
	bool m_isDraw = false;

	//仮
	DirectX::XMFLOAT3 m_scale = { 0,0,0 };

public:
	void Init();
	bool Update(bool isCreate);
	void Draw(bool isShadow);

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
	void ResetLifeParticle();

public:
	std::shared_ptr<Object> GetModel() { return m_object; }
	bool GetIsDraw() { return m_isDraw; }
	DirectX::XMFLOAT3& GetPower() { return m_particleData.power; }
	DirectX::XMFLOAT3& GetVec() { return m_particleData.vec; }

public:
	void SetNowScale(DirectX::XMFLOAT3 scale) { m_object->SetScale(scale); }
	DirectX::XMFLOAT3 GetNowPosition() { return m_object->GetPosition(); }
};

class ParticleManager
{
private:
	const int C_MAX_PARTICLE = 200;
	const int C_INIT_CREATE_TIMER = 1;
	const int C_INIT_CREATE_NUM = 1;

private:
	std::vector<Particle> m_particles;
	int m_maxParticle = 0;
	int m_createTimer = 0;
	int m_maxCreateTimer = 0;
	int m_createNum = 0;
	int m_createCount = 0;
	bool m_isCreate = false;
	bool m_isCreateStop = false;
	bool m_isArrivalCreateNum = false;

public:
	void Init();
	void Update();
	void Draw(bool isShadow = true);

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
	DirectX::XMFLOAT3 GetNowPosition(const int num) { return m_particles.at(num).GetNowPosition(); }
	DirectX::XMFLOAT3& GetVec(const int num) { return m_particles.at(num).GetVec(); }

	void SetNowScale(const int num, DirectX::XMFLOAT3 scale) { return m_particles.at(num).SetNowScale(scale); }
};

