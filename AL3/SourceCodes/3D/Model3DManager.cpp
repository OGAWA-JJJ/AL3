#include "Model3DManager.h"

Model3DManager::Model3DManager()
{
}

void Model3DManager::Init()
{
}

void Model3DManager::CreateModel(const std::string& modelName)
{
	model3D.CreateDescriptorHeap();
	model3D.CreateGraphicsPipeline();
	model3D.Init();
	model3D.CreateModel(modelName);
}

void Model3DManager::DrawModel(bool isDraw)
{
	if (!isDraw) {
		return;
	}

	model3D.Update();
	model3D.Draw();
}

void Model3DManager::SetPos(DirectX::XMFLOAT3 pos)
{
	model3D.position = pos;
}

void Model3DManager::SetScale(DirectX::XMFLOAT3 scale)
{
	model3D.scale = scale;
}
