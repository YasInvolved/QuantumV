cbuffer CameraBuffer : register(b0)
{
    float4x4 projMatrix;
    float4x4 viewMatrix;
};

cbuffer ModelBuffer : register(b1)
{
    float4x4 modelMatrix;
}

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPos = mul(float4(input.position, 1.0f), modelMatrix);
    output.position = mul(worldPos, mul(viewMatrix, projMatrix));
    output.normal = input.normal;
    output.color = input.color;
    
    return output;
}