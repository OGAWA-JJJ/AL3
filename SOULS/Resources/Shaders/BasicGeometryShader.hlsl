#include "BasicShaderHeader.hlsli"

static const uint vnum = 4;

static const float4 offset_array[vnum] =
{
    float4(-0.5f, -0.5f, 0, 0),
     float4(-0.5f, +0.5f, 0, 0),
     float4(+0.5f, -0.5f, 0, 0),
     float4(+0.5f, +0.5f, 0, 0)
};

static float2 uv_array[vnum] =
{
    float2(0, 1),
    float2(0, 0),
    float2(1, 1),
    float2(1, 0)
};

static float4 color_array = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float4 color_array2 = float4(1.0f, 0.0f, 0.0f, 1.0f);
static float4 color_array3 = float4(0.0f, 1.0f, 0.0f, 1.0f);
static float4 color_array4 = float4(0.0f, 0.0f, 1.0f, 1.0f);

[maxvertexcount(vnum)]
void GSmain(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element;
    for (uint i = 0; i < vnum; i++)
    {
        float4 offset = offset_array[i] * input[0].scale;
        offset = mul(matBillBoard, offset);
        element.svpos = input[0].pos + offset;
        element.svpos = mul(mat, element.svpos);
        //element.normal = input[i].normal;
        element.uv = uv_array[i];
        element.color = input[0].color;
        //if (i == 0)
        //    element.color = color_array;
        //else if (i == 1)
        //    element.color = color_array2;
        //else if (i == 2)
        //    element.color = color_array3;
        //else
        //    element.color = color_array4;
        output.Append(element);
    }
}