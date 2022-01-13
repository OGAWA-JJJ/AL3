#pragma once
#include "../Singleton.h"
#include "../DirectX/DirectXImportant.h"

class GraphicsPipeline :public Singleton<GraphicsPipeline>
{
public:
	friend class Singleton<GraphicsPipeline>;

private:
	ComPtr<ID3D12PipelineState> pipelinestate;

public:
	void CreateGraphicsPipeline(const wchar_t& FileName);
	ComPtr<ID3D12PipelineState> GetPipelineState();
};
