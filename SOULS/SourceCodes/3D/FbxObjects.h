#pragma once
#include "FbxModels.h"
#include "Light.h"

class FbxObjects
{
public:
	struct FbxInitData
	{
		std::string m_vsEntryPoint = "VSmain";
		std::string m_psEntryPoint = "PSmain";
	};

	struct FbxPipelineSet
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate;
	};

private:
	static const int MAX_BONES = 36;

	static Microsoft::WRL::ComPtr<ID3D12Device> device;
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	static FbxPipelineSet pipelineSet;
	static std::shared_ptr<Light> light;

private:
	struct ConstBufferDataB0
	{
		DirectX::XMMATRIX viewproj;
		DirectX::XMMATRIX world;
		DirectX::XMFLOAT3 cameraPos;
		float pad;
	};

	struct ConstBufferDataSkin
	{
		DirectX::XMMATRIX bones[MAX_BONES];
	};

private:
	struct Node
	{
		const char* name = {};
		Node* parent = {};
		DirectX::XMFLOAT3 scale = {};
		DirectX::XMFLOAT4 rotate = {};
		DirectX::XMFLOAT3 translate = {};
		DirectX::XMMATRIX local_transform = {};
		DirectX::XMMATRIX world_transform = {};

		std::vector<Node*>	children;
	};

	struct AnimationDatas
	{
		float m_addSpeed = 0.0f;
		float m_currentAnimationSeconds = 0.0f;
		bool m_isPlay = false;
		bool m_isAnimationEndTrigger = false;
		bool m_animationLoopFlag = true;
	};

private:
	std::vector<Node> nodes;
	std::vector<AnimationDatas> animationDatas;

	int m_currentAnimationIndex = 0;
	bool m_isBlend = false;
	bool m_isAddTimerEase = false;

public:
	static std::shared_ptr<FbxObjects> Create(std::shared_ptr<FbxModels> model = nullptr);
	void SetAnimationIndex(int animationIndex) { m_currentAnimationIndex = animationIndex; }

public:
	static void StaticInit(
		Microsoft::WRL::ComPtr<ID3D12Device> dev,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
	static FbxPipelineSet CreateGraphicsPipeline(const FbxInitData& fbxInitdata);
	static void SetLight(std::shared_ptr<Light> light) { FbxObjects::light = light; }

private:
	std::shared_ptr<FbxModels> model = nullptr;
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();

	Microsoft::WRL::ComPtr<ID3D12Resource> constBufferDataB0;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffSkin;

	//ボーンの名前と行列(Update後に更新)
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> affineTrans;
	//手のワールド行列
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	//ボーン全部の回転行列
	std::vector<DirectX::XMMATRIX> matRots;
	//スケール抜きワールド行列
	DirectX::XMMATRIX m_matTransRotWorld = DirectX::XMMatrixIdentity();

private:
	DirectX::XMFLOAT3 ambient = { 0.8f, 0.8f, 0.8f };
	DirectX::XMFLOAT3 diffuse = { 0.8f, 0.8f, 0.8f };
	DirectX::XMFLOAT3 specular = { 0.8f, 0.8f, 0.8f };
	float alpha = 1.0f;
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
	bool Init();
	void Update();
	void Draw(const FbxPipelineSet& pipelineSet, bool isShadow = true);

private:
	void UpdateAnimation();
	void UpdateTransform();

	void CalcNode()
	{
		const std::vector<FbxModels::Node>& l_nodes = model->GetNodes();
		nodes.resize(l_nodes.size());
		for (int node_index = 0; node_index < nodes.size(); ++node_index)
		{
			auto&& src = l_nodes.at(node_index);
			auto&& dst = nodes.at(node_index);

			dst.name = src.name.c_str();
			dst.parent = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
			dst.scale = src.scaling;
			dst.rotate = src.rotation;
			dst.translate = src.translation;

			if (dst.parent != nullptr)
			{
				dst.parent->children.emplace_back(&dst);
			}
		}
	}

public:	//Setter
	void SetModel(std::shared_ptr<FbxModels> model)
	{
		this->model = model;
		animationDatas.resize(model->GetAnimations().size());
	}

	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	void SetRotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	void BlendAnimation(int startIndex, int endIndex, float rate, bool isBlend);

	void CurrentPlayAnimation() { animationDatas[m_currentAnimationIndex].m_isPlay = true; };
	void PlayAnimation(int animationIndex) { animationDatas[animationIndex].m_isPlay = true; };

	void CurrentStopAnimation() { animationDatas[m_currentAnimationIndex].m_isPlay = false; }
	void StopAnimation(int animationIndex) { animationDatas[animationIndex].m_isPlay = false; }

	void CurrentResetAnimation() { animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds = 0.0f; }
	void ResetAnimation(int animationIndex) { animationDatas[animationIndex].m_currentAnimationSeconds = 0.0f; }

	void SetCurrentLoopAnimation(bool isLoop) { animationDatas[m_currentAnimationIndex].m_animationLoopFlag = isLoop; }
	void SetLoopAnimation(int animationIndex, bool isLoop) { animationDatas[animationIndex].m_animationLoopFlag = isLoop; }

	void SetCurrentAnimationSpeed(float addSpeed, bool isSet);				//0~1
	void SetAnimationSpeed(int animationIndex, float addSpeed, bool isSet);	//0~1

public:	//Getter
	const DirectX::XMFLOAT3& GetScale() { return scale; }
	const DirectX::XMFLOAT3& GetRotation() { return rotation; }
	const DirectX::XMFLOAT3& GetPosition() { return position; }

	const float GetCurrentAddTime() { return model->GetAnimations()[m_currentAnimationIndex].add_time; }
	const float GetCurrentEndTime() { return model->GetAnimations()[m_currentAnimationIndex].seconds_length; }
	const float GetCurrentNowTime() { return animationDatas[m_currentAnimationIndex].m_currentAnimationSeconds; }
	const float GetAddTime(int animationIndex) { return model->GetAnimations()[animationIndex].add_time; }
	const float GetEndTime(int animationIndex) { return model->GetAnimations()[animationIndex].seconds_length; }
	const float GetNowTime(int animationIndex) { return animationDatas[animationIndex].m_currentAnimationSeconds; }

	const std::vector<std::pair<std::string, DirectX::XMMATRIX>>
		& GetAffineTrans() { return affineTrans; }		//スケール行列が入っている為、OBB描画に問題←治した
	const std::vector<DirectX::XMMATRIX>& GetMatRots() { return matRots; }
	DirectX::XMMATRIX& GetMatrix() { return matrix; }	//手固定
	DirectX::XMMATRIX& GetMatWorld();					//スケール抜き

	const bool IsCurrentAnimationEnd() { return animationDatas[m_currentAnimationIndex].m_isAnimationEndTrigger; }
	const bool IsAnimationEnd(int animationIndex) { return animationDatas[animationIndex].m_isAnimationEndTrigger; }

private:
	//仮
	std::vector<int> drawSkip;
public:
	void SetDrawSkipNum(int num) { drawSkip.push_back(num); }
	void ResetDrawSkipNum() { drawSkip.clear(); }
};
