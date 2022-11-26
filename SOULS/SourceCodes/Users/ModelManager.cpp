#include "ModelManager.h"

//Player
Model* ModelManager::model_sword = nullptr;
Model* ModelManager::model_box = nullptr;
Model* ModelManager::model_box2 = nullptr;
Model* ModelManager::model_helmet = nullptr;

FbxModels* ModelManager::fbxmodel_standMiku = nullptr;
FbxModels* ModelManager::fbxmodel_slowRunMiku = nullptr;
FbxModels* ModelManager::fbxmodel_fastRunMiku = nullptr;
FbxModels* ModelManager::fbxmodel_oneSwrordAttack = nullptr;
FbxModels* ModelManager::fbxmodel_oneSwrordAttack2 = nullptr;
FbxModels* ModelManager::fbxmodel_oneSwrordAttack3 = nullptr;
FbxModels* ModelManager::fbxmodel_dieMiku = nullptr;
FbxModels* ModelManager::fbxmodel_impactMiku = nullptr;
FbxModels* ModelManager::fbxmodel_rollingMiku = nullptr;

//Enemy
FbxModels* ModelManager::fbxmodel_idleCreature = nullptr;
FbxModels* ModelManager::fbxmodel_runCreature = nullptr;
FbxModels* ModelManager::fbxmodel_kickCreature = nullptr;
FbxModels* ModelManager::fbxmodel_punchCreature = nullptr;
FbxModels* ModelManager::fbxmodel_dieCreature = nullptr;

FbxModels* ModelManager::fbxmodel_RTurnCreature = nullptr;
FbxModels* ModelManager::fbxmodel_LTurnCreature = nullptr;
FbxModels* ModelManager::fbxmodel_RBackCreature = nullptr;
FbxModels* ModelManager::fbxmodel_LBackCreature = nullptr;
FbxModels* ModelManager::fbxmodel_explosionCreature = nullptr;
FbxModels* ModelManager::fbxmodel_riseCreature = nullptr;
FbxModels* ModelManager::fbxmodel_swingDownCreature = nullptr;

//Stage
Model* ModelManager::model_stage = nullptr;
Model* ModelManager::model_sponza = nullptr;

ModelManager::~ModelManager()
{
	//Player
	delete model_sword;
	delete model_box;
	delete model_helmet;

	delete fbxmodel_standMiku;
	delete fbxmodel_slowRunMiku;
	delete fbxmodel_fastRunMiku;
	delete fbxmodel_oneSwrordAttack;
	delete fbxmodel_oneSwrordAttack2;
	delete fbxmodel_oneSwrordAttack3;
	delete fbxmodel_dieMiku;
	delete fbxmodel_impactMiku;
	delete fbxmodel_rollingMiku;

	//Enemy
	delete fbxmodel_idleCreature;
	delete fbxmodel_runCreature;
	delete fbxmodel_kickCreature;
	delete fbxmodel_punchCreature;
	delete fbxmodel_dieCreature;

	delete fbxmodel_RTurnCreature;
	delete fbxmodel_LTurnCreature;
	delete fbxmodel_RBackCreature;
	delete fbxmodel_LBackCreature;
	delete fbxmodel_explosionCreature;
	delete fbxmodel_riseCreature;
	delete fbxmodel_swingDownCreature;

	//Stage
	delete model_stage;
	delete model_sponza;
}

void ModelManager::Init()
{
	//Player
	model_sword = Model::CreateFromObj("Sword");
	model_box = Model::CreateFromObj("TestBox");
	model_box2 = Model::CreateFromObj("TestBox2");
	model_helmet = Model::CreateFromObj("Artorius_Helmet");

	fbxmodel_standMiku = FbxModels::CreateFromFbx("StandMiku", "Miku");
	fbxmodel_slowRunMiku = FbxModels::CreateFromFbx("SlowRunMiku", "Miku");
	fbxmodel_fastRunMiku = FbxModels::CreateFromFbx("RunMiku", "Miku");
	fbxmodel_oneSwrordAttack = FbxModels::CreateFromFbx("OneSwordAttack", "Miku");
	fbxmodel_oneSwrordAttack2 = FbxModels::CreateFromFbx("OneSwordAttack2", "Miku");
	fbxmodel_oneSwrordAttack3 = FbxModels::CreateFromFbx("OneSwordAttack3", "Miku");
	fbxmodel_dieMiku = FbxModels::CreateFromFbx("DyingMiku", "Miku");
	fbxmodel_impactMiku = FbxModels::CreateFromFbx("ImpactMiku", "Miku");
	fbxmodel_rollingMiku = FbxModels::CreateFromFbx("RollingMiku", "Miku");

	//Enemy
	fbxmodel_idleCreature = FbxModels::CreateFromFbx("IdleCreature", "Creature");
	fbxmodel_runCreature = FbxModels::CreateFromFbx("RunCreature", "Creature");
	fbxmodel_kickCreature = FbxModels::CreateFromFbx("KickingCreature", "Creature");
	fbxmodel_punchCreature = FbxModels::CreateFromFbx("PunchingCreature", "Creature");
	fbxmodel_dieCreature = FbxModels::CreateFromFbx("DyingCreature", "Creature");

	fbxmodel_RTurnCreature = FbxModels::CreateFromFbx("RTurnCreature", "Creature");
	fbxmodel_LTurnCreature = FbxModels::CreateFromFbx("LTurnCreature", "Creature");
	fbxmodel_RBackCreature = FbxModels::CreateFromFbx("RBackAttackCreature", "Creature");
	fbxmodel_LBackCreature = FbxModels::CreateFromFbx("LBackAttackCreature", "Creature");
	fbxmodel_explosionCreature = FbxModels::CreateFromFbx("ExplosionCreature", "Creature");
	fbxmodel_riseCreature = FbxModels::CreateFromFbx("RiseCreature", "Creature");
	fbxmodel_swingDownCreature = FbxModels::CreateFromFbx("SwingDownCreature", "Creature");

	//Stage
	model_stage = Model::CreateFromObj("yuka");
	model_sponza = Model::CreateFromObj("yuka");

	FbxModels::ReleaseModels();
}
