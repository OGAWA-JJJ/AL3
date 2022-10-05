#include "Stage.h"
#include "PipelineManager.h"
#include "../DirectX/DirectXImportant.h"
#include "ModelManager.h"

Stage::Stage()
{
	obj_Stage = Object::Create(ModelManager::model_stage);
	obj_colosseum = Object::Create(ModelManager::model_sponza);

	const float Stage_Scale = 200.0f;
	obj_Stage->SetScale(DirectX::XMFLOAT3(Stage_Scale + 200.0f, Stage_Scale, Stage_Scale));

	const float colosseumScale = 1.0f;
	obj_colosseum->SetScale(DirectX::XMFLOAT3(colosseumScale, colosseumScale, colosseumScale));
}

Stage::~Stage()
{
	delete obj_Stage;
}

void Stage::Init(ID3D12Resource* texbuff)
{
	obj_Stage->AddTexture(texbuff, ModelManager::model_stage->GetDescHeap());

	obj_Stage->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	obj_Stage->SetRotation(DirectX::XMFLOAT3(0, 90, 0));
	obj_Stage->SetColor(DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	obj_colosseum->SetRotation(DirectX::XMFLOAT3(0.0f, 90.0f, 0.0f));
}

void Stage::Update()
{
	obj_Stage->Update();

	obj_colosseum->Update();
}

void Stage::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_Stage->Draw(PipelineManager::obj_receiveShadow);
	//obj_colosseum->Draw(PipelineManager::obj_normal);
	Object::PostDraw();
}
