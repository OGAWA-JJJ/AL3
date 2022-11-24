#include "ParticleManager.h"
#include "../Users/PipelineManager.h"
#include "../Users/ModelManager.h"
#include "../DirectX/DirectXImportant.h"

//Particle
void Particle::Init()
{
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

		//âº
		if (m_life <= 10)
		{
			DirectX::XMFLOAT4 l_col = m_object->GetColor();
			if (l_col.w > 0.0f)
			{
				l_col.w -= 0.08f;
			}
			//m_object->SetColor(l_col);
		}

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
	m_particles.resize(MAX_PARTICLE);
	for (int i = 0; i < m_particles.size(); i++)
	{
		m_particles.at(i).Init();
	}
}

void ParticleManager::Update()
{
	if (isCreateTimer >= MAX_CREATE_TIMER)
	{
		isCreateTimer = 0;
		isCreate = true;
	}

	for (int i = 0; i < m_particles.size(); i++)
	{
		bool isCreateFlag = m_particles.at(i).Update(isCreate);
		if (isCreateFlag)
		{
			isCreate = false;
		}
	}

	isCreateTimer++;
}

void ParticleManager::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	for (int i = 0; i < m_particles.size(); i++)
	{
		m_particles.at(i).Draw();
	}
	Object::PostDraw();
}

void ParticleManager::SetParticle(const int num, const Particle::ParticleData& particleData)
{
	m_particles.at(num).SetParticle(particleData);
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
	m_particles.at(num).SetPosition(scale);
}

void ParticleManager::SetRotation(const int num, const DirectX::XMFLOAT3 rotation)
{
	m_particles.at(num).SetPosition(rotation);
}

void ParticleManager::SetPower(const int num, const DirectX::XMFLOAT3 power)
{
	m_particles.at(num).SetPosition(power);
}

void ParticleManager::SetVec(const int num, const DirectX::XMFLOAT3 vec)
{
	m_particles.at(num).SetPosition(vec);
}

void ParticleManager::SetColor(const int num, const DirectX::XMFLOAT3 color)
{
	m_particles.at(num).SetPosition(color);
}

bool ParticleManager::IsMove(const int num)
{
	return m_particles.at(num).GetIsDraw();
}

void ParticleManager::EndAllParticle()
{
	m_particles.clear();
}
