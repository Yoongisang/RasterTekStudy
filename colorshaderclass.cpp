#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;
    // 정점 셰이더 파일 이름을 설정
    error = wcscpy_s(vsFilename, 128, L"color.vs");
    if (error != 0)
    {
        return false;
    }
    // 픽셀 셰이더 파일 이름을 설정
    error = wcscpy_s(psFilename, 128, L"color.ps");
    if (error != 0)
    {
        return false;
    }
    // 정점 셰이더와 픽셀 셰이더를 초기화
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    return result;
}

void ColorShaderClass::Shutdown()
{
    ShutdownShader();

    return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix)
{
    bool result;
    // 렌더링에 사용할 셰이더 매개변수를 설정
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
    if (!result)
    {
        return false;
    }
    // 셰이더로 준비된 버퍼를 렌더링
    RenderShader(deviceContext, indexCount);

    return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;


    // 이 함수에서 사용할 포인터들을 null로 초기화
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    // 정점 셰이더 코드를 컴파일
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader","vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일에 실패하면 오류 메시지에 무언가 기롣되어 있어야 함
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // 오류 메시지가 비어있다면 셰이더 파일 자체를 찾지 못한 것
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }
        return false;
    }

    // 픽셀 셰이더 코드를 컴파일
    result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일에 실패하면 오류 메시지에 기옥
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // 오류 메시지가 비어있다면 파일 자체를 찾지 못한 것
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }
    
        return false;
    }
    // 버퍼로부터 정점 셰이더를 생성
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if(FAILED(result))
    {
        return false;
    }

    // 버퍼로부터 픽셀 셰이더를 생성
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if(FAILED(result))
    {
        return false;
    }
    // 정점 입력 레이아웃 description을 생성
    // 이 설정은 ModelClass의 VertexType 구조체 및 셰이더의 것과 일치해야 함
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;
    // 레이아웃의 요소 개수를 구합니다.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // 정점 입력 레이아웃을 생성합니다.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
    {
        return false;
    }

    // 더 이상 필요 없으므로 정점 셰이더 버퍼와 픽셀 셰이더 버퍼를 해제합니다.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;
    // 정점 셰이더에 있는 동적 행렬 상수 버퍼의 description을 설정
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 이 클래스 내에서 정점 셰이더 상수 버퍼에 접근할 수 있도록 상수 버퍼 포인터를 생성
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result))
    {
        return false;
    }

    return true;
}

void ColorShaderClass::ShutdownShader()
{
    // 행렬 상수 버퍼를 해제합니다.
    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // 레이아웃을 해제합니다.
    if(m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // 픽셀 셰이더를 해제합니다.
    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // 정점 셰이더를 해제합니다.
    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;
    // 오류 메시지 텍스트 버퍼의 포인터를 얻음
    compileErrors = (char*)(errorMessage->GetBufferPointer());
    // 메시의 길이를 얻음
    bufferSize = errorMessage->GetBufferSize();
    // 오류 메시지를 기록할 파일을 염
    fout.open("shader-error.txt");
    // 오류 메시지를 기록합니다.
    for(i=0; i<bufferSize; i++)
    {
        fout << compileErrors[i];
    }
    // 파일을 닫습니다.
    fout.close();
    // 오류 메시지를 해제합니다.
    errorMessage->Release();
    errorMessage = 0;
    // 컴파일 오류를 텍스트 파일에서 확인하라는 메시지를 띄웁니다.
    MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;
    // 셰이더에 보내기 위해 행렬을 전치
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);
    // 상수 버퍼에 쓸 수 있도록 잠급니다.
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }
    // 상수 버퍼 내 데이터의 포인터를 얻음
    dataPtr = (MatrixBufferType*)mappedResource.pData;
    // 행렬을 상수 버퍼에 복사
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;
    // 상수 버퍼 잠금을 해제
    deviceContext->Unmap(m_matrixBuffer, 0);
    // 정점 셰이더에서 상수 버퍼의 위치를 설정합니다.
    bufferNumber = 0;
    // 마지막으로 갱신된 값으로 정점 셰이더의 상수 버퍼를 설정합니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // 정점 입력 레이아웃을 설정합니다.
    deviceContext->IASetInputLayout(m_layout);
    // 이 삼각형을 렌더링하는 데 사용할 정점·픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);
    // 삼각형을 렌더링합니다.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
