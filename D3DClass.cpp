#include "D3DClass.h"

D3DClass::D3DClass()
{
    // 멤버변수 초기화
    m_swapChain = 0;
    m_device = 0;
    m_deviceContext = 0;
    m_renderTargetView = 0;
    m_depthStencilBuffer = 0;
    m_depthStencilState = 0;
    m_depthStencilView = 0;
    m_rasterState = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator;
    unsigned long long stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    float fieldOfView, screenAspect;
    // vsync 설정을 저장 
    m_vsync_enabled = vsync;
    // DirectX 그래픽 인터페이스 팩토리를 생성
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
    {
        return false;
    }
    // 팩토리를 사용해 주 그래픽 인터페이스용 어댑터 생성
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        return false;
    }
    // 주 어댑터 출력을 열거
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        return false;
    }
    // 어댑터 출력에서 DXGI_FORMAT_R8G8B8A8_UNORM 디스플레이 포맷에 맞는 모드 개수를 얻음
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        return false;
    }
    // 이 모니터/비디오 카드 조합의 가능한 모든 디스플레이 모드를 담을 리스트
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        return false;
    }
    // 디스플레이 모드 리스트 구조체를 채움
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
    {
        return false;
    }
    // 모든 디스플레이 모드를 순회하며 화면 너비/높이와 일치하는 모드를 찾음
    // 일치하는 모드를 찾으면 해당 모니터의 주사율 분자/분모를 저장
    for (i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }
    // 어댑터 설명을 얻음
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }
    // 전용 비디오 카드 메모리를 메가바이트 단위로 저장
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
    // 비디오 카드 이름을 문자 배열로 변환해 저장
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0)
    {
        return false;
    }
    // 디스플레이 모드 리스트 해제
    delete[] displayModeList;
    displayModeList = 0;
    // 어댑터 출력 해제
    adapterOutput->Release();
    adapterOutput = 0;
    // 어댑터 해제
    adapter->Release();
    adapter = 0;
    // 팩토리 해제
    factory->Release();
    factory = 0;
    // 스왑 체인 description을 초기화
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    // 단일 후면 버퍼로 설정
    swapChainDesc.BufferCount = 1;
    // 후면 버퍼의 너비와 높이 설정
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    // 후면 버퍼를 일반 32비트 서피스로 설정
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // 후면 버퍼의 주사율을 설정
    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }
    // 후면 버퍼의 용도를 설정
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    //렌더링할 윈도우의 핸들 설정
    swapChainDesc.OutputWindow = hwnd;
    // 멀티샘플링 off
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    // 전체 화면 또는 창 모드로 설정
    if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }
    // 스캔라인 순서와 스케일링을 unspecified로 설정
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // 표시 후 후면 버퍼 내용을 버림
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // 고급 플래그는 설정 X
    swapChainDesc.Flags = 0;
    // 피처 레벨을 DirectX11로 설정
    featureLevel = D3D_FEATURE_LEVEL_11_0;
    // 스왑 체인, Direct3D 디바이스, Direct3D 디바이스 컨텍스트 생성
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, 
        &featureLevel, 1, D3D11_SDK_VERSION, 
        &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(result))
    {
        return false;
    }
    // 후면 버퍼의 포인터를 얻음
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
    {
        return false;
    }
    // 후면 버퍼 포인터로 렌더 타겟 뷰 생성
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result))
    {
        return false;
    }
    // 후면 버퍼 포인터 해제
    backBufferPtr->Release();
    backBufferPtr = 0;
    // 깊이 버퍼 description 초기화
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    // 깊이 버퍼 description 설정
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    // 채워진 description으로 깊이 버퍼용 텍스처를 생성
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result))
    {
        return false;
    }
    // 스텐실 상태 description을 초기화
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    // 스텐실 상태 description을 설정
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    // 픽셀이 정면을 향할 때의 스텐실 연산
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // 픽셀이 후면을 향할 때의 스텐실 연산
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // 깊이 스텐실 상태 생성
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result))
    {
        return false;
    }
    // 깊이 스텐실 상태를 설정
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
    // 깊이 스텐실 뷰 초기화
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    // 깊이 스텐실 뷰 description 설정
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    // 깊이 스텐실 뷰 생성
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result))
    {
        return false;
    }
    // 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    // 어떤 폴리곤을 어떻게 그릴지 결정하는 래스터 description을 설정
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    // 방금 채운 description으로 래스터라이저 상태 생성
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if (FAILED(result))
    {
        return false;
    }
    // 래스터 라이저 상태 설정
    m_deviceContext->RSSetState(m_rasterState);
    // 렌더링용 뷰포트 설정
    m_viewport.Width = (float)screenWidth;
    m_viewport.Height = (float)screenHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    // 뷰포트를 생성
    m_deviceContext->RSSetViewports(1, &m_viewport);
    // 투영 행렬 설정
    fieldOfView = 3.141592654f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;
    // 3D 렌더링용 투영 행렬 생성
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
    // 월드 행렬을 단위 행렬로 초기화
    m_worldMatrix = XMMatrixIdentity();
    // 2D 렌더링용 직교 투영 행렬을 생성
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
    
    return true;
}

void D3DClass::Shutdown()
{
    // 종료 전에 창모드로 설정 전체 화면에서 스왑 체인 해제 시 예외 발생
    if(m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if(m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = 0;
    }

    if(m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }

    if(m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = 0;
    }

    if(m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if(m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }

    if(m_device)
    {
        m_device->Release();
        m_device = 0;
    }

    if(m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];
    // 버퍼를 지울 색생 설정
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;
    // 후면 버퍼를 지움
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    // 깊이 버퍼를 지움
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
    // 렌더링이 끝났으므로 후면 버퍼를 화면에 표시
    if (m_vsync_enabled)
    {
        // 화면 주사율에 고정
        m_swapChain->Present(1, 0);
    }
    else
    {
        // 가능한 한 빠르게 표시
        m_swapChain->Present(0, 0);
    }
}

ID3D11Device* D3DClass::GetDevice()
{
    return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}

void D3DClass::SetBackBufferRenderTarget()
{
    // 렌더 타켓 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3DClass::ResetViewport()
{
    // 뷰 포트 설정
    m_deviceContext->RSSetViewports(1, &m_viewport);
}
