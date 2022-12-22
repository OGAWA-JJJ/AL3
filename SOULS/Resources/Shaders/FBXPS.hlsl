#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float4> shadow : register(t1);
SamplerState smp : register(s0);
SamplerState shadowSmp : register(s1);

//�G���g���[�|�C���g
float4 PSmain(VSOutput input) : SV_TARGET
{
    //return float4(1, 0, 0, 1);
    
    //�e�N�X�`���}�b�s���O
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //�V�F�[�f�B���O�ɂ��F
    float4 shadecolor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //����x
    const float shininess = 2.0f;
    //���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos - input.svpos.xyz);
    //���C�g�Ɍ������x�N�g���Ɩ@���̓���
    float3 dotlightnormal = dot(lightv, input.normal);
    //���ˌ��x�N�g��
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
    //�����ˌ�
    float3 ambient = m_ambient;
    //�g�U���ˌ�
    float3 diffuse = dotlightnormal * m_diffuse * 3.14f;
    //���ʔ��ˌ�
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	
    //���Z
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    
    //�V�F�[�f�B���O�ɂ��F�ŕ`��
    return shadecolor * texcolor;
}

float4 PSShadowMain(PSOutput input) : SV_TARGET
{
    float w = 1.0f / input.posInLVP.w;
    
    //z�Q��
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
    
    //�V�F�[�f�B���O�ɂ��F
    float4 shadecolor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //����x
    const float shininess = 2.0f;
    //���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos - input.svpos.xyz);
    //���C�g�Ɍ������x�N�g���Ɩ@���̓���
    float3 dotlightnormal = dot(lightv, input.normal);
    //���ˌ��x�N�g��
    float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
    //�����ˌ�
    float3 ambient = m_ambient;
    //�g�U���ˌ�
    float3 diffuse = dotlightnormal * m_diffuse * 3.14f;
    //���ʔ��ˌ�
    float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	
    //���Z
    shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
    
    return texcolor * shadecolor *
    float4(andColor, andColor, andColor, 1);
}

float4 PSBlack(PSOutput input) : SV_TARGET
{
    return float4(0, 0, 0, 1);
    
    //float shadow = (input.posInLVP.z / 1000.0f);
    //return float4(shadow, shadow, shadow, 1.0f);
}