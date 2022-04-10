#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //輝度抽出
    //サンプリングしたカラーの明るさを計算
    float t = dot(texcolor.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    
    clip(t - 0.9f);
    return texcolor;
    
    //平均ブラー
    //2.5テクセルずらすためのUV値を求める
    float offsetU = 1.5f / 1280.0f;
    float offsetV = 1.5f / 720.0f;
    
    //基準テクセルから右のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, 0.0f));
    //基準テクセルから左のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, 0.0f));
    //基準テクセルから下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(0.0f, offsetV));
    //基準テクセルから上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(0.0f, -offsetV));
    //基準テクセルから右下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, offsetV));
    //基準テクセルから右上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, -offsetV));
    //基準テクセルから左下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, offsetV));
    //基準テクセルから左上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, -offsetV));
    
    //基準テクセルと近傍8テクセルの平均を算出
    texcolor /= 9.0f;
    
    return texcolor;
}