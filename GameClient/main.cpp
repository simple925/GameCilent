#include "pch.h"

#include "Engine.h"

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

class cTest {

private:
	const int m_i;
public:
	//cTest& operator = (const cTest& _test) {
		//m_i = _test.m_i;
		//return *this;
	//}
	cTest()
		: m_i(0)
	{

	}
	cTest(const int& a) = delete; // 해당 생성자를 자동으로 구현하지 않게 만듬
};




// SAL : 주석 언어
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// CRT new, delete 디버깅 모드에서 메모리 누수 추적, 출력창에 알림
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(150); // 누수 발생지점 중단점 걸어주는 기능
	//int* a = new int;

	hInst = hInstance;

	// Engine 초기화
	// 최상위 관리자
	if (FAILED(Engine::GetInst()->Init(hInstance, 1600, 900)))
	{
		return 0;
	}



	// 메세지 루프
	// 단축키 테이블 정보 핸들 받음
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMECLIENT));

	// 윈도우에 발생한 사건(이벤트, 메세지) 를 받을 구조체
	MSG msg = {};

	// 윈도우에 발생한 이벤트, 사건, 메세지를 가져온다.
	// 메세지 큐에서 메세지를 가져온다.
	// 가져오는 메세지가 WM_QUIT 이면, false 를 반환한다.


	// GetMessage VS PeekMessage 
	// GetMessage 함수는 메세지 큐에 메세지가 없으면, 리턴하지 않는다
	// 메세지가 있으면, 반환한다. 메세지가 WM_QUIT 이 아니면 true, 
	// 메세지가 WM_QUIT 인 경우에만 리턴을 false 리턴한다.

	// PeekMessage 함수는 메세지 큐에 메시지가 있어도 없어도 함수를 리턴 시킨다.
	// 메세지가 있었으면 true, 없었으면 false 를 반환
	// Peek - 몰래 보다
	// 확인한 메세지를 GetMessage 함수처럼 제거하려면, PM_REMOVE 옵션을 추가인자로 넣어줘야 함

	while (true)
	{
		// 메세지 큐에서 메세지를 꺼낸게 있다.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			// 단축키 관련된 내용이면 TranslateAccelerator 함수에서 처리
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				// 단축키 관련된 이벤트가 아니면 TranslateMessage, DispatchMessage 함수를 이용해서 처리
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// 메세지 큐에 메세지가 없었다.
		else
		{
			// Game 실행
			if (FAILED(Engine::GetInst()->Progress())) break;
		}
	}

	//Engine::Destroy();

	return (int)msg.wParam;
}


// 프로시저 함수
// 윈도우에 발생한 사건(이벤트, 메세지)를 처리해주는 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
		//PostQuitMessage(0); // 윈도우 종료함수
							// 메세지큐에 WM_QUIT 이 들어감
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
