#pragma once
#include "../2D/Sprite.h"
#include "../3D/Object.h"
#include "../XAudio2/Music.h"
#include "../3D/FbxObject3D.h"

class Player
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

public:	//�萔
	const float MAX_DISTANCE = 125.0f;			//�J�����Ǝ��@�̋���(�����ς�)

private:
	const float MAX_MOVE_SPEED = 2.0f;			//���@�̍ő呬�x
	const float MAX_CAMERA_MOVE_SPEED = 2.0f;	//�J�����̍ő呬�x
	const float EASE_CAMERA_TIMER = 0.006f;		//Target���[�h���؂�ւ�����ۂ̑��x

private:	//�i�[�p
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> bones;
	std::vector<DirectX::XMMATRIX> matRot;

private:	//�ϐ�
	XMFLOAT3 pos;
	XMFLOAT3 cameraAngle;
	int animationType;
	float cameraMoveEase = 0.0f;
	float cameraY = 0.0f;
	float count = 0.0f;
	bool isTarget = false;
	bool isEase = false;

	bool isHit = false;

private:	//���f��(Load�p)
	Model* model_sword = nullptr;
	//std::weak_ptr<Model> model_sword;
	Model* model_box = nullptr;
	Model* model_box2 = nullptr;

	FbxModel* fbxmodel_standMiku = nullptr;
	FbxModel* fbxmodel_slowRunMiku = nullptr;
	FbxModel* fbxmodel_fastRunMiku = nullptr;
	FbxModel* fbxmodel_oneSwrordAttack = nullptr;
	//std::weak_ptr<FbxModel> fbxmodel_standMiku;
	//std::weak_ptr<FbxModel> fbxmodel_slowRunMiku;
	//std::weak_ptr<FbxModel> fbxmodel_fastRunMiku;

private:	//�I�u�W�F�N�g(Draw�p)
	Object* obj_Sword = nullptr;
	Object* obj_ShadowSword = nullptr;
	//std::weak_ptr<Object> obj_Sword;
	//std::weak_ptr<Object> obj_ShadowSword;
	Object* obj_Box[28] = { nullptr };
	Object* obj_SwordBox = nullptr;
	Object* obj_HitBox = nullptr;

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
	Player();
	~Player();

	void Init();
	void Update(DirectX::XMFLOAT3 enemyPos);
	void Draw();
	void LuminanceDraw();
	void ShadowDraw();

private:
	void Input();
	void Setter();
	void Collision();
	void OtherUpdate();

public:
	const DirectX::XMFLOAT3& GetPos() { return pos; }
};
