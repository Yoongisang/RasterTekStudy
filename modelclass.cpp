#include "modelclass.h"

ModelClass::ModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device)
{
    bool result;
    
    // 정점 인덱스 버퍼를 초기화
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }

    return true;
}

void ModelClass::Shutdown()
{
    // 정점 인덱스 버퍼를 종료
    ShutdownBuffers();

    return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    // 그리기를 준비하기 위해 정점 인덱스 버퍼를 그래픽 파이프라인에 올림
    RenderBuffers(deviceContext);

    return;
}

int ModelClass::GetIndexCount()
{
    return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // 정점 배열의 정점 개수를 설정
    m_vertexCount = 3;
    // 인덱스 배열의 인덱스 개수를 설정
    m_indexCount = 3;
    // 정점 배열을 생성
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }
    // 인덱스 배열을 생성
    indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }

    // 정점 배열에 데이터를 채움
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // 왼쪽 아래
    vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // 위쪽 가운데
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // 오른쪽 아래
    vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    // 인덱스 배열에 데이터를 채움
    indices[0] = 0;  // 왼쪽 아래
    indices[1] = 1;  // 위쪽 가운데
    indices[2] = 2;  // 오른쪽 아래

    // 정적 정점 버퍼의 description을 설정합니다.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // 서브리소스 구조체에 정점 데이터의 포인터를 줍니다.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 정점 버퍼를 생성합니다.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // 정적 인덱스 버퍼의 description을 설정합니다.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 서브리소스 구조체에 인덱스 데이터의 포인터를 줍니다.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼를 생성합니다.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // 정적 정점 버퍼의 description을 설정 
    delete[] vertices;
    vertices = 0;
    delete[] indices;
    indices = 0;

    return true;
}

void ModelClass::ShutdownBuffers()
{
    // 인덱스 버퍼를 해제
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // 정점 버퍼를 해제
    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;
    
    // 정점 버퍼의 stride와 offset을 설정
    stride = sizeof(VertexType);
    offset = 0;
    // 렌더링되도록 입력 어셈블러에 정점 버퍼를 활성화
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    // 렌더링되도록 입력 어셈블러에 인덱스 버퍼를 활성화
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    // 이 정점 버퍼로 그릴 프리미티브 종류를 설정(삼각형)
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}
