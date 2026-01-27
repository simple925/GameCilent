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
	M_LBUTTON,
	M_RBUTTON,
	M_MBUTTON,  // 휠 클릭
	M_XBUTTON1, // 휠 업
	M_XBUTTON2, // 휠 다운
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
public:
	KEY_STATE GetKeyState(KEY _Key) { return m_vecKeys[(UINT)_Key].State; }
public:
	void Init();
	void Tick();
};

#define KEY_CHECK(key, state) KeyMgr::GetInst()->GetKeyState(key) == state
#define KEY_TAP(key)		KEY_CHECK(key, TAP)
#define KEY_PRESSED(key)	KEY_CHECK(key, PRESSED)
#define KEY_RELEASED(key)	KEY_CHECK(key, RELEASED)
#define KEY_NONE(key)		KEY_CHECK(key, NONE)

