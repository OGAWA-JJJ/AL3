cbuffer cbuff0 : register(b0)
{
	//float4 color;
    matrix mat; //3Dïœä∑çsóÒ
    matrix matBillBoard;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
	//float3 normal:NORMAL;
	//float2 uv:TEXCOORD;
    float scale : TEXCOORD;
    float4 color : COLOR;
};

struct GSOutput
{
    float4 svpos : SV_POSITION;
    //float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};