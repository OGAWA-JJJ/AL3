#pragma once
#include "StageObject.h"

class Stage
{
private:	//オブジェクト(Draw用)
	std::shared_ptr<Object> obj_Stage = nullptr;
	std::shared_ptr<Object> obj_arenaFront = nullptr;

private:
	std::unique_ptr<StageObject> m_stageObject =
		std::make_unique<StageObject>();

public:
	Stage();
	~Stage();
	void Init();
	void Update();
	void AfterBattleUpdate();
	void Draw();
	void AfterBattleDraw();
	void ShadowDraw();
	void LuminanceDraw();
	void ShaftOfLightDraw();
};