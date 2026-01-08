// GameClient.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//


#define MY_MACRO(name) name##name
#include "framework.h"
#include "GameClient.h"
#include "Engine.h"

#define MAX_LOADSTRING 100
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다. 포인터 자료형

// WCHAR == wchar_t
WCHAR szTitle[100] = L"GameClient";
WCHAR szWindowClass[100] = L"GAMECILENT";

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);









// HINSTANCE hInstance 프로세스 아이디
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    hInst = hInstance; // 내가 만든 프로세스 아이디
    /*
    int Value = 0;
    Add(10, 20, &Value);

    POINT point = {};
    PPOINT ppoint = &point;
    typedef struct tagPOINT
    {
        LONG  x;
        LONG  y;
    } POINT, * PPOINT, NEAR* NPPOINT, FAR* LPPOINT;
     NEAR*
     FAR* 옛윈도우 잔재
    */


    // 전역 문자열을 초기화합니다. 
    // String Table 리소스에 등록된 문자열을 ID를 키값으로 해서 찾아와서 배열에 세팅하는 함수
    //LoadStringW(hInstance, MY_ID, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_GAMECLIENT, szWindowClass, MAX_LOADSTRING);

    // 생성시킬 윈도우(창) 옵션 설정
    //MyRegisterClass(hInstance);

    WNDCLASSEXW wcex;

    //wcex.lpszClassName = szWindowClass;
    wcex.lpszClassName  = L"MyGame";
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc; // 함수 포인터 등록
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMECLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(1);
    //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GAMECLIENT); // 사용안할경우 null
    wcex.lpszMenuName = nullptr;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);



    // 등록한 윈도우 설정 데이터를 기반으로, 윈도우(창) 하나 생성
    //hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    // 커널 오브젝트 - OS 가 관리하는 객체
    // 핸들         - 커널 오브젝트의 ID 개념
    // 생성이 안된경우 0 리턴
    // 성공하면 윈도우 id 리턴
    
    //HWND hWnd = CreateWindowW(L"MyGame", L"MyGame",  WS_POPUP/*WS_OVERLAPPEDWINDOW*/, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    HWND hWnd = CreateWindowW(L"MyGame", L"MyGame",  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


    // 윈도우 생성 실패 시 프로그램 종료
    if (!hWnd)
    {
        return FALSE;
    }

    // 생성한 윈도우를 화면에 보이도록 설정
    ShowWindow(hWnd, true);
    UpdateWindow(hWnd);


    // 내 모니터 해상도 얻어옴
    //UINT width = GetSystemMetrics(SM_CXSCREEN);
    //UINT hegit = GetSystemMetrics(SM_CYSCREEN);
    // 윈도우 크기 및 위치 변경
    //SetWindowPos(hWnd, nullptr, 0, 0, xsize, ysize, 0);
    SetWindowPos(hWnd, nullptr, 0, 0, 1600, 900, 0);

    // 검은색으로 모든 pixel 채우기 cpu 방식
    HDC dc = GetDC(hWnd); // 입력으로 넣어준 윈도우에 그림을 그릴수 있게 해주는 도구
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // 검은색 브러쉬 생성
    SelectObject(dc, hBrush); // dc 에 검은색 Brush 를 전달
    Rectangle(dc, 0, 0, 1600, 900); // dc 를 이용해서 윈도우에 검은색 사각형 그리기

    // 프로세스는 여러개의 윈도우를 가질 수도, 하나도 가지지 않을 수도 있다.
    // 게임 화면을 출력시킬 윈도우가 반드시 필요한 프로그램
    // 따라서 윈도우가 생성되지 않거나, 윈도우를 종료하면, 곧 프로그램 종료로 싱크를 맞춰둔 상태

    // ## 이어붙힌다는 뜼
    struct HPEN___
    { 
        int unused;
    }; typedef struct HPEN___* pHPEN;

    //MY_MACRO(hi);

    HPEN; // HPEN__* pen; HPEN pen;
    HBRUSH;
    HDC;
    HANDLE; // void 포인터 같은 어떤 핸들을 다 받을 수 있음

    // KernelObject는 ID로 구분하기 때문에
    // ID 숫자만 봐서는, 어떤 종류의 KernelObject인지 구분할 수 없음
    // 따라서 서로 호환되지 않게 하기위해서 KernelObject 별로 포인터 타입 자료형을 만들어서 이름을 재정의함
    // ex) HWND(HWND__*)    HPEN(HPEN__*) HBRUSH(HBRUSH__*)

    // 어떤 핸들이던 다 받을 수 있는 HANDLE 자료형은 사실 void* 이다.

    // 메세지 루프
    // Accel 단축키
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMECLIENT));

    // 윈도우에 발생한 사건(이벤트, 메세지)를 받을 구조체
    MSG msg = {};

    // 기본 메시지 루프입니다:
    // 윈도우에 발생한 이벤트, 사건, 메세지를 가져온다.
    // 윈도우 종료 시 GetMessage false 반환
    // 가져오는 메세지가 WM_QUIT 이면, false를 반환한다.
    
    // PeekMessage 메세지가 있던 없던 즉시 반환됨
    // GetMessage 반응형 프로그램에 적합

    // GetMessage VS PeekMessage
    // GetMessage 함수는 메세지 큐에 메세지가 없으면, 리턴하지 않는다.
    // 메세지가 있으면, 반환한다. 메세지가 WM_QUIT 이 아니면 true,
    // 메세지가 WM_QUIT 인 경우에만 리턴을 false 리턴한다.

    // PeekMessage 함수는 메세지 큐에 메세지가 있던 없던 함수를 리턴 시킨다.
    // 메세지가 있었으면 true, 없었으면 false를 반환
    // 확인한 메세지를 GetMessage 함수처럼 제거하려면, PM_REMOVE 옵션을 추가인자로 넣어줘야 함

    // Peek - 몰래보다.
    while (true)
    {
        // 메세지 큐에서 메세지를 꺼낸게 있다.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            // TranslateAccelerator 단축키 번역한다
            // 단축키 관련된 내용이면 TranslateAccelerator 함수에서 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                // 단축키 관련된 이벤트가 아니면 TranslateMessage, DispatchMessage 함수를 이용해서 처리
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        // 메세지 큐에 메세지가 없었다.
        // message queue empty
        else
        {
            // Game Start
            //Engine::GetInst->Progres();
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
   
//}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
  
//}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 프로시저 함수
// 윈도우에 발생한 사건(이벤트, 메세지)를 처리해주는 함수
// DispatchMessage 내에 호출함
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
        PostQuitMessage(0); // 윈도우 종료함수
                            // 메세지큐에 WN_QUIT 들어감
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
