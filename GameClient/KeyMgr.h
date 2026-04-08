#pragma once
#include "single.h"
enum class KEY
{
	Q,
	W,
	E,
	R,
	A,
	S,
	D,
	F,
	Z,
	X,
	C,
	V,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ENTER,
	ALT,
	CTRL,
	LSHIFT,
	RSHIFT,
	SPACE,
	ESC,
	HOME,
	END, //21
	DEL,
	M_LBUTTON,
	M_RBUTTON,
	M_MBUTTON,  // 휠 클릭
	M_XBUTTON1, // 휠 업
	M_XBUTTON2, // 휠 다운
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	KEY_END,
};

//

enum KEY_STATE
{
	NONE,
	TAP,
	PRESSED,
	RELEASED,
};

struct KeyInfo
{
	// key index 필요없음 어차피 enum으로 배열의 자리 값과 동일함
	KEY_STATE	State;		// 키의 현재 상태
	bool		Pressed;	// 이전에 눌린적이 있는지 아닌지
};

class KeyMgr
	: public singleton<KeyMgr>
{
	SINGLE(KeyMgr)
private:
	vector<KeyInfo>		m_vecKeys;

	Vec2				m_MousePos;		// 이번 프레임 마우스 좌표
	Vec2				m_MousePrevPos;	// 이전 프레임 마우스 좌표
	Vec2				m_MouseDir;

	bool				m_WheelChanged;
	int					m_Wheel;		// 1, 0, -1

	HWND				m_hFocus;

	bool				m_Active;

public:
	KEY_STATE GetKeyState(KEY _Key) { return m_vecKeys[(UINT)_Key].State; }
	GET(Vec2, MousePos);
	GET(Vec2, MousePrevPos);
	GET(Vec2, MouseDir);
	GET_SET(bool, Active);
	
	/*
		마우스 이벤트 관리 윈도우 프로그램과
	*/
	GET(int, Wheel)
	void SetWheel(int _Wheel) {
		m_WheelChanged = true;
		/*
		* 1 인지 -1 인지 
		*/
		// abs 앱솔루트 절대값으로 변환
		m_Wheel = _Wheel / abs(_Wheel);
	}

public:
	void Init();
	void Tick();
};

#define KEY_CHECK(key, state) KeyMgr::GetInst()->GetKeyState(key) == state
#define KEY_TAP(key)		KEY_CHECK(key, TAP)
#define KEY_PRESSED(key)	KEY_CHECK(key, PRESSED)
#define KEY_RELEASED(key)	KEY_CHECK(key, RELEASED)
#define KEY_NONE(key)		KEY_CHECK(key, NONE)