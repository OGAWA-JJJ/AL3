#include "Hantei.h"

Hantei::Hantei()
{
	modelA = Model::CreateFromObj("kine");
	modelB = Model::CreateFromObj("usu");
	objA = Object::Create(modelA);
	objB = Object::Create(modelB);
	light = Light::Create();
	light->SetLightColor({ 1,1,1 });
	Object::SetLight(light);
	Sprite::LoadTexture(0, L"Resources/Textures/mizu.png");
	GH = Sprite::Create(0, XMFLOAT2(0, 0));

	objA->SetPosition(XMFLOAT3(-20, 0, 0));
	objB->SetPosition(XMFLOAT3(20, 0, 0));
}

Hantei::~Hantei()
{
}

void Hantei::Update()
{
	XMFLOAT3 pos = objA->GetPosition();
	XMFLOAT3 pos2 = objB->GetPosition();
	if (Input::isKey(DIK_W)) { pos.z++; }
	if (Input::isKey(DIK_S)) { pos.z--; }
	if (Input::isKey(DIK_A)) { pos.x--; }
	if (Input::isKey(DIK_D)) { pos.x++; }
	objA->SetPosition(pos);

	float diff;
	diff = sqrt(
		(pos.x - pos2.x) * (pos.x - pos2.x) +
		(pos.y - pos2.y) * (pos.y - pos2.y) +
		(pos.z - pos2.z) * (pos.z - pos2.z));

	float r1 = 4.0f;
	float r2 = 3.0f;

	if (diff > r1 + r2) {
		isHit = false;
	}
	else if (diff < r1 + r2) {
		isHit = true;
	}

	objA->Update();
	objB->Update();
}

void Hantei::Draw()
{
	Object::PreDraw(DirectXImportant::cmdList.Get());
	objA->Draw();
	objB->Draw();
	Object::PostDraw();

	Sprite::PreDraw(DirectXImportant::cmdList.Get());
	if (isHit) {
		GH->Draw();
	}
	Sprite::PostDraw();
}
