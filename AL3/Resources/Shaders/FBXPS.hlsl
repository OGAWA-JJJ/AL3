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
    float4 texcolor = { 0.5, 0.5, 1, 1 };
    //float4 texcolor = tex.Sample(smp, input.uv);
    
    //�V�F�[�f�B���O�ɂ��F
    float4 shadecolor;
    //����x
    const float shininess = 4.0f;
    //���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos - input.svpos.xyz);
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
    
    //return float4(1, 0, 0, 1);
    
    //�V�F�[�f�B���O�ɂ��F�ŕ`��
    //return texcolor;
    return shadecolor * texcolor;
}

float4 PSBlack() : SV_TARGET
{
    return float4(0.5f, 0.5f, 0.5f, 1.0f);
}