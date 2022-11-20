#include "PostEffectTest.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
    float4 texcolor = tex.Sample(smp, input.uv);
    return texcolor;
    
    //輝度抽出
    //サンプリングしたカラーの明るさを計算
    //float t = dot(texcolor.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    
    //clip(t - 0.9f);
    //return texcolor;
    
    //平均ブラー
    //2.5テクセルずらすためのUV値を求める
    //float offsetU = 1.5f / 1280.0f;
    //float offsetV = 1.5f / 720.0f;
    
    ////基準テクセルから右のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(offsetU, 0.0f));
    ////基準テクセルから左のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(-offsetU, 0.0f));
    ////基準テクセルから下のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(0.0f, offsetV));
    ////基準テクセルから上のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(0.0f, -offsetV));
    ////基準テクセルから右下のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(offsetU, offsetV));
    ////基準テクセルから右上のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(offsetU, -offsetV));
    ////基準テクセルから左下のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(-offsetU, offsetV));
    ////基準テクセルから左上のテクセルのカラーをサンプリング
    //texcolor += tex.Sample(smp, input.uv + float2(-offsetU, -offsetV));
    
    ////基準テクセルと近傍8テクセルの平均を算出
    //texcolor /= 9.0f;
    
    //return texcolor;
    
    //通常
    //return float4(texcolor.rgb, 1);
    //return float4(texcolor.r, texcolor.g, 0,          1);
    //return float4(texcolor.r, 0,          texcolor.b, 1);
    //return float4(0,          texcolor.g, texcolor.b, 1);
    
    //モザイク
    //float blockSize = 16;
    //float uvX = input.uv.x * 1280;
    //float uvY = input.uv.y * 720;
    
    //uvX /= blockSize;
    //uvY /= blockSize;
    
    //uvX = floor(uvX) * blockSize;
    //uvY = floor(uvY) * blockSize;
    
    //uvX /= 1280;
    //uvY /= 720;
    
    //float2 uv = { uvX, uvY };
  
    //return tex.Sample(smp, uv);
    
    //セピア調
    //float sepiaRatio = (texcolor.r + texcolor.g + texcolor.b) / 765.0f;
    //return float4(
    //240.0f * sepiaRatio,
    //200.0f * sepiaRatio,
    //145.0f * sepiaRatio,
    //1
    //);
    
    //グレースケール
    //float grayAverage = (texcolor.r + texcolor.g + texcolor.b) / 3.0f;
    //return float4(
    //grayAverage,
    //grayAverage,
    //grayAverage,
    //1
    //);
}

float4 PSXmain(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //平均ブラー
    const float texel = 5.0f;
    //1.5テクセルずらすためのUV値を求める
    float offsetU = texel / 1280.0f;
    float offsetV = texel / 720.0f;
    
    //基準テクセルから右のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, 0.0f));
    //基準テクセルから左のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, 0.0f));
    //基準テクセルから下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(0.0f, offsetV));
    //基準テクセルから上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(0.0f, -offsetV));
    //基準テクセルから右下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, offsetV));
    //基準テクセルから右上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, -offsetV));
    //基準テクセルから左下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, offsetV));
    //基準テクセルから左上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, -offsetV));
    
    //基準テクセルと近傍8テクセルの平均を算出
    texcolor /= 9.0f;
    
    return texcolor;
    //return float4(1, 1, 1, 1);
}

float4 PSYmain(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //平均ブラー
    const float texel = 5.0f;
    //1.5テクセルずらすためのUV値を求める
    float offsetU = texel / 1280.0f;
    float offsetV = texel / 720.0f;
    
    //基準テクセルから右のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, 0.0f));
    //基準テクセルから左のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, 0.0f));
    //基準テクセルから下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(0.0f, offsetV));
    //基準テクセルから上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(0.0f, -offsetV));
    //基準テクセルから右下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, offsetV));
    //基準テクセルから右上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(offsetU, -offsetV));
    //基準テクセルから左下のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, offsetV));
    //基準テクセルから左上のテクセルのカラーをサンプリング
    texcolor += tex.Sample(smp, input.uv + float2(-offsetU, -offsetV));
    
    //基準テクセルと近傍8テクセルの平均を算出
    texcolor /= 9.0f;
    
    return texcolor;
    //return float4(1, 1, 1, 1);
}

float4 PSBlur(VSOutput input) : SV_TARGET
{
    float4 Color;
    
    Color = weights[0].x * tex.Sample(smp, input.tex0.xy);

    Color += weights[0].y * tex.Sample(smp, input.tex1.xy);
    Color += weights[0].z * tex.Sample(smp, input.tex2.xy);
    Color += weights[0].w * tex.Sample(smp, input.tex3.xy);
    Color += weights[1].x * tex.Sample(smp, input.tex4.xy);
    Color += weights[1].y * tex.Sample(smp, input.tex5.xy);
    Color += weights[1].z * tex.Sample(smp, input.tex6.xy);
    Color += weights[1].w * tex.Sample(smp, input.tex7.xy);
    
    Color += weights[0].x * tex.Sample(smp, input.tex0.zw);
    Color += weights[0].y * tex.Sample(smp, input.tex1.zw);
    Color += weights[0].z * tex.Sample(smp, input.tex2.zw);
    Color += weights[0].w * tex.Sample(smp, input.tex3.zw);
    Color += weights[1].x * tex.Sample(smp, input.tex4.zw);
    Color += weights[1].y * tex.Sample(smp, input.tex5.zw);
    Color += weights[1].z * tex.Sample(smp, input.tex6.zw);
    Color += weights[1].w * tex.Sample(smp, input.tex7.zw);
    
    //return float4(1, 1, 0, 1);
    return float4(Color.xyz, 1.0f);
}