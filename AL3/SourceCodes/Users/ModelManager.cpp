#include "ModelManager.h"
#include "../3D/FbxLoader.h"

//Player
Model* ModelManager::model_sword = nullptr;
Model* ModelManager::model_box = nullptr;

FbxModels* ModelManager::fbxmodel_standMiku = nullptr;
FbxModels* ModelManager::fbxmodel_slowRunMiku = nullptr;
FbxModels* ModelManager::fbxmodel_fastRunMiku = nullptr;
FbxModels* ModelManager::fbxmodel_oneSwrordAttack = nullptr;
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

	delete fbxmodel_standMiku;
	delete fbxmodel_slowRunMiku;
	delete fbxmodel_fastRunMiku;
	delete fbxmodel_oneSwrordAttack;
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
	fbxmodel_standMiku = FbxModels::CreateFromFbx("StandMiku");
	fbxmodel_slowRunMiku = FbxModels::CreateFromFbx("SlowRunMiku");
	fbxmodel_fastRunMiku = FbxModels::CreateFromFbx("RunMiku");
	fbxmodel_oneSwrordAttack = FbxModels::CreateFromFbx("OneSwordAttack");
	fbxmodel_dieMiku = FbxModels::CreateFromFbx("DyingMiku");
	fbxmodel_impactMiku = FbxModels::CreateFromFbx("ImpactMiku");
	fbxmodel_rollingMiku = FbxModels::CreateFromFbx("RollingMiku");

	//Enemy
	fbxmodel_idleCreature = FbxModels::CreateFromFbx("IdleCreature");
	fbxmodel_runCreature = FbxModels::CreateFromFbx("RunCreature");
	fbxmodel_kickCreature = FbxModels::CreateFromFbx("KickingCreature");
	fbxmodel_punchCreature = FbxModels::CreateFromFbx("PunchingCreature");
	fbxmodel_dieCreature = FbxModels::CreateFromFbx("DyingCreature");

	fbxmodel_RTurnCreature = FbxModels::CreateFromFbx("RTurnCreature");
	fbxmodel_LTurnCreature = FbxModels::CreateFromFbx("LTurnCreature");
	fbxmodel_RBackCreature = FbxModels::CreateFromFbx("RBackAttackCreature");
	fbxmodel_LBackCreature = FbxModels::CreateFromFbx("LBackAttackCreature");
	fbxmodel_explosionCreature = FbxModels::CreateFromFbx("ExplosionCreature");
	fbxmodel_riseCreature = FbxModels::CreateFromFbx("RiseCreature");
	fbxmodel_swingDownCreature = FbxModels::CreateFromFbx("SwingDownCreature");

	//Stage
	model_stage = Model::CreateFromObj("yuka");
	model_sponza = Model::CreateFromObj("sponza");
}
