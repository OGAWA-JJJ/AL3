#pragma once
#include "../3D/Model.h"
#include "../3D/FbxModels.h"

class ModelManager
{
public:	//Player
	static Model* model_sword;
	static Model* model_box;
	static Model* model_box2;
	static Model* model_helmet;

	static FbxModels* fbxmodel_standMiku;
	static FbxModels* fbxmodel_slowRunMiku;
	static FbxModels* fbxmodel_fastRunMiku;
	static FbxModels* fbxmodel_oneSwrordAttack;
	static FbxModels* fbxmodel_oneSwrordAttack2;
	static FbxModels* fbxmodel_oneSwrordAttack3;
	static FbxModels* fbxmodel_dieMiku;
	static FbxModels* fbxmodel_impactMiku;
	static FbxModels* fbxmodel_rollingMiku;

public:	//Enemy
	static FbxModels* fbxmodel_idleCreature;
	static FbxModels* fbxmodel_runCreature;
	static FbxModels* fbxmodel_kickCreature;
	static FbxModels* fbxmodel_punchCreature;
	static FbxModels* fbxmodel_dieCreature;

	static FbxModels* fbxmodel_RTurnCreature;
	static FbxModels* fbxmodel_LTurnCreature;
	static FbxModels* fbxmodel_RBackCreature;
	static FbxModels* fbxmodel_LBackCreature;
	static FbxModels* fbxmodel_explosionCreature;
	static FbxModels* fbxmodel_riseCreature;
	static FbxModels* fbxmodel_swingDownCreature;

public:	//Stage
	static Model* model_stage;
	static Model* model_sponza;

public:
	~ModelManager();
	static void Init();
};

