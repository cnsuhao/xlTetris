sampler2D Texture0;
float2 TexSize;                     // �����С
int ScanPass;                       // ɨ�����
static const int MAX_RADIUS = 50;   // ���ģ���뾶
float Template[MAX_RADIUS];         // ��˹ģ��ϵ��������һ���뾶�����ϵ�ģ��ϵ��

float4 main(float2 texCoord : TEXCOORD0) : COLOR
{
    float4 color = tex2D(Texture0, texCoord) * Template[0];
    float2 coord = 0;
    bool meetZero = false;
    for (int i = 1; i < MAX_RADIUS; ++i)
    {
        if (Template[i] != 0 && !meetZero)
        {
            coord = texCoord;
            if (ScanPass == 0)
            {
                coord.x = texCoord.x - i / TexSize.x;
                color += tex2D(Texture0, coord) * Template[i];
                coord.x = texCoord.x + i / TexSize.x;
                color += tex2D(Texture0, coord) * Template[i];
            }
            else
            {
                coord.y = texCoord.y - i / TexSize.y;
                color += tex2D(Texture0, coord) * Template[i];
                coord.y = texCoord.y + i / TexSize.y;
                color += tex2D(Texture0, coord) * Template[i];
            }
        }
    }
    return color;
}
