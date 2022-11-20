#pragma once
#include "../Singleton.h"
#include <wrl.h>
#include <d3d12.h>
//#include "../DirectX/DirectXImportant.h"

class GraphicsPipeline :public Singleton<GraphicsPipeline>
{
public:
	friend class Singleton<GraphicsPipeline>;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelinestate;

public:
	void CreateGraphicsPipeline(const wchar_t& FileName);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
};
