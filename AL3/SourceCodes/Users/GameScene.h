#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../3D/Model.h"
#include "../3D/Shape.h"
#include "../XAudio2/Music.h"
#include "../3D/FbxObject3D.h"

class GameScene
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private:	//自機のパターン
	enum AnimationType
	{
		STAND, SLOWRUN, RUN
	};

private:	//定数
	const float MAX_DISTANCE = 125.0f;			//カメラと自機の距離(いつか可変に)
	const float MAX_MOVE_SPEED = 2.0f;			//自機の最大速度
	const float MAX_CAMERA_MOVE_SPEED = 2.0f;	//カメラの最大速度
	const float EASE_CAMERA_TIMER = 0.006f;		//Targetモードが切り替わった際の速度

private:	//パイプライン
	ObjPipelineSet normal;
	ObjPipelineSet shadow;
	ObjPipelineSet receiveShadow;

	FbxPipelineSet fbx_normal;
	FbxPipelineSet fbx_shadow;

private:	//変数
	XMFLOAT3 cameraAngle;
	int animationType;
	float cameraMoveEase = 0.0f;
	float cameraY = 0.0f;
	float count = 0.0f;
	bool isTarget = false;
	bool isEase = false;

private:	//光
	Light* light = nullptr;
	//std::weak_ptr<Light> light;

private:	//モデル(Load用)
	Model* model_Lich = nullptr;
	Model* model_stage = nullptr;
	Model* model_sponza = nullptr;
	Model* model_eyeball = nullptr;
	Model* model_sword = nullptr;
	//std::weak_ptr<Model> model_Lich;
	//std::weak_ptr<Model> model_stage;
	//std::weak_ptr<Model> model_eyeball;
	//std::weak_ptr<Model> model_sword;

	FbxModel* fbxmodel_standMiku = nullptr;
	FbxModel* fbxmodel_slowRunMiku = nullptr;
	FbxModel* fbxmodel_fastRunMiku = nullptr;
	//std::weak_ptr<FbxModel> fbxmodel_standMiku;
	//std::weak_ptr<FbxModel> fbxmodel_slowRunMiku;
	//std::weak_ptr<FbxModel> fbxmodel_fastRunMiku;

private:	//オブジェクト(Draw用)
	Object* obj_EyeBall = nullptr;
	Object* obj_Stage = nullptr;
	Object* obj_Lich = nullptr;
	Object* obj_Sword = nullptr;
	Object* obj_ShadowSword = nullptr;
	Object* obj_Sponza = nullptr;
	//std::weak_ptr<Object> obj_EyeBall;
	//std::weak_ptr<Object> obj_Stage;
	//std::weak_ptr<Object> obj_Lich;
	//std::weak_ptr<Object> obj_Sword;
	//std::weak_ptr<Object> obj_ShadowSword;

	FbxObject3D* fbxobj_StandMiku = nullptr;
	FbxObject3D* fbxobj_SlowRunMiku = nullptr;
	FbxObject3D* fbxobj_FastRunMiku = nullptr;
	FbxObject3D* fbxobj_StandShadowMiku = nullptr;
	FbxObject3D* fbxobj_SlowRunShadowMiku = nullptr;
	FbxObject3D* fbxobj_FastRunShadowMiku = nullptr;
	//std::weak_ptr<FbxObject> fbxobj_StandMiku;
	//std::weak_ptr<FbxObject> fbxobj_SlowRunMiku;
	//std::weak_ptr<FbxObject> fbxobj_FastRunMiku;
	//std::weak_ptr<FbxObject> fbxobj_StandShadowMiku;
	//std::weak_ptr<FbxObject> fbxobj_SlowRunShadowMiku;
	//std::weak_ptr<FbxObject> fbxobj_FastRunShadowMiku;

public:
	GameScene();
	~GameScene();
	void Init(ID3D12Resource* texbuff = nullptr);
	void Update();
	void Draw();
	void LuminanceDraw();	//輝度抽出用
	void ShadowDraw();		//影描画用
};