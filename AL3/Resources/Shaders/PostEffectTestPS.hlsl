#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //return float4(texcolor.rgb, 1);
    //return float4(texcolor.r, texcolor.g, 0,          1);
    //return float4(texcolor.r, 0,          texcolor.b, 1);
    //return float4(0,          texcolor.g, texcolor.b, 1);
    
    //セピア調
    float sepiaRatio = (texcolor.r + texcolor.g + texcolor.b) / 765.0f;
    return float4(
    240.0f * sepiaRatio,
    200.0f * sepiaRatio,
    145.0f * sepiaRatio,
    1
    );
    
    //グレースケール
    //float grayAverage = (texcolor.r + texcolor.g + texcolor.b) / 3.0f;
    //return float4(
    //grayAverage,
    //grayAverage,
    //grayAverage,
    //1
    //);
}