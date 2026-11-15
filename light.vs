// 광원 개수. PS, C++과 반드시 같은 값이어야 함
#define NUM_LIGHTS 4

cbuffer MatrixBuffer   // VS b0
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// 점광원 4개의 월드 위치 (VS b1)
cbuffer LightPositionBuffer
{
    float4 lightPosition[NUM_LIGHTS];
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex      : TEXCOORD0;
    float3 normal   : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
    float3 normal   : NORMAL;
    // 배열은 TEXCOORD1~TEXCOORD4를 차례로 사용
    float3 lightPos[NUM_LIGHTS] : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;
    int i;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    // 정점의 월드 위치
    worldPosition = mul(input.position, worldMatrix);

    // 정점 -> 광원 방향 벡터를 광원마다 계산
    for(i = 0; i < NUM_LIGHTS; i++)
    {
        output.lightPos[i] = lightPosition[i].xyz - worldPosition.xyz;
        output.lightPos[i] = normalize(output.lightPos[i]);
    }

    return output;
}