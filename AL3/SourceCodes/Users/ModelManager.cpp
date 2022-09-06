#include "ModelManager.h"
#include "../3D/FbxLoader.h"

//Player
Model* ModelManager::model_sword = nullptr;
Model* ModelManager::model_box = nullptr;

FbxModel* ModelManager::fbxmodel_standMiku = nullptr;
FbxModel* ModelManager::fbxmodel_slowRunMiku = nullptr;
FbxModel* ModelManager::fbxmodel_fastRunMiku = nullptr;
FbxModel* ModelManager::fbxmodel_oneSwrordAttack = nullptr;
FbxModel* ModelManager::fbxmodel_dieMiku = nullptr;
FbxModel* ModelManager::fbxmodel_impactMiku = nullptr;
FbxModel* ModelManager::fbxmodel_rollingMiku = nullptr;

//Enemy
FbxModel* ModelManager::fbxmodel_idleCreature = nullptr;
FbxModel* ModelManager::fbxmodel_runCreature = nullptr;
FbxModel* ModelManager::fbxmodel_kickCreature = nullptr;
FbxModel* ModelManager::fbxmodel_punchCreature = nullptr;
FbxModel* ModelManager::fbxmodel_dieCreature = nullptr;

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

	//Stage
	delete model_stage;
	delete model_sponza;
}

void ModelManager::Init()
{
	//Player
	model_sword = Model::CreateFromObj("Sword");
	model_box = Model::CreateFromObj("TestBox");
	fbxmodel_standMiku = FbxLoader::GetInstance()->LoadModelFromFile("StandMiku");
	fbxmodel_slowRunMiku = FbxLoader::GetInstance()->LoadModelFromFile("SlowRunMiku");
	fbxmodel_fastRunMiku = FbxLoader::GetInstance()->LoadModelFromFile("RunMiku");
	fbxmodel_oneSwrordAttack = FbxLoader::GetInstance()->LoadModelFromFile("OneSwordAttack");
	fbxmodel_dieMiku = FbxLoader::GetInstance()->LoadModelFromFile("DyingMiku");
	fbxmodel_impactMiku = FbxLoader::GetInstance()->LoadModelFromFile("ImpactMiku");
	fbxmodel_rollingMiku = FbxLoader::GetInstance()->LoadModelFromFile("RollingMiku");

	//Enemy
	fbxmodel_idleCreature = FbxLoader::GetInstance()->LoadModelFromFile("IdleCreature");
	fbxmodel_runCreature = FbxLoader::GetInstance()->LoadModelFromFile("RunCreature");
	fbxmodel_kickCreature = FbxLoader::GetInstance()->LoadModelFromFile("KickingCreature");
	fbxmodel_punchCreature = FbxLoader::GetInstance()->LoadModelFromFile("PunchingCreature");
	fbxmodel_dieCreature = FbxLoader::GetInstance()->LoadModelFromFile("DyingCreature");

	//Stage
	model_stage = Model::CreateFromObj("yuka");
	model_sponza = Model::CreateFromObj("sponza");
}
