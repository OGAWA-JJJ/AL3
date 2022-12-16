#pragma once
#include <d3d12.h>
#include "../DirectX/d3dx12.h"
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include <array>

class TrailRenderer
{
public:
	struct TrailRendererData
	{
		int splitNum = 1;
		int life = 60;
		DirectX::XMFLOAT3 oldPos_top = { 0,0,0 };
		DirectX::XMFLOAT3 oldPos_bottom = { 0,0,0 };
		DirectX::XMFLOAT3 currentPos_top = { 0,0,0 };
		DirectX::XMFLOAT3 currentPos_bottom = { 0,0,0 };
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
	};

private:
	struct ConstBufferData
	{
		DirectX::XMMATRIX viewProj = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4 color = { 1,1,1,1 };
	};

	struct VertexData
	{
		DirectX::XMFLOAT3 pos = { 0,0,0 };
	};

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootsignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelinestate;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_constBuff;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};

private:
	int m_intervalFrame = 2;
	std::vector<float> m_colors;
	TrailRendererData m_trailRendererData = {};

public:
	TrailRenderer();
	void Init();
	void CreateTrail(int intervalFrame = 1);
	void Update();
	void Draw();

private:
	void CreateGraphicsPipeline();

public:
	void SetTrailRenderer(const TrailRendererData& trailRendererData)
	{
		m_trailRendererData = trailRendererData;
	}
	void SetOldPos(
		const DirectX::XMFLOAT3& pos_top, const DirectX::XMFLOAT3& pos_bottom)
	{
		m_trailRendererData.oldPos_top = pos_top;
		m_trailRendererData.oldPos_bottom = pos_bottom;
	};
	void SetCurrentPos(
		const DirectX::XMFLOAT3& pos_top, const DirectX::XMFLOAT3& pos_bottom)
	{
		m_trailRendererData.currentPos_top = pos_top;
		m_trailRendererData.currentPos_bottom = pos_bottom;
	};

	DirectX::XMFLOAT3& GetOldPos_Top()
	{
		return m_trailRendererData.oldPos_top;
	}
	DirectX::XMFLOAT3& GetOldPos_Bottom()
	{
		return m_trailRendererData.oldPos_bottom;
	}
	DirectX::XMFLOAT3& GetCurrentPos_Top()
	{
		return m_trailRendererData.oldPos_top;
	}
	DirectX::XMFLOAT3& GetCurrentPos_Bottom()
	{
		return m_trailRendererData.oldPos_top;
	}
};
