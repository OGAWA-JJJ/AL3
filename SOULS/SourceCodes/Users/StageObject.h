#pragma once
#include "../3D/Object.h"

class StageObject
{
private:	//�I�u�W�F�N�g(Draw�p)
	std::shared_ptr<Object> obj_sphere = nullptr;

public:
	StageObject();
	~StageObject();
	void Init();
	void Update();
	void Draw();
	void ShadowDraw();
	void LuminanceDraw();
};