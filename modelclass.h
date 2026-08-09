#pragma once

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class ModelClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };
public:
    ModelClass();
    ModelClass(const ModelClass& other);
    ~ModelClass();

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount();

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);
    
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
};
