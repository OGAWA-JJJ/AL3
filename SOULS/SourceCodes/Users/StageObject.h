#pragma once
#include "../3D/Object.h"

class StageObject
{
private:	//オブジェクト(Draw用)
	std::shared_ptr<Object> obj_sphere = nullptr;
	std::shared_ptr<Object> obj_bonefire = nullptr;

public:
	StageObject();
	~StageObject();
	void Init();
	void Update();
	void AfterBattleUpdate();
	void Draw();
	void AfterBattleDraw();
	void ShadowDraw();
	void LuminanceDraw();
};