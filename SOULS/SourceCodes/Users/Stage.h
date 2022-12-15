#pragma once
#include "StageObject.h"

class Stage
{
private:	//�I�u�W�F�N�g(Draw�p)
	std::shared_ptr<Object> obj_Stage = nullptr;
	std::shared_ptr<Object> obj_arenaFront = nullptr;
	std::shared_ptr<Object> obj_arenaBack = nullptr;

private:
	std::unique_ptr<StageObject> m_stageObject =
		std::make_unique<StageObject>();

public:
	Stage();
	~Stage();
	void Init();
	void Update();
	void Draw();
	void ShadowDraw();
	void LuminanceDraw();
	void ShaftOfLightDraw();
};