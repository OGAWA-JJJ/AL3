#include "ShadowMapShaderHeader.hlsli"

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    output.svpos = pos;
    output.uv = uv;
    return output;
}