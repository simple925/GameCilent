#pragma once
#include "Asset.h"
class AGraphicShader :
    public Asset
{
private:
    // HLSL 로 작성한 VS 함수를 컴파일한 어셈블리코드를 저장시킬 버퍼
    ComPtr<ID3DBlob>			m_VSBlob;
    // HLSL 로 작성한 PS 함수를 컴파일한 어셈블리코드를 저장시킬 버퍼
    ComPtr<ID3DBlob>			m_PSBlob;

    // VertexShader
    ComPtr<ID3D11VertexShader>	m_VS;
    // PixelShader
    ComPtr<ID3D11PixelShader>	m_PS;

    // InputLayout - 정점 쉐이더에 입력으로 들어오는 정점 하나의 구성정보
    ComPtr<ID3D11InputLayout>	m_Layout;

    D3D11_PRIMITIVE_TOPOLOGY    m_Topology; // 랜더링 과정에서, 정점들을 어떤 도형으로 인지할 것인지
public:
    int CreateVertexShader(const wstring& _RelativeFilePath, const string& _FuncName);
    int CreatePixelShader(const wstring& _RelativeFilePath, const string& _FuncName);
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }

    void Binding();
public:
    AGraphicShader();
    virtual ~AGraphicShader();
};

