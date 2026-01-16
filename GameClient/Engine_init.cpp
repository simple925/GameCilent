#include "pch.h"
#include "Engine.h"

#include "Device.h"
#include "PathMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);

int Engine::Init(HINSTANCE _hInst, UINT _Width, UINT _Height)
{
    m_hInst = _hInst;
    m_Resolution = Vec2(_Width, _Height);

    // 생성시킬 윈도우(창) 옵션 설정
    WNDCLASSEXW wcex = {};

    wcex.lpszClassName = L"MyGame";
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc; // 함수 포인터 등록
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInst;
    wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GAMECLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;// MAKEINTRESOURCEW(IDC_GAMECLIENT); // 메뉴창 제거
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // 등록한 윈도우 설정 데이터를 기반으로, 윈도우(창) 하나 생성
    // 커널 오브젝트 - OS 가 관리하는 객체    
    // 핸들         - 커널 오브젝트의 ID 개념     

    // PopUp 모드로 타이틀 창 제거
    //HWND hWnd = CreateWindowW(L"MyGame", L"MyGame", WS_POPUP/*WS_OVERLAPPEDWINDOW*/, CW_USEDEFAULT, 0, CW_USEDEFAULT
    //                         , 0, nullptr, nullptr, hInstance, nullptr);
    m_hWnd = CreateWindowW(L"MyGame", L"MyGame", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT
        , 0, nullptr, nullptr, m_hInst, nullptr);


    // 윈도우 생성 실패시 프로그램 종료
    if (!m_hWnd)
    {
        return E_FAIL;
    }

    // 생성한 윈도우를 화면에 보이도록 설정
    ShowWindow(m_hWnd, true);
    UpdateWindow(m_hWnd);

    // 게임을 표시할 영역의 해상도가 원하는 크기가 되기 위해서
    // 실제 윈도우 크기는 몇이여야 하는지 계산
    RECT rt = { 0 , 0, (LONG)m_Resolution.x , (LONG)m_Resolution.y };
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, !!GetMenu(m_hWnd));

    // 윈도우 크기 및 위치 변경
    SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);


    // 검은색 칠하기
    //HDC dc = GetDC(m_hWnd); // 입력으로 넣어준 윈도우에 그림을 그릴수 있게 해주는 도구
    //HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // 검은색 브러쉬 생성
    //SelectObject(dc, hBrush); // dc 에 검은색 Brush 를 전달
    //Rectangle(dc, 0, 0, m_Resolution.x, m_Resolution.y); // dc 를 이용해서 윈도우에 검은색 사각형 그리기

    // GPU를 제허할 수 있는 Directx11 함수를 사용하기 위한 매니저
    // ID3D11DDevice, ID3D11DeviceContext
    if (FAILED(Device::GetInst()->Init(m_hWnd, m_Resolution)))
    {
        return E_FAIL;
    }

    // 실행파일과 같이 있는 Context 폴더의 경로를 찾아내기 위함
    PathMgr::GetInst()->Init();

    // 1프레임 동작하는데 걸리는 시간(DT) 계산
    TimeMgr::GetInst()->Init();

    // 각각의 키의 상태를 계산
    KeyMgr::GetInst()->Init();
    return S_OK;
}