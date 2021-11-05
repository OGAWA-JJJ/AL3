#pragma once
#include "Model3D.h"

class Model3DManager
{
public:
	Model3D model3D;

public:
	Model3DManager();
	void Init();
	void CreateModel(const string& modelName);
	void DrawModel(bool isDraw = true);
	void SetPos(XMFLOAT3 pos);
	void SetScale(XMFLOAT3 scale);
};

