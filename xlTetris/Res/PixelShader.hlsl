sampler2D Texture0;
float2 TexSize;                                         // 纹理大小
float ScanPass;                                         // 扫描遍数
static const int MAX_RADIUS = 200;                      // 最大模糊半径
static const int MAX_FLOAT4 = (MAX_RADIUS - 1) / 4 + 1;
float4 Template[MAX_FLOAT4];                            // 高斯模糊系数，传入一个半径方向上的模糊系数

float4 main(float2 texCoord : TEXCOORD0) : COLOR
{
    float4 coord = texCoord.xyxy;
    float2 scale = { 1 / TexSize.x, 1 / TexSize.y };
    float4 color = 0;
    bool run = true;

    for (int i = 0; i < MAX_FLOAT4; ++i)
    {
        float4 t = Template[i];
        if (i == 0)
            t[0] /= 2;
        if (run)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (t[j] <= 0)
                    run = false;

                color += tex2D(Texture0, coord.xy) * t[j];
                color += tex2D(Texture0, coord.zw) * t[j];

                if (ScanPass == 0)
                {
                    coord.x -= scale.x;
                    coord.z += scale.x;
                }
                else
                {
                    coord.y -= scale.y;
                    coord.w += scale.y;
                }
                coord = clamp(coord, float4(0, 0, 0, 0), float4(1, 1, 1, 1));
            }
        }
    }
    return color;
}
