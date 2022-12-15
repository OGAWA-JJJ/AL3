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

float4 GetHighLuminance(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //輝度抽出
    float t = dot(texcolor.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    clip(t - 0.9f);
    return texcolor;
}

float4 PSRadialBlur(VSOutput input) : SV_TARGET
{
    //ブラーの中心位置 ← 現在のテクセル位置
    float2 centerTexel = { 0.5f, 0.01f };
    float2 dir = centerTexel - input.uv;
   
    //距離を計算する
    float len = length(dir);
   
    //方向ベクトルの正規化し、１テクセル分の長さとなる方向ベクトルを計算する
    float2 t = { 1.0f / 1280.0f, 1.0f / 720.0f };
    dir = normalize(dir) * t;
   
    //m_BlurPower でボケ具合を調整する
    //距離を積算することにより、爆発の中心位置に近いほどブラーの影響が小さくなるようにする
    float2 blurPower = { 20.0f, 25.0f };
    dir *= blurPower * len;

    //合成する
    float4 Color[10];
    Color[0] = tex.Sample(smp, input.uv) * 0.19f;
    Color[1] = tex.Sample(smp, input.uv + dir) * 0.17f;
    Color[2] = tex.Sample(smp, input.uv + dir * 2.0f) * 0.15f;
    Color[3] = tex.Sample(smp, input.uv + dir * 3.0f) * 0.13f;
    Color[4] = tex.Sample(smp, input.uv + dir * 4.0f) * 0.11f;
    Color[5] = tex.Sample(smp, input.uv + dir * 5.0f) * 0.09f;
    Color[6] = tex.Sample(smp, input.uv + dir * 6.0f) * 0.07f;
    Color[7] = tex.Sample(smp, input.uv + dir * 7.0f) * 0.05f;
    Color[8] = tex.Sample(smp, input.uv + dir * 8.0f) * 0.03f;
    Color[9] = tex.Sample(smp, input.uv + dir * 9.0f) * 0.01f;
   
    return Color[0] + Color[1] + Color[2] +
    Color[3] + Color[4] + Color[5] + Color[6] +
    Color[7] + Color[8] + Color[9];
}