#include "pch.h"
#include "TimeMgr.h"
#include "Engine.h"
TimeMgr::TimeMgr()
	: m_Frequency{}
	, m_Prev{}
	, m_Current{}
	, m_DeltaTime(0.f)
	, m_Time(0.f)

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
		swprintf_s(buff, 255, L"DeltaTime : %f", m_DeltaTime);
		SetWindowText(Engine::GetInst()->GetMainWinHandle(), buff);
		m_Time -= 1.f;
	}

}
