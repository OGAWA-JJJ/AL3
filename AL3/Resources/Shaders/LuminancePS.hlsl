#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //輝度抽出
    //サンプリングしたカラーの明るさを計算
    float t = dot(texcolor.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    
    clip(t - 0.8f);
    return texcolor;
}