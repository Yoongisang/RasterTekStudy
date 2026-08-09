cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    // 올바른 행렬 계사늘 위해 위치 벡터를 4성분으로 변경
    input.position.w = 1.0f;
    // 월드, 뷰, 투영 행렬에 대해 정점의 위치를 계산
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    // 픽셀 셰이더가 사용할 입력 색상을 저장
    output.color = input.color;
    
    return output;
};
