cbuffer ConstantBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewProjectionMatrix;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(worldPos, viewProjectionMatrix);
    
    output.color = input.color;
    
    return output;
}