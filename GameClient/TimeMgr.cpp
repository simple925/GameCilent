#include "pch.h"
#include "TimeMgr.h"
#include "Engine.h"
#include "LevelMgr.h"
#include "FontMgr.h"
TimeMgr::TimeMgr()
	: m_Frequency{}
	, m_Prev{}
	, m_Current{}
	, m_DeltaTime(0.f)
	, m_Time(0.f)
	, m_FPS(0)
{

}
TimeMgr::~TimeMgr() {

}

void TimeMgr::Init()
{
	// 1초동안 가능한 카운팅 횟수
	QueryPerformanceFrequency(&m_Frequency); // cpu가 실행하지 않음

	// 현재까지 몇까지 셋는지
	QueryPerformanceCounter(&m_Prev);
}

void TimeMgr::Tick()
{
	++m_FPS;

	// 현재 카운팅 가져오기
	QueryPerformanceCounter(&m_Current);

	// 이전과 현재 카운팅 차이를 Frequency 로 나눠서 1 프레임동안 진행한 시간값을 구하기
	m_DeltaTime = (float)(m_Current.QuadPart - m_Prev.QuadPart) / (float)m_Frequency.QuadPart;

	// Prev 카운팅을 다시 현재카운팅으로 맞추기
	m_Prev = m_Current;

	// 누적 시간 계산
	m_Time += m_DeltaTime;

	// 1초마다 if 수행
	if (1.f < m_Time) {

		wchar_t buff[255] = {};
		swprintf_s(buff, 255, L"DeltaTime : %f, FPS : %d", m_DeltaTime, m_FPS);
		m_strFPS = buff;

		//SetWindowText(Engine::GetInst()->GetMainWndHwnd(), buff);

		// 초기화
		m_FPS = 0;
		m_Time -= 1.f; // m_Time -= 1.f 보다 0.f로 맞추는 게 깔끔할 수 있습니다.
	}

	// Game Engine용 Time
	g_Global.EngineDT = m_DeltaTime;
	g_Global.EngineTime += m_DeltaTime;

	// Level 이 Pause 나 Stop 상태라면
	if (LEVEL_STATE::PLAY != LevelMgr::GetInst()->GetLevelState())
	{
		g_Global.DeltaTime = m_DeltaTime = 0.f;
		g_Global.Time = 0.f;
	}
	// Level 이 Play 상태
	else
	{
		// Game Content 용 Time
		g_Global.DeltaTime = m_DeltaTime;
		g_Global.Time += m_DeltaTime;
	}
}

void TimeMgr::Render()
{
	FontMgr::GetInst()->DrawFont(m_strFPS.c_str(), 10, 30, 24, FONT_RGBA(200, 20, 20, 255));
}
