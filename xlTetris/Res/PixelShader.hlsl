sampler2D Texture0;
float2 TexSize; // 纹理大小
float Radius;   // 模糊半径
int ScanPass;   // 扫描遍数

float4 main(float2 texCoord : TEXCOORD0) : COLOR
{
    const int R = 11;
    float coef[R] =
    {
        0.01, 0.03, 0.06, 0.1, 0.15, 0.3, 0.15, 0.1, 0.06, 0.03, 0.01
    };
    float4 color = 0;
    for (int i = 0; i < R; ++i)
    {
        float2 coord = texCoord;
        if (ScanPass == 0)
            coord.x += (i - R / 2) / TexSize.x;
        else
            coord.y += (i - R / 2) / TexSize.y;
        color += tex2D(Texture0, coord) * coef[i];
    }
    return color;
}
