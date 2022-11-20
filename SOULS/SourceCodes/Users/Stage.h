#pragma once
#include "../3D/Object.h"

class Stage
{
private:	//オブジェクト(Draw用)
	Object* obj_Stage = nullptr;
	Object* obj_colosseum = nullptr;

public:
	Stage();
	~Stage();
	void Init(ID3D12Resource* texbuff);
	void Update();
	void Draw();
};

