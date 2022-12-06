#include "FBX.hlsli"

//0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t0);
//0番スロットに設定されたサンプラー
SamplerState smp : register(s0);

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

float4 PSBlack() : SV_TARGET
{
    //return float4(input.svpos.z, input.svpos.z, input.svpos.z, 1.0f);
    return float4(0.5f, 0.5f, 0.5f, 1.0f);
}