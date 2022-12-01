#pragma once
#include "FbxModels.h"
#include "Light.h"

class FbxObjects
{
public:
	struct FbxInitData
	{
		const char* m_vsEntryPoint = "VSmain";
		const char* m_psEntryPoint = "PSmain";
	};

	struct FbxPipelineSet
	{
		ID3D12RootSignature* rootsignature;
		ID3D12PipelineState* pipelinestate;
	};

private:
	static const int MAX_BONES = 32;

	static ID3D12Device* device;
	static ID3D12GraphicsCommandList* cmdList;
	static FbxPipelineSet pipelineSet;
	static Light* light;

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
	struct Node {
		const char* name = {};
		Node* parent = {};
		DirectX::XMFLOAT3 scale = {};
		DirectX::XMFLOAT4 rotate = {};
		DirectX::XMFLOAT3 translate = {};
		DirectX::XMMATRIX local_transform = {};
		DirectX::XMMATRIX world_transform = {};

		std::vector<Node*>	children;
	};

private:
	std::vector<Node> nodes;
	bool animation_loop_flag = true;
	int current_animation_index = 0;
	int m_animationCount = 0;
	float current_animation_seconds = 0.0f;

public:
	static FbxObjects* Create(FbxModels* model = nullptr);

public:
	static void StaticInit(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList);
	static FbxPipelineSet CreateGraphicsPipeline(const FbxInitData& fbxInitdata);
	static void SetLight(Light* light) { FbxObjects::light = light; }

private:
	FbxModels* model = nullptr;
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMMATRIX matWorld = DirectX::XMMatrixIdentity();

	Microsoft::WRL::ComPtr<ID3D12Resource> constBufferDataB0;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffSkin;

	bool m_isPlay = false;
	bool m_isAnimationEndTrigger = false;

	bool m_isBlend = false;
	bool m_isAddTimerEase = false;
	float m_addSpeed = 0.0f;
	float m_rate = 1.0f;

	ID3D12DescriptorHeap* fbxDescHeap = {};
	//ボーンの名前と行列(Update後に更新)
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> affineTrans;
	//手のワールド行列
	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
	//ボーン全部の回転行列
	std::vector<DirectX::XMMATRIX> matRots;

private:
	DirectX::XMFLOAT3 ambient = { 0.8f, 0.8f, 0.8f };
	DirectX::XMFLOAT3 diffuse = { 0.8f, 0.8f, 0.8f };
	DirectX::XMFLOAT3 specular = { 0.8f, 0.8f, 0.8f };
	float alpha = 1.0f;
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
	bool Init();
	void Update(bool isShadowCamera = false);
	void Draw(const FbxPipelineSet& pipelineSet);
	void PlayAnimation(); //RePlayAnimationと同じ

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
	void SetModel(FbxModels* model) { this->model = model; }

	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	void SetRotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	void BlendAnimation(FbxObjects* startObject, float rate, bool isBlend);

	void StopAnimation() { m_isPlay = false; }
	void ResetAnimation() { current_animation_seconds = 0; }
	void ReplayAnimation() { m_isPlay = true; }
	void SetLoopAnimation(bool isLoop) { animation_loop_flag = isLoop; }
	void SetAnimationTimerMax();	//あんま意味ないかも
	void SetAnimationSpeed(float addSpeed, bool isSet);	//0~1
	void MultiAnimationSpeed(float rate) { m_rate = rate; }

public:	//Getter
	const DirectX::XMFLOAT3& GetScale() { return scale; }
	const DirectX::XMFLOAT3& GetRotation() { return rotation; }
	const DirectX::XMFLOAT3& GetPosition() { return position; }

	const float GetAddTime(int animationIndex = 0) { return model->GetAnimations()[animationIndex].add_time; }
	const float GetEndTime(int animationIndex = 0) { return model->GetAnimations()[animationIndex].seconds_length; }
	const float GetNowTime(int animationIndex = 0) { return current_animation_seconds; }

	const std::vector<std::pair<std::string, DirectX::XMMATRIX>>
		& GetAffineTrans() { return affineTrans; }	//スケール行列が入っている為、OBB描画に問題←治した
	const std::vector<DirectX::XMMATRIX>& GetMatRots() { return matRots; }
	DirectX::XMMATRIX& GetMatrix() { return matrix; }	//手固定

	const bool IsAnimationEnd() { return m_isAnimationEndTrigger; }

private:
	//仮
	std::vector<int> drawSkip;
public:
	void SetDrawSkipNum(int num) { drawSkip.push_back(num); }
	void ResetDrawSkipNum() { drawSkip.clear(); }
};
