#pragma once
#include "../3D/Model.h"
#include "../3D/FbxModel.h"

class ModelManager
{
public:	//Player
	static Model* model_sword;
	static Model* model_box;

	static FbxModel* fbxmodel_standMiku;
	static FbxModel* fbxmodel_slowRunMiku;
	static FbxModel* fbxmodel_fastRunMiku;
	static FbxModel* fbxmodel_oneSwrordAttack;
	static FbxModel* fbxmodel_dieMiku;
	static FbxModel* fbxmodel_impactMiku;
	static FbxModel* fbxmodel_rollingMiku;

public:	//Enemy
	static FbxModel* fbxmodel_idleCreature;
	static FbxModel* fbxmodel_runCreature;
	static FbxModel* fbxmodel_kickCreature;
	static FbxModel* fbxmodel_punchCreature;
	static FbxModel* fbxmodel_dieCreature;

public:	//Stage
	static Model* model_stage;
	static Model* model_sponza;

public:
	~ModelManager();
	static void Init();
};

