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
		int life = 10;
		float curvePower = 0.0f;
		DirectX::XMFLOAT3 oldPos_top = { 0,0,0 };
		DirectX::XMFLOAT3 oldPos_bottom = { 0,0,0 };
		DirectX::XMFLOAT3 currentPos_top = { 0,0,0 };
		DirectX::XMFLOAT3 currentPos_bottom = { 0,0,0 };
		DirectX::XMFLOAT4 color = { 1,1,1,0.8f };
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
	const int C_CHECK_LIFE = 10;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertBuff;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_constBuff;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};

private:
	TrailRendererData m_trailRendererData = {};
	bool m_isCreate = false;
	int m_life = 0;
	int m_intervalFrame = 2;
	float m_subAlpha = 0.0f;
	DirectX::XMFLOAT4 m_color = { 1,1,1,1 };

public:
	TrailRenderer();
	void Init();
	void CreateTrail(int intervalFrame = 1);
	void Update();
	void Draw(
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSigunature,
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState);

private:
	void CheckLife();
	void CreateBuff();

public:
	void SetTrailRendererData(const TrailRendererData& trailRendererData)
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
		return m_trailRendererData.currentPos_top;
	}
	DirectX::XMFLOAT3& GetCurrentPos_Bottom()
	{
		return m_trailRendererData.currentPos_bottom;
	}
};

class TrailManager
{
private:
	const int C_INIT_MAX_TRAIL = 200;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootsignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelinestate;

private:
	std::vector<TrailRenderer> m_trailRenderers;
	int m_maxTrail = C_INIT_MAX_TRAIL;

public:
	TrailManager();
	void Init();
	void Update();
	void Draw();
	void CreateTrail(const int num) { m_trailRenderers[num].CreateTrail(); }

private:
	void CreateGraphicsPipeline(); //å©ÇΩéûÇÃç\ê¨Ç≥ÇÍï˚Ç≈âfÇÈÇ©ÇÃîªíË(ÉJÉäÉìÉO)

public:
	void SetMaxTrail(const int num);

	void SetTrailRenderer(const int num, const TrailRenderer::TrailRendererData& trailRendererData)
	{
		m_trailRenderers[num].SetTrailRendererData(trailRendererData);
	}
	void SetOldPos(
		const int num, const DirectX::XMFLOAT3& pos_top, const DirectX::XMFLOAT3& pos_bottom)
	{
		m_trailRenderers[num].SetOldPos(pos_top, pos_bottom);
	};
	void SetCurrentPos(
		const int num, const DirectX::XMFLOAT3& pos_top, const DirectX::XMFLOAT3& pos_bottom)
	{
		m_trailRenderers[num].SetCurrentPos(pos_top, pos_bottom);
	};

	DirectX::XMFLOAT3& GetOldPos_Top(const int num)
	{
		return m_trailRenderers[num].GetOldPos_Top();
	}
	DirectX::XMFLOAT3& GetOldPos_Bottom(const int num)
	{
		return m_trailRenderers[num].GetOldPos_Bottom();
	}
	DirectX::XMFLOAT3& GetCurrentPos_Top(const int num)
	{
		return m_trailRenderers[num].GetCurrentPos_Top();
	}
	DirectX::XMFLOAT3& GetCurrentPos_Bottom(const int num)
	{
		return m_trailRenderers[num].GetCurrentPos_Bottom();
	}
};
