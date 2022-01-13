#include "BasicShaderHeader.hlsli"

VSOutput VSmain(float4 pos : POSITION, float scale : TEXCOORD, float4 color : COLOR)
{
    VSOutput output;
    output.pos = pos;
    output.scale = scale;
    output.color = color;
	//output.uv = uv;
	//output.normal = normal;
    return output;
}