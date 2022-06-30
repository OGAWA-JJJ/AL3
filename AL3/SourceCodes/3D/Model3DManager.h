#pragma once
#include "Object3D.h"

class Model3DManager
{
public:
	Model3D model3D;

public:
	Model3DManager();
	void Init();
	void CreateModel(const std::string& modelName);
	void DrawModel(bool isDraw = true);
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetScale(DirectX::XMFLOAT3 scale);
};

