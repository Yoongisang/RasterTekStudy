#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
	bool result;
	// Direct3D 객체를 생성하고 초기화
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}
	
	// 카메라 객체를 생성.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// 카메라의 초기 위치를 설정.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// 모델 객체를 생성하고 초기화.
	m_Model = new ModelClass;
	strcpy_s(textureFilename, "stone01.tga");

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	
	m_Light = new LightClass;
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// 조명 셰이더 객체를 해제.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}


	// 모델 객체를 해제.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 카메라 객체를 해제.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	
	// 3D 객체를 해제
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Frame()
{
	bool result;
	// 그래픽 장면 렌더링
	static float rotation = 0.0f;
	rotation += 0.01f;
	if (rotation >= XM_2PI)
	{
		rotation -= XM_2PI;
	}
	result = Render(rotation);

	return result; // 기존 튜토리얼에서 불필요한 코드 제거
}

bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	
	// 장면을 시작하기 위해 버퍼를 지움
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 카메라의 위치를 바탕으로 뷰 행렬을 생성.
	m_Camera->Render();

	// 카메라와 d3d 객체에서 월드·뷰·투영 행렬을 가져옴.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	worldMatrix = XMMatrixRotationY(rotation);

	// 그리기를 준비하기 위해 모델의 정점·인덱스 버퍼를 그래픽 파이프라인에 올림.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 조명 셰이더에 행렬, 텍스처, 빛 방향과 색을 전달.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return result;
	}

	// 장면을 화면에 표시
	m_Direct3D->EndScene();
	
	return result;
}
