#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include "textureclass.h"

using namespace DirectX;
using namespace std;

class ModelClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };
    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };
public:
    ModelClass();
    ModelClass(const ModelClass& other);
    ~ModelClass();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFimename, char* textureFilename);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();

    bool LoadModel(char* filename);
    void ReleaseModel();
private:
    bool InitializeBuffers(ID3D11Device* device);
    bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename);
    void ReleaseTexture();
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);
    
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    TextureClass* m_Texture;
    int m_vertexCount, m_indexCount;
    ModelType* m_model;
};
