#pragma once
#include "../3D/Object.h"

class Stage
{
private:	//モデル(Load用)
	Model* model_stage = nullptr;
	Model* model_sponza = nullptr;

private:	//オブジェクト(Draw用)
	Object* obj_Stage = nullptr;

public:
	Stage();
	~Stage();
	void Init(ID3D12Resource* texbuff);
	void Update();
	void Draw();
};

