#include "pch.h"
#include "KeyMgr.h"

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
	VK_LBUTTON,
	VK_RBUTTON,
	VK_MBUTTON,
	VK_XBUTTON1,
	VK_XBUTTON2,
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
	for (int i = 0; i < (UINT)KEY::KEY_END; ++i)
	{
		SHORT sKey = GetAsyncKeyState(g_KeyIndex[i]);
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
