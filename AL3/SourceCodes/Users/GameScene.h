#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../3D/Model.h"
#include "../XAudio2/Music.h"
//#include "../3D/FbxObject3D.h"
#include "../3D/FbxObjects.h"

class GameScene
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	//MT4
	float accY = 0.0f;
	float dist = 0.0f;
	float posY = 0.0f;
	float velY = 0.0f;

	Sprite* GH1 = nullptr;

	Light* light = nullptr;

	Model* obj_model = nullptr;
	Object* obj_Sword = nullptr;

	FbxModels* l_model0 = nullptr;
	FbxObjects* l_obj0 = nullptr;

	FbxModels* l_model1 = nullptr;
	FbxObjects* l_obj1 = nullptr;

	FbxObjects::FbxPipelineSet l_normal = {};
	Object::ObjPipelineSet l_aaa = {};

	bool isFastRun = true;
	float rate = 0.0f;

public:
	GameScene();
	~GameScene();
	void Init(ID3D12Resource* texbuff = nullptr);
	void Update();
	void Draw();
	void LuminanceDraw();	//ãPìxíäèoóp
	void ShadowDraw();		//âeï`âÊóp
};