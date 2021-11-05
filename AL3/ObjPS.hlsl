#include "ObjShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1)); //右下奥向きのライト
    float light_diffuse = saturate(dot(-light, input.normal)); //diffuseを[0,1]の範囲にClamp
    float3 shade_color = m_ambient;
    shade_color += m_diffuse * light_diffuse;
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    //return float4(0, 0, 0, 1);
    return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);
}