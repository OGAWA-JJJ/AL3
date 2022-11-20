#include "ParticleManager.h"
#include "../Users/PipelineManager.h"

//Particle::ParticleData Particle::m_particleData;
//std::vector<Object*> Particle::m_objects;
//std::vector<bool> Particle::m_isDraws;

void Particle::Init()
{
	m_objects.resize(MAX_PARTICLE);
	m_isDraws.resize(MAX_PARTICLE);
}

void Particle::Update()
{
	//•`‰æ‘O
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (!m_isDraws[i])
		{
			m_objects[i]->SetPosition(m_particleData.pos);
			m_objects[i]->SetScale(m_particleData.scale);

			if (!m_particleData.isRandColor)
			{
				m_objects[i]->SetColor(m_particleData.color);
			}
			else
			{
				///DirectX::XMFLOAT4 l_color = {rand()};
				//m_objects[i]->SetColor(l_color);
			}
			m_isDraws[i] = true;
		}
	}

	//•`‰æŒã
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_isDraws[i])
		{


			if (false)
			{
				m_isDraws[i] = true;
			}
		}
	}
}

void Particle::Draw()
{
	for (int i = 0; i < m_objects.size(); i++)
	{
		if (m_isDraws[i])
		{
			m_objects[i]->Draw(PipelineManager::obj_normal);
		}
	}
}

void Particle::SetParticle(const Particle::ParticleData& particleData)
{
	m_particleData = particleData;
}

void ParticleManager::Init()
{
}

void ParticleManager::Update()
{
}

void ParticleManager::SetParticle(const Particle::ParticleData& particleData)
{

}

void ParticleManager::ChangeParticleData(const int& num, const Particle::ParticleData& particleData)
{
}

void ParticleManager::EndParticle(const int& num)
{
}

void ParticleManager::EndAllParticle()
{
}
