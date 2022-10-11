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
		bool isRandVec = true;					//ランダム方向に飛ばすか
		bool isRandColor = false;				//ランダム色にするか
		int value = 1;							//1フレームの出現数
		int frameRate = 1;						//何フレームで出現するか
		DirectX::XMFLOAT2 life = { 60,60 };		//粒の寿命フレーム(調整可能)
		DirectX::XMFLOAT3 pos = { 0,0,0 };		//出現座標
		DirectX::XMFLOAT3 scale = { 1,1,1 };	//大きさ
		DirectX::XMFLOAT3 rotation = { 0,0,0 };	//回転(角度)
		DirectX::XMFLOAT3 power = { 10,10,10 };	//飛ばす力
		DirectX::XMFLOAT3 vec = { 1,1,1 };		//ランダム方向でなかった際の方向
		DirectX::XMFLOAT4 color = { 1,1,1,1 };	//色
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

