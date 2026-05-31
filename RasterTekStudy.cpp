// RasterTekStudy.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "systemclass.h"
#include "RasterTekStudy.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdilne, int iCmdshow)
{
    SystemClass* System;
    bool result;

    // 시스템 객체 생성
    System = new SystemClass;

    // 시스템 객체 초기화 및 실행
    result = System->Initialize();
    if (result)
    {
        System->Run();
    }

    // 시스템 객체 종료 및 메모리 해제
    System->Shutdown();
    delete System;
    System = nullptr;

    return 0;

}
