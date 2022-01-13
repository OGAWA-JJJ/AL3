#include "BasicShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 PSmain(GSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(0, 0.0, 0)); //右下奥向きのライト
    //float diffuse = saturate(dot(-light, input.normal)); //diffuseを[0,1]の範囲にClamp
    //float brightness = diffuse + 0.7f; //光源へのベクトルと法線ベクトルの内積
    float4 texcolor = float4(tex.Sample(smp, input.uv));
	//return float4(texcolor.rgb * brightness,texcolor.a) * color; //輝度をRGBに代入して出力
    //return float4(1.0f, 1.0f, 1.0f, 1);
    return tex.Sample(smp, input.uv) * input.color;
}