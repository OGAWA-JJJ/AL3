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

	static FbxModels* fbxmodel_mikus;

public:	//Enemy
	static FbxModels* fbxmodel_creatures;

	static Model* model_circle;

public:	//Stage
	static Model* model_stage;
	static Model* model_arenaFront;
	static Model* model_arenaBack;

public:
	~ModelManager();
	static void Init();
};

