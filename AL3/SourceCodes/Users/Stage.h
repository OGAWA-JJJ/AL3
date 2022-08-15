#pragma once
#include "../3D/Object.h"

class Stage
{
private:	//���f��(Load�p)
	Model* model_stage = nullptr;
	Model* model_sponza = nullptr;

private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_Stage = nullptr;

public:
	Stage();
	~Stage();
	void Init(ID3D12Resource* texbuff);
	void Update();
	void Draw();
};

