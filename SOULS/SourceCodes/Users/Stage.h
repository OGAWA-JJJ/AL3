#pragma once
#include "../3D/Object.h"

class Stage
{
private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_Stage = nullptr;
	Object* obj_colosseum = nullptr;

public:
	Stage();
	~Stage();
	void Init(ID3D12Resource* texbuff);
	void Update();
	void Draw();
};

