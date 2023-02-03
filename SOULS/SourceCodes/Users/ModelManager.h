#pragma once
#include "../3D/Model.h"
#include "../3D/FbxModels.h"

class ModelManager
{
public:	//Player
	static std::shared_ptr<Model> model_sword;
	static std::shared_ptr<Model> model_box;
	static std::shared_ptr<Model> model_box2;
	static std::shared_ptr<Model> model_helmet;

	static std::shared_ptr<FbxModels> fbxmodel_mikus;

public:	//Enemy
	static std::shared_ptr<FbxModels> fbxmodel_creatures;

public:	//Stage
	static std::shared_ptr<Model> model_stage;
	static std::shared_ptr<Model> model_arenaFront;
	static std::shared_ptr<Model> model_arenaBack;

public:	//StageObject
	static std::shared_ptr<Model> model_sphere;
	static std::shared_ptr<Model> model_bonefire;

public:
	~ModelManager();
	static void Init();
};

