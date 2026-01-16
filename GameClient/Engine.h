#pragma once

// 외부에서 생성자를 호출하지 못하도록 함
class Engine
	: public singleton<Engine>
{
private:
	SINGLE(Engine)
private:
	HINSTANCE m_hInst;
	HWND	m_hWnd;
	Vec2	m_Resolution;

public:
	HINSTANCE GetInstance() {
		return m_hInst;
	}

	// main 핸들 리턴
	HWND GetMainWinHandle() {
		return m_hWnd;
	}

	Vec2 GetResolution() { return m_Resolution; }
public:
	int Init(HINSTANCE _hInst, UINT _Width, UINT _Height);
	int Progress();
};