#include "PostEffectTest.hlsli"

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    output.svpos = pos;
    output.uv = uv;
    return output;
}


//VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
//{
//    VSOutput output;
//    float x = (float) (pos.x % 2) * 2 - 1.0;
//    float y = (float) (pos.y / 2) * -2 + 1.0;
//    output.svpos = float4(x, y, 0, 1);

//    float tx = (uv.x % 2);
//    float ty = (uv.y / 2);
//    output.uv = float2(tx, ty);

//    return output;
//}