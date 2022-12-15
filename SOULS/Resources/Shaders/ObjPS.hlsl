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
    float w = 1.0f / input.posInLVP.w;
    
    //z参照
    float2 shadowTex;
    shadowTex.x = (1.0f + input.posInLVP.x * w) * 0.5f;
    shadowTex.y = (1.0f - input.posInLVP.y * w) * 0.5f;
    float z = shadow.Sample(smp, shadowTex).x;
    
    float andColor = 1.0f;
    float4 texcolor = tex.Sample(smp, input.uv);
    if (input.posInLVP.z * w > z + 0.005f)
    {
        andColor = 0.5f;
    }
    
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
    
    return texcolor * color * shadecolor *
    float4(andColor, andColor, andColor, 1);
}

float4 PSBlack(PSOutput input) : SV_TARGET
{
    //return tex.Sample(smp, input.uv);
    
    float shadow = (input.posInLVP.z / 1000.0f);
    return float4(shadow, shadow, shadow, 1.0f);
}

float4 PSTexture(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * color;
}