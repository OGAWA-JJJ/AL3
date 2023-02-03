cbuffer cbuff0 : register(b0)
{
    matrix viewproj;
};

cbuffer cbuff0 : register(b1)
{
    float4 weights[2];
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 tex0 : TEXCOORD1;
    float4 tex1 : TEXCOORD2;
    float4 tex2 : TEXCOORD3;
    float4 tex3 : TEXCOORD4;
    float4 tex4 : TEXCOORD5;
    float4 tex5 : TEXCOORD6;
    float4 tex6 : TEXCOORD7;
    float4 tex7 : TEXCOORD8;
};