#include "pch.h"
#include "Device.h"

Device::Device()
	: m_hWnd(nullptr)
{
}

Device::~Device()
{
}

int Device::Init(HWND _hwnd, Vec2 _Resolution)
{
	m_hWnd = _hwnd;
	m_RenderResol = _Resolution;
	// Dx11 라이브러리는 동적 라이브러리 이고
	// Dx11 관련 객체 생성함수를 통해서 생성된 객체의 주소를 받은 경우,
	// 메모리 해제도 Dx11 쪽 함수를 이용해서 해제해줘야 한다.

	// 스마트 포인터


#ifdef _DEBUG
	UINT iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL* pLevel = nullptr;

	// 이중 포인터
	// ID3D11Device, ID3D11DeviceContext
	// D3D11CreateDevice 함수는 는 Dx11 라이브러리에서 제공하는 함수로,
	// ID3D11Device 객체와, ID3D11DeviceContext 객체를 생성해서, 주소를 알려주는 함수
	// ID3D11Device, ID3D11DeviceContext 객체를 생성할때 레퍼런스 카운트를 이미 1을 주고 시작함
	// 따라서 스마트 포인터가 해당 객체를 가리키게 되면, 레퍼런스카운트가 2가 되는 문제가 생김
	// 스마트 포인터가 대상을 가리키는 맴버 포인터의 주소(이중포인터)를 받아와서 가리킬 주소값을 강제로 바로 세팅해주려고
	// 맴버의 주소(이중포인터) 를 요구함
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, iFlag, 0, 0, D3D11_SDK_VERSION
		, m_Device.GetAddressOf(), pLevel, m_Context.GetAddressOf())))
	{
		return E_FAIL;
	}
	CreateSwapChain();

	// SwapChain
	// 윈도우에서 이미지를 표시하기 위해, 비트맵 데이터를 SystemMemory 영역에 가지고 있다.
	// 이 비트맵에 그림을 그려야 윈도우 화면 영역에 표시가 된다.
	if (FAILED(CreateSwapChain()))
	{
		return E_FAIL;
	}
	// GPU 는 이 SystemMemory 영역에 직접적으로 접근이 불가능하다.
	// 따라서 GPU 로 렌더링 작업을 하기 위해서 VRAM 에 RenderTarget(픽셀 데이터 집합체 버퍼) 을 생성하고
	// 여기에 그림을 그린다음, SwapChain 을 이용해서 SystemMemory 에 있는 비트맵으로 픽셀 데이터를 복사받아야 한다.

	// RenderTarget
	// 그림을 그릴 목적지 타겟에 해당하는 RenderTarget 은 SwapChain 생성 시 같이 만들어 진다.

	// DepthStencilTarget
	// 렌더타겟과 동일한 해상도여야 함
	// 렌더타겟이 물체의 그림이 그려진다면, DepthStencil 타겟에는 카메라와 물체의 거리값이 픽셀에 기록이 됨
	// 더 가까운 깊이값을 가지는 물체가 픽셀단위로 렌더타겟에 덮어 씌임, 그리고 자신의 깊이를 깊이타겟에 덮어 씀
	// 만약 먼저 기록된 물체보다 현재 물체의 깊이값이 더 멀면, 렌더타겟에도 그려지지않고, 깊이타겟도 갱신하지 않음

	if (FAILED(CreateBuffer())) {
		return E_FAIL;
	}

	D3D11_VIEWPORT ViewPort = {};
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.Width = m_RenderResol.x;
	ViewPort.Height = m_RenderResol.y;
	ViewPort.MinDepth = 0.f;
	ViewPort.MaxDepth = 1.f;

	// RenderTarget 을 목적지에 출력시킬 영역 설정
	m_Context->RSSetViewports(1, &ViewPort);

	// 랜더타겟, 깊이 타겟 출력 설정
	// 렌더링 파이프라인 과정에서 마지막에 그림을 출력시킬 목적지 설정
	m_Context->OMSetRenderTargets(1, m_RTV.GetAddressOf(), m_DSV.Get());

	// 랜더타겟의 모든 픽셀을 특정 색상으로 칠한다.
	// Dx 에서는 색상 데이터를 0 ~ 1 범위로 정규화(Normalize) 해서 사용한다.
	// Normalize(0 ~ 255 -> 0.f ~ 1.f)(정규화)
	float clearColor[4] = { 0.3f, 0.5f ,0.7f ,1.f };
	m_Context->ClearRenderTargetView(m_RTV.Get(), clearColor);


	// View - 리소스의 전달자, 매니징 역할. 연결된 리소스의 무결성을 보증, 실제 리소스가 필요한 곳에다가 연결된 
	//        담당 View 를 전달해서 리소스을 연결해줌
	// RenderTargetView
	// DepthStnecilView
	// ShaderResourceView
	// UnorderedAccessView	

	// 앞으로 사용할 상수버퍼 미리 생성	
	m_TransformCB = new ConstBuffer;
	m_TransformCB->Create(CB_TYPE::TRANSFORM, sizeof(Transform));
	return S_OK;
}

void Device::ClearTarget()
{
	Vec4 vColor = Vec4(0.f, 0.f, 0.f, 0.f);
	CONTEXT->ClearRenderTargetView(m_RTV.Get(), vColor);
	CONTEXT->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	// NDC 좌표계 찾아보기*****************************************************************************
	//CONTEXT->ClearDepthStencilView(m_DSV, );
}

int Device::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC m_Desc = {};

	// 버퍼 개수
	m_Desc.BufferCount = 1;

	// 버퍼(랜더타겟) 해상도 == 원도우 비트맵 해상도
	m_Desc.BufferDesc.Width = (UINT)m_RenderResol.x;
	m_Desc.BufferDesc.Height = (UINT)m_RenderResol.y;

	// 버퍼의 사용 용도 == 랜더타겟
	m_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 픽셀의 종류, 픽셀 타입
	m_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 스왑체인이 렌더타겟을 출력시킬 목적지 윈도우(bitMap)
	m_Desc.OutputWindow = m_hWnd;

	// 창모드 설정 ( true: 창 모드, false: 전체화면 모드)
	m_Desc.Windowed = true;


	// 랜더타겟의 이미지를 출력 후 버려도 된다.
	m_Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_Desc.BufferDesc.RefreshRate.Numerator = 60;
	m_Desc.BufferDesc.RefreshRate.Denominator = 1;
	m_Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	m_Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;
	m_Desc.Flags = 0;

	// SwapChain 생성
	// IDXGIFactory
	ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
	ComPtr<IDXGIAdapter> pAdapter = nullptr;
	ComPtr<IDXGIFactory> pFactory = nullptr;

	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

	if (FAILED(pFactory->CreateSwapChain(m_Device.Get(), &m_Desc, m_SwapChain.GetAddressOf()))) {
		return E_FAIL;
	}


	return 0;
}


int Device::CreateBuffer()
{
	// RenderTarget
	// SwapChain 생성할때 이미 같이 만들어져있다.
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_RenderTarget.GetAddressOf());

	// DepthStencilTarget 제작하기
	D3D11_TEXTURE2D_DESC Desc = {};

	// 텍스쳐 1장
	Desc.ArraySize = 1;


	// 해상도 - 깊이 타겟도 렌더타겟과 해상도가 동일해야한다.
	Desc.Width = (UINT)m_RenderResol.x;
	Desc.Height = (UINT)m_RenderResol.y;
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 스텐실 ? <<< 찾아보기

	// cpu 메모리 접근 옵션 - cpu 접근 불가
	Desc.CPUAccessFlags = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;

	// 용도 - 깊이를 저장
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Desc.MipLevels = 1; // 밉맵, 저화질 버전 텍스쳐 추가생성, 1 == 원본
	Desc.MiscFlags = 0;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	if (FAILED(m_Device->CreateTexture2D(&Desc, nullptr/*초기 전달할 데이터*/, m_DepthStencilTarget.GetAddressOf()))) {
		return E_FAIL;
	}

	// RenderTagetView, DepthStencilView

	if (FAILED(m_Device->CreateRenderTargetView(m_RenderTarget.Get(), nullptr, m_RTV.GetAddressOf()))) {
		return E_FAIL;
	}
	if (FAILED(m_Device->CreateDepthStencilView(m_DepthStencilTarget.Get(), nullptr, m_DSV.GetAddressOf()))) {
		return E_FAIL;
	}


	return S_OK;
}

// 깊이 타겟
// 


// dx11 상속
// iunknwn d3d11 resolce