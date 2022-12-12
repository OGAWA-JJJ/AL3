#pragma once
#include "../3D/Object.h"

class Stage
{
private:	//オブジェクト(Draw用)
	std::shared_ptr<Object> obj_Stage = nullptr;
	std::shared_ptr<Object> obj_arenaFront = nullptr;
	std::shared_ptr<Object> obj_arenaBack = nullptr;

	std::shared_ptr<Object> obj_shadowArenaFront = nullptr;
	std::shared_ptr<Object> obj_shadowArenaBack = nullptr;

public:
	Stage();
	~Stage();
	void Init();
	void Update();
	void Draw();
	void ShadowDraw();
};