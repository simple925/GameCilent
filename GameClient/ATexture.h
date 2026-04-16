#pragma once
#include "Asset.h"
class ATexture :
    public Asset
{
private:
    ScratchImage                        m_Image; // Content 폴더에 있는 이미지 파일을 메모리(SysMem) 로 불러드림
    ComPtr<ID3D11Texture2D>             m_Tex2D; // Sysmem
    D3D11_TEXTURE2D_DESC                m_Desc;

    // 텍스쳐의 용도에 맞는 View 들
    ComPtr<ID3D11RenderTargetView>      m_RTV;
    ComPtr<ID3D11DepthStencilView>      m_DSV;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;

    // u 레지스터 : ComputeShader 시점에만 사용 가능한 레지스터
    // UnorderedAcessView : u 레지스터에 바인딩할때 사용하는 View
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    int                                 m_RecentNum;
    int                                 m_RecentSRV_CS;
    int                                 m_RecentUAV_CS;
public:
    void Binding(UINT _RegisterNum);
    void Clear();

    // ComputeShader 시점, t 레지스터 바인딩
    void Binding_CS_SRV(UINT _RegisterNum);

    // ComputeShader 시점, u 레지스터 바인딩
    void Binding_CS_UAV(UINT _RegisterNum);

    // ComputeShader 시점, t 레지스터 클리어
    void Clear_CS_SRV(int _RegisterNum = -1);

    // ComputeShader 시점, u 레지스터 클리어
    void Clear_CS_UAV();

    float GetWidth() { return (float)m_Desc.Width; }
    float GetHeight() { return (float)m_Desc.Height; }

    ComPtr<ID3D11Texture2D> GetTex2D() { return m_Tex2D; }
    ComPtr<ID3D11RenderTargetView>    GetRTV() { return m_RTV; }
    ComPtr<ID3D11DepthStencilView>    GetDSV() { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView>  GetSRV() { return m_SRV; }
public:
    int Create(UINT _width, UINT _height, DXGI_FORMAT _format, const vector<unsigned char>& _pixelData);
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _format, UINT _Flag, D3D11_USAGE _usage);
    virtual int Load(const wstring& _FilePath);
    virtual int Save(const wstring& _FilePath);
public:
    ATexture();
    virtual ~ATexture();
};

