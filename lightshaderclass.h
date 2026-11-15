#pragma once

// HLSL의 NUM_LIGHTS와 같은 값
const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class LightShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    // PS b0 : LightColorBuffer
    struct LightColorBufferType
    {
        XMFLOAT4 diffuseColor[NUM_LIGHTS];
    };

    // VS b1 : LightPositionBuffer (XMFLOAT4라서 16바이트 정렬이 자동으로 맞음)
    struct LightPositionBufferType
    {
        XMFLOAT4 lightPosition[NUM_LIGHTS];
    };

public:
    LightShaderClass();
    LightShaderClass(const LightShaderClass& other);
    ~LightShaderClass();

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColor[], XMFLOAT4 lightPosition[]);

private:
    bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColor[], XMFLOAT4 lightPosition[]);
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11Buffer* m_lightColorBuffer;
    ID3D11Buffer* m_lightPositionBuffer;
    ID3D11SamplerState* m_sampleState;
};
