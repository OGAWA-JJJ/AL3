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

private:	//���@�̃p�^�[��
	enum AnimationType
	{
		STAND, SLOWRUN, RUN, ATTACK
	};

private:	//�萔
	const float MAX_DISTANCE = 125.0f;			//�J�����Ǝ��@�̋���(�����ς�)
	const float MAX_MOVE_SPEED = 2.0f;			//���@�̍ő呬�x
	const float MAX_CAMERA_MOVE_SPEED = 2.0f;	//�J�����̍ő呬�x
	const float EASE_CAMERA_TIMER = 0.006f;		//Target���[�h���؂�ւ�����ۂ̑��x

private:	//�p�C�v���C��
	ObjPipelineSet normal;
	ObjPipelineSet shadow;
	ObjPipelineSet receiveShadow;

	FbxPipelineSet fbx_normal;
	FbxPipelineSet fbx_shadow;

private:	//�ϐ�
	XMFLOAT3 cameraAngle;
	int animationType;
	float cameraMoveEase = 0.0f;
	float cameraY = 0.0f;
	float count = 0.0f;
	bool isTarget = false;
	bool isEase = false;

	int rot = 0;
	bool isHit = false;

private:	//��
	Light* light = nullptr;
	//std::weak_ptr<Light> light;

private:	//���f��(Load�p)
	Model* model_Lich = nullptr;
	Model* model_stage = nullptr;
	Model* model_sponza = nullptr;
	Model* model_eyeball = nullptr;
	Model* model_sword = nullptr;

	Model* model_box = nullptr;
	Model* model_box2 = nullptr;
	//std::weak_ptr<Model> model_Lich;
	//std::weak_ptr<Model> model_stage;
	//std::weak_ptr<Model> model_eyeball;
	//std::weak_ptr<Model> model_sword;

	FbxModel* fbxmodel_standMiku = nullptr;
	FbxModel* fbxmodel_slowRunMiku = nullptr;
	FbxModel* fbxmodel_fastRunMiku = nullptr;
	FbxModel* fbxmodel_oneSwrordAttack = nullptr;
	//std::weak_ptr<FbxModel> fbxmodel_standMiku;
	//std::weak_ptr<FbxModel> fbxmodel_slowRunMiku;
	//std::weak_ptr<FbxModel> fbxmodel_fastRunMiku;

private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_EyeBall = nullptr;
	Object* obj_Stage = nullptr;
	Object* obj_Lich = nullptr;
	Object* obj_Sword = nullptr;
	Object* obj_ShadowSword = nullptr;
	Object* obj_Sponza = nullptr;

	Object* obj_Box[28] = { nullptr };
	Object* obj_SwordBox = nullptr;
	Object* obj_HitBox = nullptr;
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

	FbxObject3D* fbxobj_OneSwordAttack = nullptr;
	FbxObject3D* fbxobj_OneSwordAttackShadow = nullptr;
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
	void LuminanceDraw();	//�P�x���o�p
	void ShadowDraw();		//�e�`��p
};