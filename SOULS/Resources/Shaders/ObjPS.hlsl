#include "ObjShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> shadow : register(t1); //1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 PSmain(VSOutput input) : SV_TARGET
{
    //�e�N�X�`���}�b�s���O
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //�V�F�[�f�B���O�ɂ��F
    float4 shadecolor;
    //����x
    const float shininess = 4.0f;
    //���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
    //���C�g�Ɍ������x�N�g���Ɩ@���̓���
    float3 dotlightnormal = dot(lightv, input.normal);
    //���ˌ��x�N�g��
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
    //�����ˌ�
    float3 ambient = { 1, 1, 1 }; /*m_ambient*/
    //�g�U���ˌ�
    float3 diffuse = dotlightnormal * m_diffuse * 3.14f;
    //���ʔ��ˌ�
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	
    //���Z
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    shadecolor.a = m_alpha;
    
    //�V�F�[�f�B���O�ɂ��F�ŕ`��
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
    
    //�͈͓�
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