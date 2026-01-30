#pragma once
#include "Asset.h"
class ATexture :
    public Asset
{
private:
    ScratchImage                        m_Image; // Content 폴더에 있는 이미지 파일을 메모리(SysMem) 로 불러드림
    ComPtr<ID3D11Texture2D>             m_Tex2D; // Sysmem
    D3D11_TEXTURE2D_DESC                m_Desc;
    ComPtr<ID3D11RenderTargetView>      m_RTV;
    ComPtr<ID3D11DepthStencilView>      m_DSV;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;

    int                                 m_RecentNum;
public:
    int Load(const wstring& _strFilePath);
    void Binding(UINT _RegisterNum);
    void Clear();
public:
    ATexture();
    virtual ~ATexture();
};

