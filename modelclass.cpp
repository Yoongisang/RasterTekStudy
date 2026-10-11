#include "modelclass.h"

ModelClass::ModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_Texture = 0;
    m_model = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,char* modelFilename, char* textureFilename)
{
    bool result;

    // 모델 데이터를 읽어 들임.
    result = LoadModel(modelFilename);
    if (!result)
    {
        return false;
    }

    // 정점 인덱스 버퍼를 초기화
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }
    
    // 텍스처 초기화
    result = LoadTexture(
        device,
        deviceContext,
        textureFilename
    );

    if (!result)
    {
        ShutdownBuffers();
        return false;
    }
    
    return true;
}

void ModelClass::Shutdown()
{
    // model texture를 해제
    ReleaseTexture();

    // 정점 인덱스 버퍼를 종료
    ShutdownBuffers();

    // model data를 해제한다.
    ReleaseModel();
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

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
    return m_Texture->GetTexture();
}

bool ModelClass::LoadModel(char* filename)
{
    ifstream fin;
    char input;
    int i;


    // 모델 파일 오픈.
    fin.open(filename);

    // 파일을 열 수 없으면 종료.
    if(fin.fail())
    {
        return false;
    }

    // 정점 수 값이 나올 때까지 읽음.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }

    // 정점 수를 읽어 들임.
    fin >> m_vertexCount;

    // 인덱스 수를 정점 수와 같게 설정.
    m_indexCount = m_vertexCount;

    // 읽어 들인 정점 수만큼 모델 배열을 생성.
    m_model = new ModelType[m_vertexCount];

    // 데이터 시작 지점까지 읽음.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);

    // 정점 데이터를 읽어 들임.
    for(i=0; i<m_vertexCount; i++)
    {
        fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
        fin >> m_model[i].tu >> m_model[i].tv;
        fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
    }

    // 모델 파일을 닫음.
    fin.close();

    return true;
}

void ModelClass::ReleaseModel()
{
    if(m_model)
    {
        delete [] m_model;
        m_model = 0;
    }
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;
    // 정점 배열을 생성
    vertices = new VertexType[m_vertexCount];
    // 인덱스 배열을 생성
    indices = new unsigned long[m_indexCount];
    // 정점 배열과 인덱스 배열에 데이터를 채움.
    for(i=0; i<m_vertexCount; i++)
    {
        vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
        vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

        indices[i] = i;
    }

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

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
    bool result;


    // 텍스처 객체를 생성하고 초기화한다.
    m_Texture = new TextureClass;

    result = m_Texture->Initialize(device, deviceContext,textureFilename);
    if (!result)
    {
        return false;
    }

    return true;
}

void ModelClass::ReleaseTexture()
{
    // 텍스처 객체를 해제.
    if(m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }
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
