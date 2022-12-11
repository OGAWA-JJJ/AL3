#include "ObjShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> shadow : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) : SV_TARGET
{
    //テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //シェーディングによる色
    float4 shadecolor;
    //光沢度
    const float shininess = 4.0f;
    //頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
    //ライトに向かうベクトルと法線の内積
    float3 dotlightnormal = dot(lightv, input.normal);
    //反射光ベクトル
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
    //環境反射光
    float3 ambient = { 1, 1, 1 }; /*m_ambient*/
    //拡散反射光
    float3 diffuse = dotlightnormal * m_diffuse * 3.14f;
    //鏡面反射光
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	
    //加算
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    shadecolor.a = m_alpha;
    
    //シェーディングによる色で描画
    return shadecolor * texcolor * color;
}

float4 PSShadowMain(PSOutput input) : SV_TARGET
{
    //float4x4 light = lightViewProj;
    float4 texcolor = tex.Sample(smp, input.uv);
    
    float2 shadowMapUV = input.posInLVP.xy / input.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;
    
    //float zInLVP = input.posInLVP.z / input.posInLVP.w;
    
    float4 shadowMap = 1.0f;
    
    //範囲内
    if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
        && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {
        //float zInShadowMap = shadow.Sample(smp, shadowMapUV).r;
        //if (zInLVP > zInShadowMap)
        //{
        //    shadowMap = shadow.Sample(smp, shadowMapUV);
        //}
        
        shadowMap = shadow.Sample(smp, shadowMapUV);
        //texcolor.xyz *= shadowMap.xyz;
    }
    
    texcolor.xyz *= shadowMap.xyz;
    
    return texcolor * color;
}

float4 PSBlack(PSOutput input) : SV_TARGET
{
    //return float4(input.posInLVP.z, 0.0f, 0.0f, input.posInLVP.w);
    
    float shadow = (input.posInLVP.z / 500.0f) ;
    return float4(shadow, shadow, shadow, 1.0f);
    
    //return float4(0.5f, 0.5f, 0.5f, 1.0f);
}

float4 PSTexture(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * color;
}