#include "ModelManager.h"

//Player
std::shared_ptr<Model> ModelManager::model_sword = nullptr;
std::shared_ptr<Model> ModelManager::model_box = nullptr;
std::shared_ptr<Model> ModelManager::model_box2 = nullptr;
std::shared_ptr<Model> ModelManager::model_helmet = nullptr;

std::shared_ptr<FbxModels> ModelManager::fbxmodel_mikus = nullptr;

//Enemy
std::shared_ptr<FbxModels> ModelManager::fbxmodel_creatures = nullptr;

//Stage
std::shared_ptr<Model> ModelManager::model_stage = nullptr;
std::shared_ptr<Model> ModelManager::model_arenaFront = nullptr;
std::shared_ptr<Model> ModelManager::model_arenaBack = nullptr;

//StageObject
std::shared_ptr<Model> ModelManager::model_sphere = nullptr;
std::shared_ptr<Model> ModelManager::model_bonefire = nullptr;

ModelManager::~ModelManager()
{
}

void ModelManager::Init()
{
	//Player
	model_sword = Model::CreateFromObj("Sword");
	model_box = Model::CreateFromObj("TestBox");
	model_box2 = Model::CreateFromObj("TestBox2");
	model_helmet = Model::CreateFromObj("knight_Helmet");

	fbxmodel_mikus = FbxModels::CreateFromFbx("StandMiku");
	fbxmodel_mikus->AddAnimation("SlowRunMiku");
	fbxmodel_mikus->AddAnimation("RunMiku");
	fbxmodel_mikus->AddAnimation("BackRunMiku");
	fbxmodel_mikus->AddAnimation("RightRunMiku");
	fbxmodel_mikus->AddAnimation("LeftRunMiku");
	fbxmodel_mikus->AddAnimation("OneSwordAttack");
	fbxmodel_mikus->AddAnimation("OneSwordAttack2");
	fbxmodel_mikus->AddAnimation("OneSwordAttack3");
	fbxmodel_mikus->AddAnimation("ImpactMiku");
	fbxmodel_mikus->AddAnimation("RollingMiku");
	fbxmodel_mikus->AddAnimation("DyingMiku");
	fbxmodel_mikus->AddAnimation("DrinkingMiku");

	//Enemy
	fbxmodel_creatures = FbxModels::CreateFromFbx("IdleCreature");
	fbxmodel_creatures->AddAnimation("RunCreature");
	fbxmodel_creatures->AddAnimation("KickingCreature");
	fbxmodel_creatures->AddAnimation("PunchingCreature");
	fbxmodel_creatures->AddAnimation("DyingCreature");
	fbxmodel_creatures->AddAnimation("RTurnCreature");
	fbxmodel_creatures->AddAnimation("LTurnCreature");
	fbxmodel_creatures->AddAnimation("RBackAttackCreature");
	fbxmodel_creatures->AddAnimation("LBackAttackCreature");
	fbxmodel_creatures->AddAnimation("ExplosionCreature");
	fbxmodel_creatures->AddAnimation("RiseCreature");
	fbxmodel_creatures->AddAnimation("SwingDownCreature");
	fbxmodel_creatures->AddAnimation("TackleCreature");
	fbxmodel_creatures->AddAnimation("AwakeningCreature");
	fbxmodel_creatures->AddAnimation("SwipingCreature");
	fbxmodel_creatures->AddAnimation("MagicCreature");
	fbxmodel_creatures->AddAnimation("RazerCreature");
	fbxmodel_creatures->AddAnimation("UltReady");
	fbxmodel_creatures->AddAnimation("Ultimate");

	//Stage
	model_stage = Model::CreateFromObj("yuka");
	model_arenaFront = Model::CreateFromObj("arena");

	//StageObject
	model_sphere = Model::CreateFromObj("sphere");
	model_bonefire = Model::CreateFromObj("bonefire");
}
