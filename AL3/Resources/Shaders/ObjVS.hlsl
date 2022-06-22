#include "ObjShaderHeader.hlsli"

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//ライト
    //float3 lightdir = float3(1, -1, 1);
    //lightdir = normalize(lightdir);
	//ライトの色
    //float3 lightcolor = float3(1, 1, 1);
    float4 wnormal = normalize(mul(world, float4(normal, 0)));
    float4 wpos = mul(world, pos);
    
    VSOutput output; // ピクセルシェーダーに渡す値
    output.svpos = mul(mul(viewproj, world), pos);
    output.worldpos = wpos;
    output.normal = wnormal.xyz;
    output.uv = uv;
    return output;
}

PSOutput VSShadowMain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    float4 wnormal = normalize(mul(world, float4(normal, 0)));
    float4 wpos = mul(world, pos);
    
    PSOutput output;
    output.svpos = mul(mul(viewproj, world), pos);
    //output.worldpos = wpos;
    output.normal = wnormal.xyz;
    output.uv = uv;
    //output.uv = 0.9f;
    
    output.posInLVP = mul(lightViewProj, wpos);
    
    return output;
}