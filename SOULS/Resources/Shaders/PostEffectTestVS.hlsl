#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    output.svpos = pos;
    output.uv = uv;
    
    output.tex0 = float4(0, 0, 0, 0);
    output.tex1 = float4(0, 0, 0, 0);
    output.tex2 = float4(0, 0, 0, 0);
    output.tex3 = float4(0, 0, 0, 0);
    output.tex4 = float4(0, 0, 0, 0);
    output.tex5 = float4(0, 0, 0, 0);
    output.tex6 = float4(0, 0, 0, 0);
    output.tex7 = float4(0, 0, 0, 0);
    
    return output;
}

VSOutput VSXmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    //output.svpos = mul(viewproj, pos);
    output.svpos = pos;
    
    float2 texSize;
    float level;
    tex.GetDimensions(0, texSize.x, texSize.y, level);
    
    float2 tex = uv;
    
    output.tex0.xy = float2(1.0f / texSize.x, 0.0f);
    output.tex1.xy = float2(3.0f / texSize.x, 0.0f);
    output.tex2.xy = float2(5.0f / texSize.x, 0.0f);
    output.tex3.xy = float2(7.0f / texSize.x, 0.0f);
    output.tex4.xy = float2(9.0f / texSize.x, 0.0f);
    output.tex5.xy = float2(11.0f / texSize.x, 0.0f);
    output.tex6.xy = float2(13.0f / texSize.x, 0.0f);
    output.tex7.xy = float2(15.0f / texSize.x, 0.0f);
    
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;
    
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);
    
    return output;
}

VSOutput VSYmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    //output.svpos = mul(viewproj, pos);
    output.svpos = pos;
    
    float2 texSize;
    float level;
    tex.GetDimensions(0, texSize.x, texSize.y, level);
    
    float2 tex = uv;
    
    output.tex0.xy = float2(0.0f, 1.0f / texSize.y);
    output.tex1.xy = float2(0.0f, 3.0f / texSize.y);
    output.tex2.xy = float2(0.0f, 5.0f / texSize.y);
    output.tex3.xy = float2(0.0f, 7.0f / texSize.y);
    output.tex4.xy = float2(0.0f, 9.0f / texSize.y);
    output.tex5.xy = float2(0.0f, 11.0f / texSize.y);
    output.tex6.xy = float2(0.0f, 13.0f / texSize.y);
    output.tex7.xy = float2(0.0f, 15.0f / texSize.y);
    
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;
    
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);
    
    return output;
}