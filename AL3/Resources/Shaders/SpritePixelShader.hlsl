#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
	//float4 color = (1.0f,1.0f,1.0f,1.0f);
	return tex.Sample(smp,input.uv) * color;
}