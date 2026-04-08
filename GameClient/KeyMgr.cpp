#include "pch.h"
#include "KeyMgr.h"
#include "Engine.h"

UINT g_KeyIndex[(UINT)KEY::KEY_END] =
{
	'Q',
	'W',
	'E',
	'R',
	'A',
	'S',
	'D',
	'F',
	'Z',
	'X',
	'C',
	'V',
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	VK_RETURN,
	VK_MENU,
	VK_CONTROL,
	VK_LSHIFT,
	VK_RSHIFT,
	VK_SPACE,
	VK_ESCAPE,
	VK_HOME,
	VK_END,
	VK_DELETE,
	VK_LBUTTON,
	VK_RBUTTON,
	VK_MBUTTON,
	VK_XBUTTON1,
	VK_XBUTTON2,
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
};

KeyMgr::KeyMgr()
{

}
KeyMgr::~KeyMgr()
{

}
void KeyMgr::Init()
{
	// 키 등록
	m_vecKeys.resize((UINT)KEY::KEY_END);
}

void KeyMgr::Tick()
{
	m_hFocus = GetFocus();
	if (m_hFocus == Engine::GetInst()->GetMainWndHwnd() && m_Active)
	{
		for (UINT i = 0; i < (UINT)KEY::KEY_END; ++i)
		{
			SHORT sKey = GetAsyncKeyState(g_KeyIndex[i]);
			// 16bit 0x8000  각 자리수당 1 2 4 8
			bool bDown = (sKey & 0x8000) != 0;
			if (bDown)
			{
				// 이전에도 눌려있었다.
				if (m_vecKeys[i].Pressed)
				{
					m_vecKeys[i].State = PRESSED;
				}
				// 이전까지는 눌린적이 없었다.
				else
				{
					m_vecKeys[i].State = TAP;
				}

				m_vecKeys[i].Pressed = true;
			}
			// 지금 안눌려 있음
			else
			{
				// 이전에도 눌려있었다.
				if (m_vecKeys[i].Pressed)
				{
					m_vecKeys[i].State = RELEASED;
				}
				else
				{
					m_vecKeys[i].State = NONE;
				}
				m_vecKeys[i].Pressed = false;
			}
		}
	}
	// 마우스 좌표 계산
	POINT pt = {};
	GetCursorPos(&pt); // window 기준 마우스 좌표

	ScreenToClient(Engine::GetInst()->GetMainWndHwnd(), &pt); // 윈도우 기준으로 마우스좌표 변경

	m_MousePrevPos = m_MousePos;	// 이전 프레임 정보를 저장
	m_MousePos = Vec2((float)pt.x, (float)pt.y);	// 현재 프레임 정보를 저장

	// 마우스 진행 방향
	m_MouseDir = m_MousePos - m_MousePrevPos;

	// 휠 이벤트 처리
	if (m_WheelChanged)
		m_WheelChanged = false;
	else
		m_Wheel = 0;
}
