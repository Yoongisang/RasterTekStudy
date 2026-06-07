#include "applicationclass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	// Direct3D 객체를 생성하고 초기화
	m_Direct3D = new D3DClass;
	
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Direct3D", L"Error", MB_OK);
		return result;
	}
	
	return result;
}

void ApplicationClass::Shutdown()
{
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
	result = Render();

	return result; // 기존 튜토리얼에서 불필요한 코드 제거
}

bool ApplicationClass::Render()
{
	// 장면을 시작하기 위해 버퍼를 지움
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);
	// 장면을 화면에 표시
	m_Direct3D->EndScene();
	
	return true;
}
