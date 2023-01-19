#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float4> shadow : register(t1);
SamplerState smp : register(s0);
SamplerState shadowSmp : register(s1);

//エントリーポイント
float4 PSmain(VSOutput input) : SV_TARGET
{
    //return float4(1, 0, 0, 1);
    
    //テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //シェーディングによる色
    float4 shadecolor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //光沢度
    const float shininess = 2.0f;
    //頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.svpos.xyz);
    //ライトに向かうベクトルと法線の内積
    float3 dotlightnormal = dot(lightv, input.normal);
    //反射光ベクトル
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
    //環境反射光
    float3 ambient = m_ambient;
    //拡散反射光
    float3 diffuse = dotlightnormal * m_diffuse * 3.14f;
    //鏡面反射光
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	
    //加算
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    
    //シェーディングによる色で描画
    return shadecolor * texcolor;
}

float4 PSShadowMain(PSOutput input) : SV_TARGET
{
    float w = 1.0f / input.posInLVP.w;
    
    //z参照
    float2 shadowTex;
    shadowTex.x = (1.0f + input.posInLVP.x * w) * 0.5f;
    shadowTex.y = (1.0f - input.posInLVP.y * w) * 0.5f;
    float z = shadow.Sample(shadowSmp, shadowTex).x;
    
    float andColor = 1.0f;
    float4 texcolor = tex.Sample(smp, input.uv);
    if (input.posInLVP.z * w > z + 0.005f)
    {
        andColor = 0.5f;
    }
    
    //シェーディングによる色
    float4 shadecolor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //光沢度
    const float shininess = 2.0f;
    //頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.svpos.xyz);
    //ライトに向かうベクトルと法線の内積
    float3 dotlightnormal = dot(lightv, input.normal);
    //反射光ベクトル
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
    //環境反射光
    float3 ambient = m_ambient;
    //拡散反射光
    float3 diffuse = dotlightnormal * m_diffuse * 3.14f;
    //鏡面反射光
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
    
    float3 pos[4];
    pos[0] = float3(0, 0, 0);
    pos[1] = float3(0, 0, -500);
    pos[2] = float3(500, 0, 0);
    pos[3] = float3(-500, 0, 0);
    const float lightatten = 0.8f;
    for (int i = 0; i < 4; i++)
    {
        float3 lightv = pos[i] - input.svpos.xyz;
        float d = length(lightv);
        lightv = normalize(lightv);
        float atten = 1.0f / (lightatten + lightatten * d + lightatten * d * d);
        dotlightnormal = dot(lightv, input.normal);
        reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
        diffuse = dotlightnormal * m_diffuse;
        specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
        shadecolor.rgb += atten * (diffuse + specular) * float3(0.9f, 0.6f, 0.0f);
    }
	
    //加算
    //shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    
    return texcolor * shadecolor *
    float4(andColor, andColor, andColor, 1);
}

float4 PSBlack(PSOutput input) : SV_TARGET
{
    return float4(0, 0, 0, 1);
}