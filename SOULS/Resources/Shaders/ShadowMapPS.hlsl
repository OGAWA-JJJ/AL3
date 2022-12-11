#include "ShadowMapShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    //float shadowMap = input.svpos.z / input.svpos.w;
    //return float4(shadowMap, shadowMap, shadowMap, 1.0f);
    //return input.svpos;
    return tex.Sample(smp, input.uv);

}