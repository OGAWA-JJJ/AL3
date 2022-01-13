#include "GraphicsPipeline.h"

void GraphicsPipeline::CreateGraphicsPipeline(const wchar_t& FileName)
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob;
	wchar_t filename = FileName;

	//VertexShader
	result = D3DCompileFromFile(
		L"ObjVS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VSmain", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);

	//PixelShader
	result = D3DCompileFromFile(
		L"ObjPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSmain", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
}

ComPtr<ID3D12PipelineState> GraphicsPipeline::GetPipelineState()
{
	return pipelinestate;
}
