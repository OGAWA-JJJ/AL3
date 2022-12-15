#include "Stage.h"
#include "../DirectX/DirectXImportant.h"
#include "PipelineManager.h"
#include "ModelManager.h"

#include "../../imgui/ImguiControl.h"

Stage::Stage()
{
	obj_Stage = Object::Create(ModelManager::model_stage);
	obj_arenaFront = Object::Create(ModelManager::model_arenaFront);
	//obj_arenaFront = Object::Create(ModelManager::model_arenaFront);
	//obj_arenaBack = Object::Create(ModelManager::model_arenaBack);

	const float Stage_Scale = 2000.0f;
	obj_Stage->SetScale(DirectX::XMFLOAT3(Stage_Scale + 200.0f, Stage_Scale, Stage_Scale));

	const float arenaScale = 1000.0f;
	obj_arenaFront->SetScale(
		DirectX::XMFLOAT3(arenaScale, arenaScale, arenaScale));
	//obj_arenaBack->SetScale(
		//DirectX::XMFLOAT3(arenaScale, arenaScale, arenaScale));
}

Stage::~Stage()
{
}

void Stage::Init()
{
	obj_Stage->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	obj_Stage->SetRotation(DirectX::XMFLOAT3(0, 90, 0));
	obj_Stage->SetColor(DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	m_stageObject->Init();
}

void Stage::Update()
{
	if (!ImguiControl::Imgui_isSponzaDraw)
	{
		obj_Stage->Update();
	}
	else
	{
		obj_arenaFront->Update();
		//obj_arenaBack->Update();
	}

	m_stageObject->Update();
}

void Stage::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());

	if (!ImguiControl::Imgui_isSponzaDraw)
	{
		obj_Stage->Draw(PipelineManager::obj_receiveShadow);
	}
	else
	{
		obj_arenaFront->Draw(PipelineManager::obj_receiveShadow);
		//obj_arenaBack->Draw(PipelineManager::obj_receiveShadow);
	}

	m_stageObject->Draw();

	Object::PostDraw();
}

void Stage::ShadowDraw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_arenaFront->Draw(PipelineManager::obj_shadow, false);
	//obj_arenaBack->Draw(PipelineManager::obj_shadow, false);
	Object::PostDraw();
}

void Stage::LuminanceDraw()
{

}

void Stage::ShaftOfLightDraw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_arenaFront->Draw(PipelineManager::obj_normal, false);
	//obj_arenaBack->Draw(PipelineManager::obj_normal, false);
	m_stageObject->Draw();
	Object::PostDraw();
}
