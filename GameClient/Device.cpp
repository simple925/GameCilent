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

	// View - 리소스의 전달자, 매니징 역할. 연결된 리소스의 무결성을 보증, 실제 리소스가 필요한 곳에다가 연결된 
	//        담당 View 를 전달해서 리소스을 연결해줌
	// RenderTargetView
	// DepthStnecilView
	// ShaderResourceView
	// UnorderedAccessView	

	CreateConstBuffer();

	// 기본 샘플러 생성
	if (FAILED(CreateSampler()))
		return E_FAIL;

	// 앞으로 사용할 RasterizerState 제작
	if (FAILED(CreateRasterizerState()))
		return E_FAIL;

	// 앞으로 사용할 BlendState 제작
	if (FAILED(CreateBlendState()))
		return E_FAIL;

	// 각 b 레지스터 번호별로 대응하는 상수버퍼 제작
	CreateConstBuffer();

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

int Device::CreateBlendState()
{
	// 1. Default		-	// (SrcRGB * 1) + (DestRGB * 0)
	// Context 에 nullptr 를 전달하면, 기본 blendState를 사용한다는 뜻
	m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;

	// 2. AlphaBlend	-	// (SrcRGB * SrcA) + (DestRGB * (1 - SrcA))
	D3D11_BLEND_DESC Desc = {};
	Desc.AlphaToCoverageEnable = false;  // 깊이 텍스쳐 관련 설정
	Desc.IndependentBlendEnable = false; // 렌더타겟이 여러개인 경우
	Desc.RenderTarget[0].BlendEnable = true;

	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // blending 결과를 target 에 출력
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHABLEND].GetAddressOf());

	// 3. One_One		-	//(SrcRGB * 1) + (DestRGB * 1 *)
	Desc = {};
	Desc.AlphaToCoverageEnable = false;  // 깊이 텍스쳐 관련 설정
	Desc.IndependentBlendEnable = false; // 렌더타겟이 여러개인 경우
	Desc.RenderTarget[0].BlendEnable = true;

	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // blending 결과를 target 에 출력
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());
	return S_OK;
}

void Device::CreateConstBuffer()
{
	// 앞으로 사용할 상수버퍼 미리 생성	
	m_CB[(UINT)CB_TYPE::TRANSFORM] = new ConstBuffer;
	m_CB[(UINT)CB_TYPE::TRANSFORM]->Create(CB_TYPE::TRANSFORM, sizeof(TransformMatrix));

	m_CB[(UINT)CB_TYPE::MATERIAL] = new ConstBuffer;
	m_CB[(UINT)CB_TYPE::MATERIAL]->Create(CB_TYPE::MATERIAL, sizeof(MtrlConst));
}

int Device::CreateSampler()
{
	// 이방성 필터링 + 랩 모드
	D3D11_SAMPLER_DESC Desc = {};
	Desc.Filter			= D3D11_FILTER_ANISOTROPIC; // 3D에서 자주 사용됨 보간이 들어감
	// Adress Mode 입력된 소수짐으로 랩핑됨
	Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // MIRROR, 
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // 3차원 만들때 쓰임 게임에서는 거의 안씀 확인은 필요

	DEVICE->CreateSamplerState(&Desc, m_arrSam[0].GetAddressOf());

	CONTEXT->VSSetSamplers(0, 1, m_arrSam[0].GetAddressOf());
	CONTEXT->HSSetSamplers(0, 1, m_arrSam[0].GetAddressOf());
	CONTEXT->DSSetSamplers(0, 1, m_arrSam[0].GetAddressOf());
	CONTEXT->GSSetSamplers(0, 1, m_arrSam[0].GetAddressOf());
	CONTEXT->PSSetSamplers(0, 1, m_arrSam[0].GetAddressOf());


	// POINT 필터링 + 랩 모드 보간이 안들어감
	Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 2D에서 자주 사용됨
	// Adress Mode 입력된 소수짐으로 랩핑됨
	Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // MIRROR, 
	Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // 3차원 만들때 쓰임 게임에서는 거의 안씀 확인은 필요

	DEVICE->CreateSamplerState(&Desc, m_arrSam[1].GetAddressOf());

	CONTEXT->VSSetSamplers(1, 1, m_arrSam[1].GetAddressOf());
	CONTEXT->HSSetSamplers(1, 1, m_arrSam[1].GetAddressOf());
	CONTEXT->DSSetSamplers(1, 1, m_arrSam[1].GetAddressOf());
	CONTEXT->GSSetSamplers(1, 1, m_arrSam[1].GetAddressOf());
	CONTEXT->PSSetSamplers(1, 1, m_arrSam[1].GetAddressOf());
	return S_OK;
}

int Device::CreateRasterizerState()
{
	// CULL_BACK
	m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	// CULL_FRONT
	D3D11_RASTERIZER_DESC Desc = {};
	Desc.CullMode = D3D11_CULL_FRONT;
	Desc.FillMode = D3D11_FILL_SOLID;
	DEVICE -> CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

	// CULL_NONE
	Desc = {};
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_SOLID;
	DEVICE -> CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	// WIRE_FRAME
	Desc = {};
	Desc.CullMode = D3D11_CULL_NONE;
	Desc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE -> CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return S_OK;
}

// 깊이 타겟
// 


// dx11 상속
// iunknwn d3d11 resolce