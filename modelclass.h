#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "textureclass.h"

using namespace DirectX;

class ModelClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };
public:
    ModelClass();
    ModelClass(const ModelClass& other);
    ~ModelClass();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();

private:
    bool InitializeBuffers(ID3D11Device* device);
    bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
    void ReleaseTexture();
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);
    
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    TextureClass* m_Texture;
    int m_vertexCount, m_indexCount;
};
