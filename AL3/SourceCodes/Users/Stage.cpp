#include "Stage.h"
#include "PipelineManager.h"
#include "../DirectX/DirectXImportant.h"

Stage::Stage()
{
	model_stage = Model::CreateFromObj("yuka");
	model_sponza = Model::CreateFromObj("triangle");

	obj_Stage = Object::Create(model_stage);

	const float Stage_Scale = 100.0f;
	obj_Stage->SetScale(DirectX::XMFLOAT3(Stage_Scale + 100.0f, Stage_Scale, Stage_Scale));
}

Stage::~Stage()
{
	delete model_stage;
	delete obj_Stage;
}

void Stage::Init(ID3D12Resource* texbuff)
{
	obj_Stage->AddTexture(texbuff, model_stage->GetDescHeap());

	obj_Stage->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	obj_Stage->SetRotation(DirectX::XMFLOAT3(0, 90, 0));
}

void Stage::Update()
{
	obj_Stage->Update();
}

void Stage::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	obj_Stage->Draw(PipelineManager::GetObjReceiveShadowPipeline());
	Object::PostDraw();
}
