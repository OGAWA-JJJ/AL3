#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    //float4 color = (1.0f,1.0f,1.0f,0.5f);
    return tex.Sample(smp, input.uv) * color;
}

float4 PSRadialMain(VSOutput input) : SV_TARGET
{
    //ブラーの中心位置 ← 現在のテクセル位置
    float2 centerTexel = { 0.1f, 0.1f };
    float2 dir = centerTexel - input.uv;
   
    //距離を計算する
    float len = length(dir);
   
    //方向ベクトルの正規化し、１テクセル分の長さとなる方向ベクトルを計算する
    float2 t = { 1.0f / 1280.0f, 1.0f / 720.0f };
    dir = normalize(dir) * t;
   
    //m_BlurPower でボケ具合を調整する
    //距離を積算することにより、爆発の中心位置に近いほどブラーの影響が小さくなるようにする
    float2 blurPower = { 30.0f, 30.0f };
    dir *= blurPower * len;

    //合成する
    float4 Color[10];
    Color[0] = tex.Sample(smp, input.uv) * 0.19f;
    Color[1] = tex.Sample(smp, input.uv + dir) * 0.17f;
    Color[2] = tex.Sample(smp, input.uv + dir * 2.0f) * 0.15f;
    Color[3] = tex.Sample(smp, input.uv + dir * 3.0f) * 0.13f;
    Color[4] = tex.Sample(smp, input.uv + dir * 4.0f) * 0.11f;
    Color[5] = tex.Sample(smp, input.uv + dir * 5.0f) * 0.09f;
    Color[6] = tex.Sample(smp, input.uv + dir * 6.0f) * 0.07f;
    Color[7] = tex.Sample(smp, input.uv + dir * 7.0f) * 0.05f;
    Color[8] = tex.Sample(smp, input.uv + dir * 8.0f) * 0.03f;
    Color[9] = tex.Sample(smp, input.uv + dir * 9.0f) * 0.01f;
   
    return Color[0] + Color[1] + Color[2] + Color[3] + Color[4] + Color[5] + Color[6] + Color[7] + Color[8] + Color[9];
}