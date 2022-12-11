#pragma once
#include "../3D/Object.h"

class Stage
{
private:	//オブジェクト(Draw用)
	Object* obj_Stage = nullptr;
	Object* obj_arenaFront = nullptr;
	Object* obj_arenaBack = nullptr;

	Object* obj_shadowArenaFront = nullptr;
	Object* obj_shadowArenaBack = nullptr;

public:
	Stage();
	~Stage();
	void Init(Microsoft::WRL::ComPtr<ID3D12Resource> texbuff);
	void Update();
	void Draw();
	void ShadowDraw();
};