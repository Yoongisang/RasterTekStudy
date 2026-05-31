#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "inputclass.h"
#include "applicationclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass& other);
	~SystemClass();
	// 윈도우 생성 및 입력 객체와 애플리케이션 객체 초기화
	bool Initialize();
	// 애플리케이션 객체, 입력 객체, 윈도우를 순서대로 정리
	void Shutdown();
	// 종료 메시지가 올 때까지 메시지 처리와 프레임 처리 반복
	void Run();
	// 키보드 입력 메시지를 입력 객체로 전달하고 나머지는 기본 윈도우 핸들러에 위임
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	// ESC 키 입력 확인 후 애플리케이션 프레임 처리를 수행
	bool Frame();
	// 윈도우 클래스를 설정하고 창을 생성 FULL_SCREEN 변수가 ture면 전체화면 false면 800 * 600 창 생성
	void InitializeWindows(int&, int&);
	// 화면 설정을 복원하고 윈도우와 관련된 핸들 해제
	void ShutdownWindows();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};
// 윈도우 파괴 닫기 메시지를 처리하고 나머지는 MessageHandler로 전달
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;
