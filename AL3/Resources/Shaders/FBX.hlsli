static const int MAX_BONES = 32;

cbuffer cbuff0 : register(b0)
{
    matrix viewproj;
    matrix world;
    float3 cameraPos;
}

cbuffer cbuff1 : register(b1)
{
    float3 m_ambient : packoffset(c0);
    float3 m_diffuse : packoffset(c1);
    float3 m_specular : packoffset(c2);
    float4 m_color : packoffset(c3);
}

cbuffer cbuff2 : register(b2)
{
    float3 lightv;
    float3 lightcolor;
    matrix lightViewProj;
}

cbuffer skinning : register(b3)
{
    matrix matSkinning[MAX_BONES];
}

struct VSInput
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PSOutput
{
    float4 svpos : SV_POSITION; 
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 posInLVP : TEXCOORD1;
};