#include "Stage.h"
#include "PipelineManager.h"
#include "../DirectX/DirectXImportant.h"
#include "ModelManager.h"
#include "../../imgui/ImguiControl.h"

Stage::Stage()
{
	obj_Stage = Object::Create(ModelManager::model_stage);
	obj_arenaFront = Object::Create(ModelManager::model_arenaFront);
	obj_arenaBack = Object::Create(ModelManager::model_arenaBack);

	obj_shadowArenaFront = Object::Create(ModelManager::model_arenaFront);
	obj_shadowArenaBack = Object::Create(ModelManager::model_stage);

	const float Stage_Scale = 2000.0f;
	obj_Stage->SetScale(DirectX::XMFLOAT3(Stage_Scale + 200.0f, Stage_Scale, Stage_Scale));

	const float arenaScale = 30.0f;
	obj_arenaFront->SetScale(
		DirectX::XMFLOAT3(arenaScale, arenaScale, arenaScale));
	obj_arenaBack->SetScale(
		DirectX::XMFLOAT3(arenaScale, arenaScale, arenaScale));

	obj_shadowArenaFront->SetScale(
		DirectX::XMFLOAT3(arenaScale, arenaScale, arenaScale));
	obj_shadowArenaBack->SetScale(
		DirectX::XMFLOAT3(arenaScale, arenaScale, arenaScale));
}

Stage::~Stage()
{
	delete obj_Stage;
	delete obj_arenaFront;
	delete obj_arenaBack;

	delete obj_shadowArenaFront;
	delete obj_shadowArenaBack;
}

void Stage::Init(Microsoft::WRL::ComPtr<ID3D12Resource> texbuff)
{
	//obj_Stage->AddTexture(texbuff, ModelManager::model_stage->GetDescHeap());
	obj_Stage->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	obj_Stage->SetRotation(DirectX::XMFLOAT3(0, 90, 0));
	obj_Stage->SetColor(DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	//obj_arenaFront->AddTexture(texbuff, ModelManager::model_arenaFront->GetDescHeap());
	//obj_arenaBack->AddTexture(texbuff, ModelManager::model_arenaBack->GetDescHeap());
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
		obj_arenaBack->Update();

		obj_shadowArenaFront->Update(true);
		obj_shadowArenaBack->Update(true);
	}
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
		//obj_arenaFront->Draw(PipelineManager::obj_normal);
		//obj_arenaBack->Draw(PipelineManager::obj_normal);
	}

	Object::PostDraw();
}

void Stage::ShadowDraw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	//obj_shadowArenaFront->Draw(PipelineManager::obj_shadow);
	obj_shadowArenaBack->Draw(PipelineManager::obj_shadow);
	Object::PostDraw();
}
