#include "FBX.hlsli"

//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex : register(t0);
//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
SamplerState smp : register(s0);

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

float4 PSBlack() : SV_TARGET
{
    //return float4(input.svpos.z, input.svpos.z, input.svpos.z, 1.0f);
    return float4(0.5f, 0.5f, 0.5f, 1.0f);
}