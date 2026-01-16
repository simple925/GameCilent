#include "pch.h"
#include "Engine.h"
#include "Device.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

int TestFunc();
void TestRelease();

Engine::Engine()
	: m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_Resolution{}
{
}

Engine::~Engine()
{
	TestRelease();
}

int Engine::Progress()
{
	TimeMgr::GetInst()->Tick();
	KeyMgr::GetInst()->Tick();
	// 화면을 검은색으로 색칠한다.
	// 화면에 랜더링
	// 1600*900=144만개 의 픽셀을 0,0,0,0

	// CPU를 이용
	// Window 에서 제공하는 GDI 함수를 사용
	// 

	// GPU를 이용
	// Directx API(함수) 를 이용해서 GPU를 컨트롤하고 명령해야함

	// Pixel 1개당 4byte RGBA  r256 * g256 * b256 = 16,777,216

	if (FAILED(TestFunc())) {
		return E_FAIL;
	}

	// 랜더타겟에 그려진 그림을, 윈도우 비트맵으로 복사
	Device::GetInst()->Present();
	return S_OK;
}

