#define NOMINMAX

#include "Enemy.h"
#include "ModelManager.h"
#include "Config.h"

#include "../DirectX/DirectXImportant.h"
#include "../Users/PipelineManager.h"
#include "../Math/OgaJHelper.h"
#include "../../imgui/ImguiControl.h"
#include "../DirectX/Camera.h"

#include <random>
#include <fstream>
#include <iostream>

rapidjson::Document m_eDoc(rapidjson::kObjectType);

Enemy::Enemy()
{
	pManagerHit.Init();
	pManagerEx.Init();
	pManagerRazer.Init();

	std::ifstream ifs("Resources/Config/EnemyDataConfig.json");
	std::string str;
	std::string txt;
	while (getline(ifs, str))
	{
		txt += str;
	}
	m_eDoc.Parse(txt.c_str());
	InportJson();

#pragma region ModelCreate

	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i] = Object::Create(ModelManager::model_box);
		//obj_Box[i]->SetScale(DirectX::XMFLOAT3(boxSize, boxSize, boxSize));
	}

	fbxobj_creature = FbxObjects::Create(ModelManager::fbxmodel_creatures);

	fbxobj_creature->PlayAnimation(AnimationType::STAND);
	fbxobj_creature->PlayAnimation(AnimationType::RUN);

	fbxobj_creature->PlayAnimation(AnimationType::KICK);
	fbxobj_creature->SetLoopAnimation(AnimationType::KICK, false);

	fbxobj_creature->PlayAnimation(AnimationType::PUNCH);
	fbxobj_creature->SetLoopAnimation(AnimationType::PUNCH, false);

	fbxobj_creature->PlayAnimation(AnimationType::DIE);
	fbxobj_creature->SetLoopAnimation(AnimationType::DIE, false);

	fbxobj_creature->PlayAnimation(AnimationType::R_TURN);
	fbxobj_creature->SetLoopAnimation(AnimationType::R_TURN, false);

	fbxobj_creature->PlayAnimation(AnimationType::L_TURN);
	fbxobj_creature->SetLoopAnimation(AnimationType::L_TURN, false);

	fbxobj_creature->PlayAnimation(AnimationType::R_BACK);
	fbxobj_creature->SetLoopAnimation(AnimationType::R_BACK, false);

	fbxobj_creature->PlayAnimation(AnimationType::L_BACK);
	fbxobj_creature->SetLoopAnimation(AnimationType::L_BACK, false);

	fbxobj_creature->PlayAnimation(AnimationType::EXPLOSION);
	fbxobj_creature->SetLoopAnimation(AnimationType::EXPLOSION, false);

	fbxobj_creature->PlayAnimation(AnimationType::RISE);
	fbxobj_creature->SetLoopAnimation(AnimationType::RISE, false);

	fbxobj_creature->PlayAnimation(AnimationType::SWING_DOWN);
	fbxobj_creature->SetLoopAnimation(AnimationType::SWING_DOWN, false);

	fbxobj_creature->PlayAnimation(AnimationType::TACKLE);

	fbxobj_creature->PlayAnimation(AnimationType::AWAKE);
	fbxobj_creature->SetLoopAnimation(AnimationType::AWAKE, false);

	fbxobj_creature->PlayAnimation(AnimationType::SWIP);
	fbxobj_creature->SetLoopAnimation(AnimationType::SWIP, false);

#pragma endregion

#pragma region ModelInit

	const float Creature_Scale = 0.6f;
	fbxobj_creature->SetScale(DirectX::XMFLOAT3(Creature_Scale, Creature_Scale, Creature_Scale));

#pragma endregion
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
#pragma region Init

	m_pos = { 0,0,-300.0f };
	m_swingDownStartPos = { 0,0,0 };
	m_swingDownEndPos = { 0,0,0 };
	m_playerPos = { 0,0,0 };
	m_animationTimer = 0;
	m_animationType = STAND;
	m_oldAnimationType = m_animationType;
	m_hitOBBNum = 0;
	m_deg = 180.0f;
	m_easeTimer = 0.0f;
	m_turnStartAngle = 0.0f;
	m_turnEndAngle = 0.0f;
	m_dist = 0.0f;
	m_blendTimer = 0.0f;
	m_riseStartY = 0.0f;
	m_beforeBattleEaseTimer = 0.0f;
	m_isInvincible = false;
	m_isAttack = false;
	m_isAttackTrigger = false;
	m_isCalc = false;
	m_isCalcEnd = false;
	m_isTurn = false;
	m_turnVec = false;
	m_isBackAttackLottery = false;
	m_isBackAttack = false;
	m_isRise = false;
	m_isSwing = false;
	m_isChange = false;
	m_beforeBattleEaseEndTrigger = false;

#pragma endregion

#pragma region StatusInit

	m_hp = C_MAX_HP;

#pragma endregion

	//パーティクル(Hit)
	const float pScaleHit = 1.0f;
	const float pPowerHit = 3.0f;

	Particle::ParticleData pDataHit;
	pDataHit.isRandVec = true;
	pDataHit.isRandColor = false;
	pDataHit.life = 30;
	pDataHit.scale = { pScaleHit, pScaleHit, pScaleHit };
	pDataHit.power = { pPowerHit, pPowerHit, pPowerHit };
	pDataHit.color = { 0.4f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < pManagerHit.GetMaxParticle(); i++)
	{
		pManagerHit.SetParticle(i, pDataHit);
	}
	pManagerHit.SetCreateNum(pManagerHit.GetMaxParticle());
	pManagerHit.SetIsCreateStop(true);

	//パーティクル(爆発)
	const float pScaleEx = 2.0f;
	const float pPowerEx = 1.0f;
	Particle::ParticleData pDataEx;
	pDataEx.isRandVec = false;
	pDataEx.life = 30;
	pDataEx.scale = { pScaleEx, pScaleEx, pScaleEx };
	pDataEx.power = { pPowerEx, pPowerEx, pPowerEx };
	pDataEx.color = { 0.9f, 0.9f, 0.2f, 1.0f };
	for (int i = 0; i < pManagerEx.GetMaxParticle(); i++)
	{
		pManagerEx.SetParticle(i, pDataEx);
	}
	pManagerEx.SetIsCreateStop(true);

	//パーティクル(レーザー)
	const float pScaleRazer = 2.0f;
	const float pPowerRazer = 0.5f;
	const int pColorRange = 31;

	Particle::ParticleData pDataRazer;
	pDataRazer.isRandVec = false;
	pDataRazer.life = 60;
	pDataRazer.scale = { pScaleRazer, pScaleRazer, pScaleRazer };
	pDataRazer.power = { pPowerRazer, pPowerRazer, pPowerRazer };
	for (int i = 0; i < pManagerRazer.GetMaxParticle(); i++)
	{
		pDataRazer.color = {
			0.0f,
			(rand() % pColorRange * 0.01f) + 0.3f,
			0.9f,
			0.6f };
		pManagerRazer.SetParticle(i, pDataRazer);
	}
	pManagerRazer.SetCreateNum(3);
	pManagerRazer.SetIsCreateStop(true);

	//Box(enemyScale=0.4fがベース)
	std::array<float, 10> l_x = {
		15,
		6,
		5,
		6.6f,
		6,
		6,
		5,
		6,
		5,
		6
	};
	std::array<float, 10> l_y = {
		10,
		6,
		7,
		15,
		8,
		10,
		7,
		7,
		7,
		7
	};
	std::array<float, 10> l_z = {
		10,
		6,
		5,
		6.6f,
		6,
		6,
		5,
		6,
		5,
		6
	};
	float addScale = fbxobj_creature->GetScale().x / 0.4f;
	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(
			l_x[i] * addScale,
			l_y[i] * addScale,
			l_z[i] * addScale
		));
		ImguiControl::Imgui_enemyOBBScale[i][0] = l_x[i] * addScale;
		ImguiControl::Imgui_enemyOBBScale[i][1] = l_y[i] * addScale;
		ImguiControl::Imgui_enemyOBBScale[i][2] = l_z[i] * addScale;
	}

	fbxobj_creature->ResetAnimation(AWAKE);
}

void Enemy::Update(DirectX::XMFLOAT3 playerPos)
{
	m_playerPos = playerPos;

	if (!IsDead())
	{
		if (m_animationType != STAND &&
			m_animationType != AWAKE)
		{
			//非攻撃中かつ振り向き中じゃない
			if (!m_isAttack)
			{
				DirectX::XMFLOAT3 l_pos = { m_pos.x,0,m_pos.z };
				float l_dist = OgaJHelper::CalcDist(l_pos, playerPos);
				if (!m_isTurn)
				{
					//完全追従←ゆっくり振り向け(定数足す)
					DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(playerPos, m_pos);
					eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
					m_deg = atan2(eDirection.x, eDirection.z);
					OgaJHelper::ConvertToDegree(m_deg);

					if (l_dist >= C_MAX_DIST)
					{
						m_pos.x += -eDirection.x * C_MAX_MOVE_SPEED;
						m_pos.z += -eDirection.z * C_MAX_MOVE_SPEED;
					}
				}

				JudgAnimationType(l_dist);
			}

			CalcAngleDiff();
		}
	}
	else
	{
		//生成終了
		pManagerEx.SetIsCreateStop(true);

		m_animationType = DIE;
		if (fbxobj_creature->IsAnimationEnd(DIE))
		{
			m_isDeadAnimationEnd = true;	//Initで初期化して
		}
	}

	CalcBlendAnimation();
	OtherUpdate();
	CalcOBB();
	SetImgui();

	//パーティクル
	if (pManagerHit.IsArrivalCreateNum())
	{
		pManagerHit.SetIsCreateStop(true);
	}
	pManagerHit.Update();
	pManagerRazer.Update();

	fbxobj_creature->SetAnimationIndex(m_animationType);

	CalcArea();
}

void Enemy::Draw()
{
	if (ImguiControl::Imgui_isEnemyDraw)
	{
		fbxobj_creature->Draw(PipelineManager::fbx_receiveShadow);
	}

	if (ImguiControl::Imgui_isOBBDraw)
	{
		for (int i = 0; i < obj_Box.size(); i++)
		{
			obj_Box[i]->Draw(PipelineManager::obj_receiveShadow);
		}
	}

	pManagerHit.Draw();
	pManagerEx.Draw();
	pManagerRazer.Draw();
}

void Enemy::LuminanceDraw()
{
	pManagerEx.Draw();
	pManagerRazer.Draw();
}

void Enemy::ShadowDraw()
{
	if (ImguiControl::Imgui_isEnemyDraw)
	{
		fbxobj_creature->Draw(PipelineManager::fbx_shadow, false);
	}
	pManagerEx.Draw(false);
	pManagerRazer.Draw(false);
}

bool Enemy::BeforeBattleScene()
{
	if (m_beforeBattleEaseTimer < 1.0f &&
		!m_beforeBattleEaseEndTrigger)
	{
		m_beforeBattleEaseTimer += c_beforeBattleAddTimer;
		if (m_beforeBattleEaseTimer > 1.0f)
		{
			m_beforeBattleEaseTimer = 1.0f;
		}

		const DirectX::XMFLOAT3 l_startEye = { -20.0f,20.0f,50.0f };
		const DirectX::XMFLOAT3 l_endEye = { 0.0f,50.0f,-450.0f };
		DirectX::XMFLOAT3 l_eye =
			OgaJEase::easeInOutCubicXMFLOAT3(l_startEye, l_endEye, m_beforeBattleEaseTimer);

		Camera::SetEye(l_eye);
	}
	else
	{
		m_beforeBattleEaseTimer += c_beforeBattleAddTimer * 2.0f;
		if (m_beforeBattleEaseTimer > 1.0f)
		{
			m_beforeBattleEaseTimer = 1.0f;
		}

		const DirectX::XMFLOAT3 l_startEye = { -30.0f,70.0f,-220.0f };
		const DirectX::XMFLOAT3 l_endEye = { 30.0f,70.0f,-220.0f };
		DirectX::XMFLOAT3 l_eye =
			OgaJEase::easeInSineXMFLOAT3(l_startEye, l_endEye, m_beforeBattleEaseTimer);
		Camera::SetTarget(m_obbs[1].pos);
		Camera::SetEye(l_eye);

		if (!m_beforeBattleEaseEndTrigger)
		{
			m_beforeBattleEaseTimer = 0.0f;
			m_beforeBattleEaseEndTrigger = true;
			m_animationType = AWAKE;
		}
		if (fbxobj_creature->IsCurrentAnimationEnd())
		{
			return true;
		}
	}

	return false;
}

void Enemy::AddMenbers()
{
	//Document
	rapidjson::Document l_doc(rapidjson::kObjectType);

#pragma region FRAME
	rapidjson::Value l_frameValue(rapidjson::kObjectType);
	l_frameValue.AddMember("KICK_COLLISION_TIMER         ", ImguiControl::C_KICK_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("KICK_COLLISION_ENDTIMER      ", ImguiControl::C_KICK_COLLISION_ENDTIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("PUNCH_COLLISION_TIMER        ", ImguiControl::C_PUNCH_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("PUNCH_COLLISION_ENDTIMER     ", ImguiControl::C_PUNCH_COLLISION_ENDTIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("BACK_COLLISION_TIMER         ", ImguiControl::C_BACK_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("BACK_COLLISION_ENDTIMER      ", ImguiControl::C_BACK_COLLISION_ENDTIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("EXPLOSION_COLLISION_TIMER    ", ImguiControl::C_EXPLOSION_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("EXPLOSION_COLLISION_DELAY    ", ImguiControl::C_EXPLOSION_COLLISION_DELAY, m_eDoc.GetAllocator());
	l_frameValue.AddMember("EXPLOSION_COLLISION_ENDTIMER ", ImguiControl::C_EXPLOSION_COLLISION_ENDTIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("SWIP_COLLISION_TIMER         ", ImguiControl::C_SWIP_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("SWIP_COLLISION_ENDTIMER      ", ImguiControl::C_SWIP_COLLISION_ENDTIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("RAZER_COLLISION_STARTTIMER   ", ImguiControl::C_RAZER_COLLISION_STARTTIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("RAZER_COLLISION_TIMER        ", ImguiControl::C_RAZER_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("RAZER_COLLISION_DELAY        ", ImguiControl::C_RAZER_COLLISION_DELAY, m_eDoc.GetAllocator());
	l_frameValue.AddMember("SWINGDOWN_COLLISION_TIMER    ", ImguiControl::C_SWINGDOWN_COLLISION_TIMER, m_eDoc.GetAllocator());
	l_frameValue.AddMember("SWINGDOWN_COLLISION_ENDTIMER ", ImguiControl::C_SWINGDOWN_COLLISION_ENDTIMER, m_eDoc.GetAllocator());
	l_doc.AddMember("FRAME", l_frameValue, m_eDoc.GetAllocator());
#pragma endregion

#pragma region OTHER
	rapidjson::Value l_frameOther(rapidjson::kObjectType);
	l_frameOther.AddMember("MAX_DIST        ", ImguiControl::C_MAX_DIST, m_eDoc.GetAllocator());
	l_frameOther.AddMember("MAX_TURN_RAD    ", ImguiControl::C_MAX_TURN_RAD, m_eDoc.GetAllocator());
	l_frameOther.AddMember("MAX_BACK_RAD    ", ImguiControl::C_MAX_BACK_RAD, m_eDoc.GetAllocator());
	l_frameOther.AddMember("MAX_BLEND_TIMER ", ImguiControl::C_MAX_BLEND_TIMER, m_eDoc.GetAllocator());
	l_doc.AddMember("OTHER", l_frameOther, m_eDoc.GetAllocator());
#pragma endregion

#pragma region STATUS
	rapidjson::Value l_frameStatus(rapidjson::kObjectType);
	l_frameStatus.AddMember("MAX_POWER            ", ImguiControl::C_MAX_POWER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_EXPLOSION_POWER  ", ImguiControl::C_MAX_EXPLOSION_POWER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_HP               ", ImguiControl::C_MAX_HP, m_eDoc.GetAllocator());

	l_frameStatus.AddMember("RISE_TIMER           ", ImguiControl::C_RISE_TIMER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("SWING_DOWN_TIMER     ", ImguiControl::C_SWING_DOWN_TIMER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_RISE_HEIGHT      ", ImguiControl::C_MAX_RISE_HEIGHT, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_MOVE_SPEED       ", ImguiControl::C_MAX_MOVE_SPEED, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_TURN_TIMER       ", ImguiControl::C_MAX_TURN_TIMER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_RISE_TIMER       ", ImguiControl::C_MAX_RISE_TIMER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_SWING_DOWN_TIMER ", ImguiControl::C_MAX_SWING_DOWN_TIMER, m_eDoc.GetAllocator());

	l_frameStatus.AddMember("MAX_TACKLE_TIMER     ", ImguiControl::C_MAX_TACKLE_TIMER, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_TACKLE_COUNT     ", ImguiControl::C_MAX_TACKLE_COUNT, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_TACKLE_SPEED     ", ImguiControl::C_MAX_TACKLE_SPEED, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("MAX_TACKLE_RANGE     ", ImguiControl::C_MAX_TACKLE_RANGE, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("CALC_TACKLE_RANGE    ", ImguiControl::C_CALC_TACKLE_RANGE, m_eDoc.GetAllocator());
	l_frameStatus.AddMember("BEFOREBATTLEADDTIMER ", ImguiControl::c_beforeBattleAddTimer, m_eDoc.GetAllocator());
	l_doc.AddMember("STATUS", l_frameStatus, m_eDoc.GetAllocator());
#pragma endregion

	Config::Writter("EnemyDataConfig", m_eDoc);
}

void Enemy::ExportJson()
{
#pragma region FRAME
	m_eDoc["FRAME"]["KICK_COLLISION_TIMER         "].SetInt(ImguiControl::C_KICK_COLLISION_TIMER);
	m_eDoc["FRAME"]["KICK_COLLISION_ENDTIMER      "].SetInt(ImguiControl::C_KICK_COLLISION_ENDTIMER);
	m_eDoc["FRAME"]["PUNCH_COLLISION_TIMER        "].SetInt(ImguiControl::C_PUNCH_COLLISION_TIMER);
	m_eDoc["FRAME"]["PUNCH_COLLISION_ENDTIMER     "].SetInt(ImguiControl::C_PUNCH_COLLISION_ENDTIMER);
	m_eDoc["FRAME"]["BACK_COLLISION_TIMER         "].SetInt(ImguiControl::C_BACK_COLLISION_TIMER);
	m_eDoc["FRAME"]["BACK_COLLISION_ENDTIMER      "].SetInt(ImguiControl::C_BACK_COLLISION_ENDTIMER);
	m_eDoc["FRAME"]["EXPLOSION_COLLISION_TIMER    "].SetInt(ImguiControl::C_EXPLOSION_COLLISION_TIMER);
	m_eDoc["FRAME"]["EXPLOSION_COLLISION_DELAY    "].SetInt(ImguiControl::C_EXPLOSION_COLLISION_DELAY);
	m_eDoc["FRAME"]["EXPLOSION_COLLISION_ENDTIMER "].SetInt(ImguiControl::C_EXPLOSION_COLLISION_ENDTIMER);
	m_eDoc["FRAME"]["SWIP_COLLISION_TIMER         "].SetInt(ImguiControl::C_SWIP_COLLISION_TIMER);
	m_eDoc["FRAME"]["SWIP_COLLISION_ENDTIMER      "].SetInt(ImguiControl::C_SWIP_COLLISION_ENDTIMER);
	m_eDoc["FRAME"]["RAZER_COLLISION_STARTTIMER   "].SetInt(ImguiControl::C_RAZER_COLLISION_STARTTIMER);
	m_eDoc["FRAME"]["RAZER_COLLISION_TIMER        "].SetInt(ImguiControl::C_RAZER_COLLISION_TIMER);
	m_eDoc["FRAME"]["RAZER_COLLISION_DELAY        "].SetInt(ImguiControl::C_RAZER_COLLISION_DELAY);
	m_eDoc["FRAME"]["SWINGDOWN_COLLISION_TIMER    "].SetInt(ImguiControl::C_SWINGDOWN_COLLISION_TIMER);
	m_eDoc["FRAME"]["SWINGDOWN_COLLISION_ENDTIMER "].SetInt(ImguiControl::C_SWINGDOWN_COLLISION_ENDTIMER);
#pragma endregion

#pragma region OTHER
	m_eDoc["OTHER"]["MAX_DIST        "].SetFloat(ImguiControl::C_MAX_DIST);
	m_eDoc["OTHER"]["MAX_TURN_RAD    "].SetFloat(ImguiControl::C_MAX_TURN_RAD);
	m_eDoc["OTHER"]["MAX_BACK_RAD    "].SetFloat(ImguiControl::C_MAX_BACK_RAD);
	m_eDoc["OTHER"]["MAX_BLEND_TIMER "].SetFloat(ImguiControl::C_MAX_BLEND_TIMER);
#pragma endregion

#pragma region STATUS
	m_eDoc["STATUS"]["MAX_POWER            "].SetInt(ImguiControl::C_MAX_POWER);
	m_eDoc["STATUS"]["MAX_EXPLOSION_POWER  "].SetInt(ImguiControl::C_MAX_EXPLOSION_POWER);
	m_eDoc["STATUS"]["MAX_HP               "].SetInt(ImguiControl::C_MAX_HP);

	m_eDoc["STATUS"]["RISE_TIMER           "].SetInt(ImguiControl::C_RISE_TIMER);
	m_eDoc["STATUS"]["SWING_DOWN_TIMER     "].SetInt(ImguiControl::C_SWING_DOWN_TIMER);
	m_eDoc["STATUS"]["MAX_RISE_HEIGHT      "].SetFloat(ImguiControl::C_MAX_RISE_HEIGHT);
	m_eDoc["STATUS"]["MAX_MOVE_SPEED       "].SetFloat(ImguiControl::C_MAX_MOVE_SPEED);
	m_eDoc["STATUS"]["MAX_TURN_TIMER       "].SetFloat(ImguiControl::C_MAX_TURN_TIMER);
	m_eDoc["STATUS"]["MAX_RISE_TIMER       "].SetFloat(ImguiControl::C_MAX_RISE_TIMER);
	m_eDoc["STATUS"]["MAX_SWING_DOWN_TIMER "].SetFloat(ImguiControl::C_MAX_SWING_DOWN_TIMER);

	m_eDoc["STATUS"]["MAX_TACKLE_TIMER     "].SetInt(ImguiControl::C_MAX_TACKLE_TIMER);
	m_eDoc["STATUS"]["MAX_TACKLE_COUNT     "].SetInt(ImguiControl::C_MAX_TACKLE_COUNT);
	m_eDoc["STATUS"]["MAX_TACKLE_SPEED     "].SetFloat(ImguiControl::C_MAX_TACKLE_SPEED);
	m_eDoc["STATUS"]["MAX_TACKLE_RANGE     "].SetFloat(ImguiControl::C_MAX_TACKLE_RANGE);
	m_eDoc["STATUS"]["CALC_TACKLE_RANGE    "].SetFloat(ImguiControl::C_CALC_TACKLE_RANGE);
	m_eDoc["STATUS"]["BEFOREBATTLEADDTIMER "].SetFloat(ImguiControl::c_beforeBattleAddTimer);
#pragma endregion

	Config::Writter("EnemyDataConfig", m_eDoc);
}

void Enemy::InportJson()
{
#pragma region FRAME
	ImguiControl::C_KICK_COLLISION_TIMER = m_eDoc["FRAME"]["KICK_COLLISION_TIMER         "].GetInt();
	ImguiControl::C_KICK_COLLISION_ENDTIMER = m_eDoc["FRAME"]["KICK_COLLISION_ENDTIMER      "].GetInt();
	ImguiControl::C_PUNCH_COLLISION_TIMER = m_eDoc["FRAME"]["PUNCH_COLLISION_TIMER        "].GetInt();
	ImguiControl::C_PUNCH_COLLISION_ENDTIMER = m_eDoc["FRAME"]["PUNCH_COLLISION_ENDTIMER     "].GetInt();
	ImguiControl::C_BACK_COLLISION_TIMER = m_eDoc["FRAME"]["BACK_COLLISION_TIMER         "].GetInt();
	ImguiControl::C_BACK_COLLISION_ENDTIMER = m_eDoc["FRAME"]["BACK_COLLISION_ENDTIMER      "].GetInt();
	ImguiControl::C_EXPLOSION_COLLISION_TIMER = m_eDoc["FRAME"]["EXPLOSION_COLLISION_TIMER    "].GetInt();
	ImguiControl::C_EXPLOSION_COLLISION_DELAY = m_eDoc["FRAME"]["EXPLOSION_COLLISION_DELAY    "].GetInt();
	ImguiControl::C_EXPLOSION_COLLISION_ENDTIMER = m_eDoc["FRAME"]["EXPLOSION_COLLISION_ENDTIMER "].GetInt();
	ImguiControl::C_SWIP_COLLISION_TIMER = m_eDoc["FRAME"]["SWIP_COLLISION_TIMER         "].GetInt();
	ImguiControl::C_SWIP_COLLISION_ENDTIMER = m_eDoc["FRAME"]["SWIP_COLLISION_ENDTIMER      "].GetInt();
	ImguiControl::C_RAZER_COLLISION_STARTTIMER = m_eDoc["FRAME"]["RAZER_COLLISION_STARTTIMER   "].GetInt();
	ImguiControl::C_RAZER_COLLISION_TIMER = m_eDoc["FRAME"]["RAZER_COLLISION_TIMER        "].GetInt();
	ImguiControl::C_RAZER_COLLISION_DELAY = m_eDoc["FRAME"]["RAZER_COLLISION_DELAY        "].GetInt();
	ImguiControl::C_SWINGDOWN_COLLISION_TIMER = m_eDoc["FRAME"]["SWINGDOWN_COLLISION_TIMER    "].GetInt();
	ImguiControl::C_SWINGDOWN_COLLISION_ENDTIMER = m_eDoc["FRAME"]["SWINGDOWN_COLLISION_ENDTIMER "].GetInt();
#pragma endregion

#pragma region OTHER
	ImguiControl::C_MAX_DIST = m_eDoc["OTHER"]["MAX_DIST        "].GetFloat();
	ImguiControl::C_MAX_TURN_RAD = m_eDoc["OTHER"]["MAX_TURN_RAD    "].GetFloat();
	ImguiControl::C_MAX_BACK_RAD = m_eDoc["OTHER"]["MAX_BACK_RAD    "].GetFloat();
	ImguiControl::C_MAX_BLEND_TIMER = m_eDoc["OTHER"]["MAX_BLEND_TIMER "].GetFloat();
#pragma endregion

#pragma region STATUS
	ImguiControl::C_MAX_POWER = m_eDoc["STATUS"]["MAX_POWER            "].GetInt();
	ImguiControl::C_MAX_EXPLOSION_POWER = m_eDoc["STATUS"]["MAX_EXPLOSION_POWER  "].GetInt();
	ImguiControl::C_MAX_HP = m_eDoc["STATUS"]["MAX_HP               "].GetInt();

	ImguiControl::C_RISE_TIMER = m_eDoc["STATUS"]["RISE_TIMER           "].GetInt();
	ImguiControl::C_SWING_DOWN_TIMER = m_eDoc["STATUS"]["SWING_DOWN_TIMER     "].GetInt();
	ImguiControl::C_MAX_RISE_HEIGHT = m_eDoc["STATUS"]["MAX_RISE_HEIGHT      "].GetFloat();
	ImguiControl::C_MAX_MOVE_SPEED = m_eDoc["STATUS"]["MAX_MOVE_SPEED       "].GetFloat();
	ImguiControl::C_MAX_TURN_TIMER = m_eDoc["STATUS"]["MAX_TURN_TIMER       "].GetFloat();
	ImguiControl::C_MAX_RISE_TIMER = m_eDoc["STATUS"]["MAX_RISE_TIMER       "].GetFloat();
	ImguiControl::C_MAX_SWING_DOWN_TIMER = m_eDoc["STATUS"]["MAX_SWING_DOWN_TIMER "].GetFloat();

	ImguiControl::C_MAX_TACKLE_TIMER = m_eDoc["STATUS"]["MAX_TACKLE_TIMER     "].GetInt();
	ImguiControl::C_MAX_TACKLE_COUNT = m_eDoc["STATUS"]["MAX_TACKLE_COUNT     "].GetInt();
	ImguiControl::C_MAX_TACKLE_SPEED = m_eDoc["STATUS"]["MAX_TACKLE_SPEED     "].GetFloat();
	ImguiControl::C_MAX_TACKLE_RANGE = m_eDoc["STATUS"]["MAX_TACKLE_RANGE     "].GetFloat();
	ImguiControl::C_CALC_TACKLE_RANGE = m_eDoc["STATUS"]["CALC_TACKLE_RANGE    "].GetFloat();
	ImguiControl::c_beforeBattleAddTimer = m_eDoc["STATUS"]["BEFOREBATTLEADDTIMER "].GetFloat();
#pragma endregion
}

void Enemy::CalcOBB()
{
	//OBB計算
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> l_affine =
		fbxobj_creature->GetAffineTrans();
	std::vector<DirectX::XMMATRIX> l_matRot =
		fbxobj_creature->GetMatRots();

	for (int i = 0; i < C_BOX_NUM; i++)
	{
		obj_Box[i]->SetPosition(DirectX::XMFLOAT3(
			ImguiControl::Imgui_enemyOBBPos[i][0],
			ImguiControl::Imgui_enemyOBBPos[i][1],
			ImguiControl::Imgui_enemyOBBPos[i][2]
		));
		obj_Box[i]->SetScale(DirectX::XMFLOAT3(
			ImguiControl::Imgui_enemyOBBScale[i][0],
			ImguiControl::Imgui_enemyOBBScale[i][1],
			ImguiControl::Imgui_enemyOBBScale[i][2]
		));
	}

	int i = 0;
	int l_eraseCount = 0;
	while (1)
	{
		if (boxes.size() <= i)
		{
			break;
		}

		if (boxes[i] == 0)
		{
			l_affine.erase(l_affine.begin() + (i - l_eraseCount));
			l_matRot.erase(l_matRot.begin() + (i - l_eraseCount));
			l_eraseCount++;
		}

		i++;
	}

	std::vector<OBB> l_obbs;
	for (int i = 0; i < l_affine.size(); i++)
	{
		obj_Box[i]->MultiMatrix(l_affine[i].second);
		obj_Box[i]->Update();

		OBB l_obb;
		l_obb.pos = DirectX::XMFLOAT3(
			l_affine[i].second.r[3].m128_f32[0],
			l_affine[i].second.r[3].m128_f32[1],
			l_affine[i].second.r[3].m128_f32[2]);
		l_obb.matrix = l_matRot[i];
		l_obb.length = obj_Box[i]->GetScale();

		l_obbs.push_back(l_obb);
	}
	m_obbs = l_obbs;
}

void Enemy::JudgAnimationType(float dist)
{
	int l_div = 300;
	int l_rand = std::rand();
	if (m_isTurn)
	{
		l_div = 1;
		l_rand = l_rand % l_div;

		if (l_rand == l_div - 1 &&
			m_animationType != RUN &&
			m_animationType != R_BACK &&
			m_animationType != L_BACK &&
			m_isBackAttackLottery)
		{
			if (m_turnVec == RIGHT)
			{
				m_animationType = R_BACK;
				fbxobj_creature->SetPosition(m_pos);
				fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
				fbxobj_creature->PlayAnimation(AnimationType::R_BACK);
			}
			else
			{
				m_animationType = L_BACK;
				fbxobj_creature->SetPosition(m_pos);
				fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
				fbxobj_creature->PlayAnimation(L_BACK);
			}

			m_isAttack = true;
			m_isCalcEnd = false;
			m_isBackAttackLottery = false;
			m_isBackAttack = true;
			m_isTurn = false;

			return;
		}
		else
		{
			if (m_turnVec == RIGHT)
			{
				m_animationType = R_TURN;
				fbxobj_creature->PlayAnimation(R_TURN);
			}
			else
			{
				m_animationType = L_TURN;
				fbxobj_creature->PlayAnimation(L_TURN);
			}
		}
	}
	else if (dist > C_MAX_DIST)
	{
		l_rand = l_rand % l_div;
		if (dist < C_MAX_TACKLE_RANGE)
		{
			if (l_rand == 0)
			{
				m_isAttack = true;

				m_isTackleEnd = false;
				m_tackleCount = 0;
				m_animationType = TACKLE;
				fbxobj_creature->PlayAnimation(TACKLE);
				return;
			}
		}
		m_animationType = RUN;
	}
	else
	{
		m_isAttack = true;
		m_isCalc = false;
		m_isCalcEnd = false;

		l_rand = l_rand % l_div;
		if (l_rand % 7 == 0 &&
			m_oldAnimationType != EXPLOSION &&
			GetHpRate() < 0.75f)
		{
			m_pPowerCount = 20;
			pManagerEx.SetCreateNum(m_createCount);
			pManagerEx.SetIsCreateStop(false);
			m_animationType = EXPLOSION;
			fbxobj_creature->PlayAnimation(EXPLOSION);
		}

		//l_rand % 5 == 0 &&GetHpRate() < 0.75f
		else if (l_rand % 5 == 0 && GetHpRate() < 0.75f)
		{
			//ここの管理方法を考える←仮
			m_riseStartY = 0.0f;
			m_animationType = RISE;
			fbxobj_creature->ResetAnimation(AnimationType::RISE);
			fbxobj_creature->ResetAnimation(AnimationType::SWING_DOWN);
		}

		else if (m_oldAnimationType != KICK)
		{
			m_animationType = KICK;
			fbxobj_creature->PlayAnimation(KICK);
		}
		else
		{
			m_animationType = PUNCH;
			fbxobj_creature->PlayAnimation(PUNCH);
		}
	}
}

void Enemy::CalcAngleDiff()
{
	if (m_isTurn)
	{
		m_deg = CalcDeg();
		if (fabsf(m_turnStartAngle - (m_deg + 180.0f)) >= 180.0f)
		{
			if (m_turnStartAngle < m_deg + 180.0f)
			{
				m_turnStartAngle += 359.9f;
			}
			else
			{
				m_deg += 359.9f;
			}
		}

		float l_rotY = ((m_deg + 180.0f) - m_turnStartAngle) * OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));

		if (m_easeTimer < 1.0f)
		{
			m_easeTimer += C_MAX_TURN_TIMER;
		}
		else
		{
			m_deg = l_rotY - 180.0f;
			if (fbxobj_creature->IsAnimationEnd(m_animationType))
			{
				m_easeTimer = 0.0f;
				m_isTurn = false;
			}
		}
	}
	else if (m_isBackAttack)
	{
		if (m_easeTimer < 1.0f)
		{
			float l_rotY = (m_turnEndAngle - m_turnStartAngle) *
				OgaJEase::easeOutSine(m_easeTimer) + m_turnStartAngle;
			fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, l_rotY, 0));
			m_easeTimer += C_MAX_TURN_TIMER;
		}

		else
		{
			m_deg = m_turnEndAngle - 180.0f;

			if (fbxobj_creature->IsAnimationEnd(m_animationType))
			{
				m_easeTimer = 0.0f;
				m_isBackAttack = false;
				m_isAttack = false;

				CalcNearAngle(fbxobj_creature->GetRotation().y);
				m_isBackAttackLottery = false;
			}
		}
	}
}

void Enemy::CalcNearAngle(float myAngleY)
{
	m_turnStartAngle = myAngleY;

	//正面ベクトルを角度に
	float rotY = myAngleY - 180.0f;
	OgaJHelper::ConvertToRadian(rotY);
	float l_x = sinf(rotY);
	float l_z = cosf(rotY);
	float l_enemyDeg = atan2(l_x, l_z);
	OgaJHelper::ConvertToDegree(l_enemyDeg);
	if (l_enemyDeg < 0)
	{
		l_enemyDeg += 359.9f;
	}

	//プレイヤー方向のベクトルを角度に
	DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(m_playerPos, m_pos);
	eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
	float l_frontDeg = atan2(eDirection.x, eDirection.z);
	OgaJHelper::ConvertToDegree(l_frontDeg);
	if (l_frontDeg < 0)
	{
		l_frontDeg += 359.9f;
	}

	float l_nearSub = OgaJHelper::RotateEarliestArc(l_enemyDeg, l_frontDeg);
	//自機が範囲外なら振り向きアニメーション抽選をする
	float l_nearSubAbs = fabsf(l_nearSub);
	if (l_nearSubAbs > C_MAX_TURN_RAD)
	{
		m_isTurn = true;

		if (l_nearSubAbs >= C_MAX_BACK_RAD)
		{
			if (!m_isBackAttackLottery && OgaJHelper::CalcDist(m_pos, m_playerPos) < C_MAX_DIST)
			{
				m_isBackAttackLottery = true;
			}
		}

		if (l_nearSub < 0)
		{
			m_turnVec = TurnVec::RIGHT;
		}
		else
		{
			m_turnVec = TurnVec::LEFT;
		}
		fbxobj_creature->SetPosition(m_pos);
	}
	else
	{
		m_isTurn = false;
	}

	m_turnEndAngle = myAngleY + l_nearSub;
	if (m_turnEndAngle < 0.0f)
	{
		m_turnEndAngle += 359.9f;
	}
	if (fabsf(m_turnStartAngle - m_turnEndAngle) >= 180.0f)
	{
		if (m_turnStartAngle < m_turnEndAngle)
		{
			m_turnStartAngle += 359.9f;
		}
		else
		{
			m_turnEndAngle += 359.9f;
		}
	}
}

void Enemy::CalcAttackCollisionTimer(const int startFrame, const int endFrame)
{
	if (!m_isCalc)
	{
		if (m_animationTimer < startFrame)
		{
			m_animationTimer++;
		}
		else
		{
			m_animationTimer = 0;
			m_isCalc = true;
		}
	}
	else
	{
		if (m_animationTimer < endFrame)
		{
			m_animationTimer++;
		}
		else
		{
			m_animationTimer = 0;
			m_isCalc = false;
			m_isCalcEnd = true;
		}
	}
}

void Enemy::CalcRise()
{
	if (m_animationTimer < C_RISE_TIMER)
	{
		m_animationTimer++;
	}
	else
	{
		m_pos.y = ((m_riseStartY + C_MAX_RISE_HEIGHT) - m_riseStartY) * OgaJEase::easeOutCubic(m_easeTimer) + m_riseStartY;
		if (m_easeTimer < 1.0f)
		{
			m_easeTimer += C_MAX_RISE_TIMER;
			if (m_easeTimer > 1.0f)
			{
				m_easeTimer = 1.0f;
			}
		}
		else
		{
			m_animationTimer = 0;
			m_easeTimer = 0.0f;
			m_swingDownStartPos = m_pos;
			m_swingDownEndPos = m_playerPos;
			m_animationType = AnimationType::SWING_DOWN;
			fbxobj_creature->SetPosition(m_pos);
			fbxobj_creature->StopAnimation(SWING_DOWN);
			fbxobj_creature->Update();
		}
		fbxobj_creature->SetPosition(m_pos);
	}
	fbxobj_creature->Update();
}

void Enemy::CalcSwingDown()
{
	if (m_animationTimer < C_SWING_DOWN_TIMER)
	{
		m_animationTimer++;
	}
	else
	{
		m_pos = OgaJEase::easeInCircXMFLOAT3(m_swingDownStartPos, m_swingDownEndPos, m_easeTimer);
		if (m_easeTimer < 1.0f)
		{
			m_easeTimer += C_MAX_SWING_DOWN_TIMER;
			if (m_easeTimer > 1.0f)
			{
				m_easeTimer = 1.0f;
			}
		}
		else
		{
			fbxobj_creature->PlayAnimation(SWING_DOWN);
			if (!m_isCalcEnd)
			{
				CalcAttackCollisionTimer(
					C_SWINGDOWN_COLLISION_TIMER, C_SWINGDOWN_COLLISION_ENDTIMER);
			}

			if (fbxobj_creature->IsAnimationEnd(SWING_DOWN))
			{
				m_animationTimer = 0;
				m_easeTimer = 0.0f;
				float l_dist = OgaJHelper::CalcDist(m_pos, m_playerPos);
				fbxobj_creature->SetPosition(m_pos);
				m_isAttack = false;
			}
		}
		fbxobj_creature->SetPosition(m_pos);
	}
	fbxobj_creature->Update();
}

void Enemy::CalcTackle()
{
	//違うとこでも結構求めてるから修正必要アリ(共有化)
	DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(m_playerPos, m_pos);
	eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);

	//突進前
	if (m_animationTimer < C_MAX_TACKLE_TIMER)
	{
		m_deg = atan2(eDirection.x, eDirection.z);
		OgaJHelper::ConvertToDegree(m_deg);
		m_animationTimer++;
		if (m_animationTimer >= C_MAX_TACKLE_TIMER)
		{
			m_tackleDirection = eDirection;
		}
	}
	else
	{
		m_pos.x += -m_tackleDirection.x * C_MAX_TACKLE_SPEED;
		m_pos.z += -m_tackleDirection.z * C_MAX_TACKLE_SPEED;

		float l_dist = OgaJHelper::CalcDist(m_pos, m_playerPos);
		//突進中
		if (!m_isTackleRange)
		{
			//範囲外(追従)
			if (l_dist > C_CALC_TACKLE_RANGE)
			{
				m_deg = atan2(eDirection.x, eDirection.z);
				OgaJHelper::ConvertToDegree(m_deg);
				m_tackleDirection = eDirection;
			}
			//範囲内
			else
			{
				m_isCalc = true;

				m_isTackleRange = true;
				m_tackleDirection = eDirection;
			}
		}

		//突進後,範囲外
		if (m_isTackleRange && l_dist > C_MAX_TACKLE_RANGE)
		{
			m_tackleCount++;
			m_animationTimer = 0;
			m_isTackleRange = false;

			//何回突進するか
			if (m_tackleCount >= C_MAX_TACKLE_COUNT)
			{
				m_isTackleEnd = true;
			}
		}

		//判定外
		if (l_dist > C_CALC_TACKLE_RANGE)
		{
			m_isCalc = false;
		}
	}
}

void Enemy::CalcExplosion()
{
	//パーティクル(解放前)
	const int l_randPosRange = 501;
	const float l_halfRandPosRange = l_randPosRange * 0.5f;	//-この値から+この値までが範囲になる。

	const int l_createAddFrame = 15;						//何フレーム毎に生成される個数が増えるか。
	const float l_emitPosY = 100.0f;						//吸収される高さ
	const float l_power = 1.0f;
	const float l_addPower = 1.1f;
	const float l_scale = 2.0f;
	const float l_colorR = 0.9f;
	const float l_colorG = 0.9f;
	const float l_colorB = 0.2f;

	//パーティクル(解放後)
	const int l_randPowerRange = 81;						//baseに加算される値の範囲,/10.0fされて加算
	const int l_randScaleRange = 101;						//baseに加算される値の範囲,/10.0fされて加算
	const int l_randColorRangeR = 21;						//baseに加算される値の範囲,/100.0fされて加算
	const int l_randColorRangeG = 51;						//baseに加算される値の範囲,/100.0fされて加算
	const float l_basePower = 3.0f;							//最低値
	const float l_baseScale = 5.0f;							//最低値
	const float l_baseColorR = 0.7f;						//最低値
	const float l_baseColorG = 0.4f;						//最低値
	const float l_baseColorB = 0.2f;						//最低値

	for (int i = 0; i < pManagerEx.GetMaxParticle(); i++)
	{
		if (!m_isExplosion)
		{
			if (!pManagerEx.IsMove(i))
			{
				DirectX::XMFLOAT3 l_pos = m_pos + DirectX::XMFLOAT3(
					rand() % l_randPosRange - l_halfRandPosRange,
					rand() % l_randPosRange - l_halfRandPosRange,
					rand() % l_randPosRange - l_halfRandPosRange);
				DirectX::XMFLOAT3 l_pos2 = m_pos;
				l_pos2.y = l_emitPosY;
				DirectX::XMFLOAT3 l_vec =
					OgaJHelper::CalcNormalizeVec3(OgaJHelper::CalcDirectionVec3(l_pos, l_pos2));

				pManagerEx.SetPosition(i, l_pos);
				pManagerEx.SetVec(i, l_vec);
				pManagerEx.SetPower(i, DirectX::XMFLOAT3(l_power, l_power, l_power));
				pManagerEx.SetScale(i, DirectX::XMFLOAT3(l_scale, l_scale, l_scale));
				pManagerEx.SetColor(i, DirectX::XMFLOAT4(l_colorR, l_colorG, l_colorB, 1.0f));
			}

			else
			{
				DirectX::XMFLOAT3 l_power = pManagerEx.GetPower(i);
				l_power.x *= l_addPower;
				l_power.y *= l_addPower;
				l_power.z *= l_addPower;
				pManagerEx.SetPower(i, l_power);
			}
		}
		else
		{
			pManagerEx.SetIsCreateStop(true);

			if (pManagerEx.IsMove(i))
			{
				if (pManagerEx.GetNowPosition(i).y < 0)
				{
					pManagerEx.SetNowScale(i, DirectX::XMFLOAT3(0, 0, 0));
				}
			}
		}
	}

	if (!m_isExplosion &&
		m_pPowerCount < C_EXPLOSION_COLLISION_TIMER + C_EXPLOSION_COLLISION_DELAY + C_EXPLOSION_COLLISION_ENDTIMER)
	{
		if (m_pPowerCount % l_createAddFrame == 0)
		{
			m_createCount++;
			pManagerEx.SetCreateNum(m_createCount);
		}
		if (m_pPowerCount == C_EXPLOSION_COLLISION_TIMER)
		{
			pManagerEx.SetIsCreateStop(true);
		}
		if (m_pPowerCount >= C_EXPLOSION_COLLISION_TIMER + C_EXPLOSION_COLLISION_DELAY)
		{
			m_isExplosion = true;
			pManagerEx.SetCreateNum(pManagerEx.GetMaxParticle());
			pManagerEx.SetIsCreateStop(false);
			pManagerEx.ResetLifeParticle();

			for (int i = 0; i < pManagerEx.GetMaxParticle(); i++)
			{
				DirectX::XMFLOAT3 l_pos = m_pos + DirectX::XMFLOAT3(
					rand() % l_randPosRange - l_halfRandPosRange,
					rand() % l_randPosRange - l_halfRandPosRange + l_emitPosY,
					rand() % l_randPosRange - l_halfRandPosRange);
				DirectX::XMFLOAT3 l_pos2 = m_pos;
				l_pos2.y = l_emitPosY;
				DirectX::XMFLOAT3 l_vec =
					OgaJHelper::CalcNormalizeVec3(OgaJHelper::CalcDirectionVec3(l_pos2, l_pos));

				pManagerEx.SetPosition(i, l_pos2);
				pManagerEx.SetVec(i, l_vec);
				pManagerEx.SetPower(i, DirectX::XMFLOAT3(
					l_basePower + (rand() % l_randPowerRange / 10.0f),
					l_basePower + (rand() % l_randPowerRange / 10.0f),
					l_basePower + (rand() % l_randPowerRange / 10.0f)));
				pManagerEx.SetScale(i, DirectX::XMFLOAT3(
					l_baseScale + (rand() % l_randScaleRange / 10.0f),
					l_baseScale + (rand() % l_randScaleRange / 10.0f),
					l_baseScale + (rand() % l_randScaleRange / 10.0f)));
				pManagerEx.SetColor(i, DirectX::XMFLOAT4(
					l_baseColorR + (rand() % l_randColorRangeR / 100.0f),
					l_baseColorG + (rand() % l_randColorRangeG / 100.0f),
					l_baseColorB, 1.0f));
			}
		}
	}
	else if (m_isExplosion)
	{
		if (m_pPowerCount >
			C_EXPLOSION_COLLISION_TIMER + C_EXPLOSION_COLLISION_DELAY + C_EXPLOSION_COLLISION_ENDTIMER)
		{
			m_isExplosion = false;
			pManagerEx.SetIsCreateStop(true);
		}
	}
	m_pPowerCount++;
}

void Enemy::CalcSwip()
{
	if (!m_isCalcEnd)
	{
		CalcAttackCollisionTimer(C_SWIP_COLLISION_TIMER, C_SWIP_COLLISION_ENDTIMER);
	}
	if (!m_isCalc && !m_isCalcEnd)
	{
		m_deg = CalcDeg();
	}

	fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
	fbxobj_creature->Update();

	if (fbxobj_creature->IsAnimationEnd(SWIP))
	{
		m_isAttack = false;

		CalcNearAngle(fbxobj_creature->GetRotation().y);
	}
}

void Enemy::CalcRazer()
{
	const float l_razer_dist = 100.0f;
	const int l_rand_range = 51;
	const int l_rand_after_range = 31;

	//撃つ前
	if (m_animationTimer <
		C_RAZER_COLLISION_TIMER + C_RAZER_COLLISION_DELAY)
	{
		m_animationTimer++;

		//溜め
		if (m_animationTimer < C_RAZER_COLLISION_TIMER)
		{
			//追従
			m_deg = CalcDeg();

			//パーティクル集合地点
			float rotY = m_deg - 180.0f;
			OgaJHelper::ConvertToRadian(rotY);
			float l_x = sinf(rotY);
			float l_z = cosf(rotY);
			DirectX::XMFLOAT3 l_goalPos = m_pos;
			l_goalPos.x += l_x * l_razer_dist;
			l_goalPos.y = 60.0f;
			l_goalPos.z += l_z * l_razer_dist;

			//最初は生成しない
			if (m_animationTimer > C_RAZER_COLLISION_STARTTIMER)
			{
				//パーティクル生成
				for (int i = 0; i < pManagerRazer.GetMaxParticle(); i++)
				{
					//生成時
					if (!pManagerRazer.IsMove(i))
					{
						pManagerRazer.SetPower(i, DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
						pManagerRazer.SetPosition(i, DirectX::XMFLOAT3(
							l_goalPos.x + (rand() % l_rand_range - l_rand_range * 0.5f),
							l_goalPos.y + (rand() % l_rand_range - l_rand_range * 0.5f),
							l_goalPos.z + (rand() % l_rand_range - l_rand_range * 0.5f)
						));
					}
					else
					{
						DirectX::XMFLOAT3 l_pToGoalVec =
							OgaJHelper::CalcNormalizeVec3(
								OgaJHelper::CalcDirectionVec3(
									pManagerRazer.GetNowPosition(i), l_goalPos));
						pManagerRazer.SetVec(i, l_pToGoalVec);
					}
				}
			}
		}

		//超えたら
		if (m_animationTimer ==
			C_RAZER_COLLISION_TIMER)
		{
			pManagerRazer.SetIsCreateStop(true);
		}

		if (m_animationTimer >=
			C_RAZER_COLLISION_TIMER + C_RAZER_COLLISION_DELAY)
		{
			pManagerRazer.SetIsCreateStop(false);
		}
	}

	//発射中
	else
	{
		const float l_rotate_speed = 0.2f;

		//自機の方に段々傾ける
		//正面
		float rotY = m_deg - 180.0f;
		OgaJHelper::ConvertToRadian(rotY);
		float l_x = sinf(rotY);
		float l_z = cosf(rotY);
		float l_enemyDeg = atan2(l_x, l_z);
		OgaJHelper::ConvertToDegree(l_enemyDeg);
		if (l_enemyDeg < 0)
		{
			l_enemyDeg += 359.9f;
		}

		//プレイヤー方向の角度
		DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(m_playerPos, m_pos);
		eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
		float l_frontDeg = atan2(eDirection.x, eDirection.z);
		OgaJHelper::ConvertToDegree(l_frontDeg);
		if (l_frontDeg < 0)
		{
			l_frontDeg += 359.9f;
		}

		float l_nearSub = OgaJHelper::RotateEarliestArc(l_enemyDeg, l_frontDeg);
		if (l_nearSub < 0)
		{
			m_deg += l_rotate_speed;
		}
		else
		{
			m_deg -= l_rotate_speed;
		}

		//ヒット判定
		if (fabsf(l_nearSub - 180.0f) < 20.0f)
		{
			m_isRazerHit = true;
		}
		else
		{
			m_isRazerHit = false;
		}

		//パーティクル集合地点
		DirectX::XMFLOAT3 l_goalPos = m_pos;
		l_goalPos.x += l_x * l_razer_dist;
		l_goalPos.y = 30.0f;
		l_goalPos.z += l_z * l_razer_dist;

		//飛ばす
		//パーティクル生成
		for (int i = 0; i < pManagerRazer.GetMaxParticle(); i++)
		{
			//生成時
			if (!pManagerRazer.IsMove(i))
			{
				pManagerRazer.SetCreateNum(5);
				pManagerRazer.SetLife(i, 20);
				pManagerRazer.SetPower(i, DirectX::XMFLOAT3(15.0f, 15.0f, 15.0f));
				pManagerRazer.SetPosition(i, DirectX::XMFLOAT3(
					l_goalPos.x + (rand() % l_rand_after_range - l_rand_after_range * 0.5f),
					l_goalPos.y + (rand() % l_rand_after_range - l_rand_after_range * 0.5f),
					l_goalPos.z + (rand() % l_rand_after_range - l_rand_after_range * 0.5f)
				));
			}
			else
			{
				pManagerRazer.SetVec(i, DirectX::XMFLOAT3(l_x, 0, l_z));
			}
		}
		//当たったら強制終了させたい

		if (fbxobj_creature->GetCurrentNowTime() > 6.8f)
		{
			m_isRazerHit = false;
			pManagerRazer.SetIsCreateStop(true);
		}

		if (fbxobj_creature->IsAnimationEnd(RAZER))
		{
			m_animationTimer = 0;
			m_isAttack = false;
			CalcNearAngle(fbxobj_creature->GetRotation().y);
			m_isRazerHit = false;
		}
	}

	fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
	fbxobj_creature->Update();
}

void Enemy::OtherUpdate()
{
	//FbxUpdate
	switch (m_animationType)
	{
	case STAND:
	{
		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature->SetPosition(m_pos);
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "STAND";
		break;
	}
	case RUN:
	{
		m_isAttack = false;
		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature->SetPosition(m_pos);
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "RUN";
		break;
	}
	case KICK:
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_KICK_COLLISION_TIMER, C_KICK_COLLISION_ENDTIMER);
		}
		if (!m_isCalc && !m_isCalcEnd)
		{
			m_deg = CalcDeg();
		}

		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature->SetPosition(m_pos);
		fbxobj_creature->Update();

		if (m_isCalc)
		{
			obj_Box[9]->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			obj_Box[9]->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (fbxobj_creature->IsAnimationEnd(KICK))
		{
			m_isAttack = false;

			CalcNearAngle(fbxobj_creature->GetRotation().y);
		}

		ImguiControl::Imgui_enemyAniType = "KICK";
		break;
	}
	case PUNCH:
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_PUNCH_COLLISION_TIMER, C_PUNCH_COLLISION_ENDTIMER);
		}
		if (!m_isCalc && !m_isCalcEnd)
		{
			m_deg = CalcDeg();
		}

		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature->SetPosition(m_pos);
		fbxobj_creature->Update();

		if (m_isCalc)
		{
			obj_Box[5]->SetColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			obj_Box[5]->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (fbxobj_creature->IsAnimationEnd(PUNCH))
		{
			m_isAttack = false;

			CalcNearAngle(fbxobj_creature->GetRotation().y);
		}

		ImguiControl::Imgui_enemyAniType = "PUNCH";
		break;
	}
	case DIE:
	{
		fbxobj_creature->Update();

		if (fbxobj_creature->IsAnimationEnd(AnimationType::DIE))
		{
			fbxobj_creature->StopAnimation(AnimationType::DIE);
		}
		ImguiControl::Imgui_enemyAniType = "DIE";
		break;
	}
	case R_TURN:
	{
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "R_TURN";
		break;
	}
	case L_TURN:
	{
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "L_TURN";
		break;
	}
	case R_BACK:
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "R_BACK";
		break;
	}
	case L_BACK:
	{
		if (!m_isCalcEnd)
		{
			CalcAttackCollisionTimer(C_BACK_COLLISION_TIMER, C_BACK_COLLISION_ENDTIMER);
		}
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "L_BACK";
		break;
	}
	case EXPLOSION:
	{
		CalcExplosion();

		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature->SetPosition(m_pos);
		fbxobj_creature->Update();

		if (fbxobj_creature->IsAnimationEnd(EXPLOSION))
		{
			m_isAttack = false;

			CalcNearAngle(fbxobj_creature->GetRotation().y);

			m_pPowerCount = 0;
			m_createCount = 1;
			m_isExplosion = false;
			pManagerEx.SetIsCreateStop(true);
		}

		ImguiControl::Imgui_enemyAniType = "EXPLOSION";
		break;
	}
	case RISE:
	{
		CalcRise();

		ImguiControl::Imgui_enemyAniType = "RISE";
		break;
	}
	case SWING_DOWN:
	{
		CalcSwingDown();

		ImguiControl::Imgui_enemyAniType = "SWING_DOWN";
		break;
	}
	case TACKLE:
	{
		CalcTackle();

		fbxobj_creature->SetRotation(DirectX::XMFLOAT3(0, m_deg + 180.0f, 0));
		fbxobj_creature->SetPosition(m_pos);
		fbxobj_creature->Update();

		if (m_isTackleEnd)
		{
			m_animationType = RAZER;
			m_animationTimer = 0;
			fbxobj_creature->PlayAnimation(RAZER);
			pManagerRazer.SetIsCreateStop(false);
			for (int i = 0; i < pManagerRazer.GetMaxParticle(); i++)
			{
				pManagerRazer.SetPosition(i, m_pos);
			}
		}

		ImguiControl::Imgui_enemyAniType = "TACKLE";
		break;
	}
	case AWAKE:
	{
		fbxobj_creature->Update();

		ImguiControl::Imgui_enemyAniType = "AWAKE";
		break;
	}
	case SWIP:
	{
		CalcSwip();
		ImguiControl::Imgui_enemyAniType = "SWIP";
		break;
	}
	case RAZER:
	{
		CalcRazer();
		ImguiControl::Imgui_enemyAniType = "RAZER";
		break;
	}
	}

	pManagerEx.Update();
}

void Enemy::SetImgui()
{
	switch (m_oldAnimationType)
	{
	case STAND:
	{
		ImguiControl::Imgui_enemyOldAniType = "STAND";
		break;
	}
	case RUN:
	{
		ImguiControl::Imgui_enemyOldAniType = "RUN";
		break;
	}
	case KICK:
	{
		ImguiControl::Imgui_enemyOldAniType = "KICK";
		break;
	}
	case PUNCH:
	{
		ImguiControl::Imgui_enemyOldAniType = "PUNCH";
		break;
	}
	case R_TURN:
	{
		ImguiControl::Imgui_enemyOldAniType = "R_TURN";
		break;
	}
	case L_TURN:
	{
		ImguiControl::Imgui_enemyOldAniType = "L_TURN";
		break;
	}
	case R_BACK:
	{
		ImguiControl::Imgui_enemyOldAniType = "R_BACK";
		break;
	}
	case L_BACK:
	{
		ImguiControl::Imgui_enemyOldAniType = "L_BACK";
		break;
	}
	case EXPLOSION:
	{
		ImguiControl::Imgui_enemyOldAniType = "EXPLOSION";
		break;
	}
	case RISE:
	{
		ImguiControl::Imgui_enemyOldAniType = "RISE";
		break;
	}
	case SWING_DOWN:
	{
		ImguiControl::Imgui_enemyOldAniType = "SWING_DOWN";
		break;
	}
	case TACKLE:
	{
		ImguiControl::Imgui_enemyOldAniType = "TACKLE";
		break;
	}
	case AWAKE:
	{
		ImguiControl::Imgui_enemyOldAniType = "AWAKE";
		break;
	}
	case SWIP:
	{
		ImguiControl::Imgui_enemyOldAniType = "SWIP";
		break;
	}
	case RAZER:
	{
		ImguiControl::Imgui_enemyOldAniType = "RAZER";
		break;
	}
	}

	//無敵かどうか
	if (m_isInvincible)
	{
		ImguiControl::Imgui_enemyInvi = "TRUE";
	}
	else
	{
		ImguiControl::Imgui_enemyInvi = "FALSE";
	}

	ImguiControl::Imgui_enemyBlendTimer = m_blendTimer;
	ImguiControl::Imgui_enemyCurrentAniTimer = fbxobj_creature->GetNowTime(m_animationType);
	ImguiControl::Imgui_enemyOldAniTimer = fbxobj_creature->GetNowTime(m_oldAnimationType);
}

void Enemy::CalcBlendAnimation()
{
	//切り替わったら
	if (!m_isChange &&
		m_animationType != m_oldAnimationType)
	{
		m_blendTimer = 0.0f;
		m_isChange = true;
		m_keepAnimationType = m_animationType;
		fbxobj_creature->ResetAnimation(m_animationType);
	}

	//補間計算
	else
	{
		//途中切り替え時
		if (m_animationType != m_keepAnimationType)
		{
			m_blendTimer = 0.0f;
			m_keepAnimationType = m_animationType;
			fbxobj_creature->ResetAnimation(m_animationType);
		}

		if (m_blendTimer < 1.0f)
		{
			m_blendTimer += C_MAX_BLEND_TIMER;
			if (m_blendTimer > 1.0f)
			{
				m_blendTimer = 1.0f;
				m_isChange = false;
				m_oldAnimationType = m_animationType;
			}
		}
		else
		{
			m_isChange = false;
			m_oldAnimationType = m_animationType;
		}
	}

	//BlendAnimation
	fbxobj_creature->
		BlendAnimation(m_oldAnimationType, m_animationType, OgaJEase::easeOutCubic(m_blendTimer), true);
}

float Enemy::CalcDeg()
{
	DirectX::XMFLOAT3 eDirection = OgaJHelper::CalcDirectionVec3(m_playerPos, m_pos);
	eDirection = OgaJHelper::CalcNormalizeVec3(eDirection);
	float l_deg = atan2(eDirection.x, eDirection.z);
	OgaJHelper::ConvertToDegree(l_deg);
	return l_deg;
}

void Enemy::CalcArea()
{
	//エリア外判定
	float l_dist = OgaJHelper::CalcDist(DirectX::XMFLOAT3(0, 0, 0), m_pos);
	if (l_dist > ImguiControl::Imgui_stageArea)
	{
		float l_sub = l_dist - ImguiControl::Imgui_stageArea;
		DirectX::XMFLOAT3 l_backVec = OgaJHelper::CalcDirectionVec3(
			m_pos, DirectX::XMFLOAT3(0, 0, 0));
		l_backVec = OgaJHelper::CalcNormalizeVec3(l_backVec);
		float l_backX = l_backVec.x * l_sub;
		float l_backZ = l_backVec.z * l_sub;
		m_pos.x += l_backX;
		m_pos.z += l_backZ;
	}
}

/*----------呼ぶやつ----------*/
void Enemy::HitAttack(int damage)
{
	m_hp -= damage;
	if (m_hp <= 0) { m_hp = 0; }
	m_isInvincible = true;

	DirectX::XMFLOAT3 l_pos = m_obbs[m_hitOBBNum].pos;
	for (int i = 0; i < pManagerHit.GetMaxParticle(); i++)
	{
		//生成時のみ剣に付随
		if (!pManagerHit.IsMove(i))
		{
			pManagerHit.SetPosition(i, l_pos);
		}
	}
	pManagerHit.SetIsCreateStop(false);

	OutputDebugStringA("Hit!\n");
}

std::vector<OBB> Enemy::GetAttackOBBs()
{
	std::vector<OBB> l_obb;
	if (m_animationType == KICK)
	{
		l_obb = { m_obbs[9] };
		return l_obb;
	}
	else if (m_animationType == PUNCH)
	{
		l_obb = { m_obbs[3] };
		return l_obb;
	}
	else if (m_animationType == R_BACK ||
		m_animationType == L_BACK)
	{
		l_obb = { m_obbs[3],m_obbs[5] };
		return l_obb;
	}
	else
	{
		return m_obbs;
	}
}

bool Enemy::IsDead()
{
	if (m_hp <= 0) { return true; }
	return false;
}

bool Enemy::IsFighting()
{
	if (m_animationType == STAND) { return false; }
	return true;
}
