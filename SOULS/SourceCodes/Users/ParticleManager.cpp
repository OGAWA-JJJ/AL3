#include "ParticleManager.h"
#include "../Users/PipelineManager.h"
#include "../Users/ModelManager.h"
#include "../DirectX/DirectXImportant.h"

//Particle
void Particle::Init()
{
	m_life = 0;
	m_createFrame = 0;
	m_loopNum = 0;
	m_isDraw = false;
	m_object = Object::Create(ModelManager::model_box2);
}

void Particle::Create()
{
	//ê∂ê¨ëO
	m_object->SetPosition(m_particleData.pos);
	m_object->SetScale(m_particleData.scale);
	m_object->SetRotation(m_particleData.rotation);

	if (!m_particleData.isRandColor)
	{
		m_object->SetColor(m_particleData.color);
	}
	else
	{
		DirectX::XMFLOAT4 l_color = {
			rand() % 101 / 100.0f,
			rand() % 101 / 100.0f,
			rand() % 101 / 100.0f,
			1.0f };
		m_object->SetColor(l_color);
	}

	if (!m_particleData.isRandVec)
	{
		m_object->SetColor(m_particleData.color);
	}
	else
	{
		DirectX::XMFLOAT3 l_vec = {
			(rand() % 201 - 100) / 100.0f,
			(rand() % 201 - 100) / 100.0f,
			(rand() % 201 - 100) / 100.0f
		};
		m_particleData.vec = l_vec;
	}

	m_life = 0;
	m_isDraw = true;
}

void Particle::Move()
{
	//ê∂ê¨å„
	if (m_life < m_particleData.life)
	{
		DirectX::XMFLOAT3 l_pos = m_object->GetPosition();
		l_pos.x += m_particleData.power.x * m_particleData.vec.x;
		l_pos.y += m_particleData.power.y * m_particleData.vec.y;
		l_pos.z += m_particleData.power.z * m_particleData.vec.z;
		m_object->SetPosition(l_pos);

		m_life++;
	}
	else
	{
		m_isDraw = false;
	}
}

bool Particle::Update(bool isCreate)
{
	bool hoge = false;
	if (!m_isDraw && isCreate)
	{
		Create();
		hoge = true;
	}
	else if (m_isDraw)
	{
		Move();
	}

	m_object->Update();
	return hoge;
}

void Particle::Draw()
{
	if (m_isDraw)
	{
		m_object->Draw(PipelineManager::obj_texColorReceice);
	}
}

void Particle::SetParticle(const Particle::ParticleData& particleData)
{
	m_particleData = particleData;
}

void Particle::SetLife(const int life)
{
	m_particleData.life = life;
}

void Particle::SetPosition(const DirectX::XMFLOAT3 pos)
{
	m_particleData.pos = pos;
}

void Particle::SetScale(const DirectX::XMFLOAT3 scale)
{
	m_particleData.scale = scale;
}

void Particle::SetRotation(const DirectX::XMFLOAT3 rotation)
{
	m_particleData.rotation = rotation;
}

void Particle::SetPower(const DirectX::XMFLOAT3 power)
{
	m_particleData.power = power;
}

void Particle::SetVec(const DirectX::XMFLOAT3 vec)
{
	m_particleData.vec = vec;
}

void Particle::SetColor(const DirectX::XMFLOAT4 color)
{
	m_particleData.color = color;
}

//ParticleManager
void ParticleManager::Init()
{
	m_maxParticle = C_MAX_PARTICLE;
	m_createTimer = 0;
	m_maxCreateTimer = C_INIT_CREATE_TIMER;
	m_createNum = C_INIT_CREATE_NUM;
	m_createCount = 0;
	m_isCreate = false;
	m_isCreateStop = false;
	m_isArrivalCreateNum = false;

	m_particles.resize(C_MAX_PARTICLE);
	for (int i = 0; i < m_particles.size(); i++)
	{
		m_particles.at(i).Init();
	}
}

void ParticleManager::Update()
{
	if (m_createTimer >= m_maxCreateTimer && !m_isCreateStop)
	{
		m_createTimer = 0;
		m_createCount = 0;
		m_isCreate = true;
		m_isArrivalCreateNum = false;
	}
	else
	{
		m_createTimer++;
	}

	for (int i = 0; i < m_maxParticle; i++)
	{
		if (m_particles.at(i).Update(m_isCreate))
		{
			if (m_createCount < m_createNum)
			{
				m_createCount++;
				if (m_createCount >= m_createNum)
				{
					m_isCreate = false;
					m_isArrivalCreateNum = true;
				}
			}
		}
	}
}

void ParticleManager::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	for (int i = 0; i < m_maxParticle; i++)
	{
		m_particles.at(i).Draw();
	}
	Object::PostDraw();
}

void ParticleManager::SetParticle(const int num, const Particle::ParticleData& particleData)
{
	m_particles.at(num).SetParticle(particleData);
}

void ParticleManager::SetMaxParticle(const int num)
{
	if (num > C_MAX_PARTICLE) { assert(0); }
	m_maxParticle = num;
}

void ParticleManager::SetMaxCreateTimer(const int num)
{
	m_createTimer = 0;
	m_maxCreateTimer = num;
}

void ParticleManager::SetCreateNum(const int num)
{
	if (num > C_MAX_PARTICLE) { assert(0); }
	m_createNum = num;
}

void ParticleManager::SetLife(const int num, const int life)
{
	m_particles.at(num).SetLife(life);
}

void ParticleManager::SetPosition(const int num, const DirectX::XMFLOAT3 pos)
{
	m_particles.at(num).SetPosition(pos);
}

void ParticleManager::SetScale(const int num, const DirectX::XMFLOAT3 scale)
{
	m_particles.at(num).SetScale(scale);
}

void ParticleManager::SetRotation(const int num, const DirectX::XMFLOAT3 rotation)
{
	m_particles.at(num).SetRotation(rotation);
}

void ParticleManager::SetPower(const int num, const DirectX::XMFLOAT3 power)
{
	m_particles.at(num).SetPower(power);
}

void ParticleManager::SetVec(const int num, const DirectX::XMFLOAT3 vec)
{
	m_particles.at(num).SetVec(vec);
}

void ParticleManager::SetColor(const int num, const DirectX::XMFLOAT4 color)
{
	m_particles.at(num).SetColor(color);
}

void ParticleManager::SetIsCreateStop(const bool isCreateStop)
{
	m_isCreateStop = isCreateStop;
	m_isArrivalCreateNum = false;
}

void ParticleManager::MultiMatrix(const int num, DirectX::XMMATRIX matrix)
{
	m_particles.at(num).GetModel()->MultiMatrix(matrix);
}

void ParticleManager::EndAllParticle()
{
	m_particles.clear();
}
