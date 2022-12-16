#include "Trail.hlsli"

float4 VSmain(float4 pos : POSITION) : SV_POSITION
{
    float4 viewpos = mul(viewproj, pos);
    return viewpos;
}