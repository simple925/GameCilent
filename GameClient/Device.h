#pragma once


// Directx11 라이브러리 함수를 이용해서 
// GPU 를 제어할 수 있는 클래스
class Device
	: public singleton<Device>
{
	SINGLE(Device);
private:
	ComPtr<ID3D11Device>			m_Device;			// GPU 메모리 관리
	ComPtr<ID3D11DeviceContext>		m_Context;			// GPU 제어 및 렌더링, GPGPU(General Purpose) - ComputeShader 명령

	ComPtr<IDXGISwapChain>			m_SwapChain;
	ComPtr<ID3D11Texture2D>			m_RenderTarget;
	ComPtr<ID3D11Texture2D>			m_DepthStencilTarget;

	ComPtr<ID3D11RenderTargetView>	m_RTV;
	ComPtr<ID3D11DepthStencilView>	m_DSV;

	//ComPtr<ID3D11SamplerState>		m_arrSampler[2];
	//ComPtr<ID3D11RasterizerState>	m_RSState;
	//ComPtr<ID3D11DepthStencilState>	m_DSState;
	//ComPtr<ID3D11BlendState>		m_BSState;

	HWND							m_hWnd;			// 출력윈도우
	Vec2							m_RenderResol;

public:
	int Init(HWND _hwnd, Vec2 _Resolution);
	void Present() { m_SwapChain->Present(0, 0); }
	ComPtr<ID3D11Device> GetDevice() { return m_Device; };
	ComPtr<ID3D11DeviceContext> GetContext() { return m_Context; };
	HWND GetHwnd() { return m_hWnd; }
	Vec2 GetRenderResol() { return m_RenderResol; }
	void ClearTarget();
private:
	int CreateSwapChain();
	int CreateBuffer();
};

#define DEVICE Device::GetInst()->GetDevice().Get()
#define CONTEXT Device::GetInst()->GetContext().Get()