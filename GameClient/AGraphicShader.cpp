#include "pch.h"
#include "AGraphicShader.h"
#include "Device.h"
#include "PathMgr.h"
// 에섯 - 공유자원
// 랜더링 파이프라인 자체를 하나의 에셋으로 본다.
AGraphicShader::AGraphicShader()
	: Asset(ASSET_TYPE::GRAPHICSHADER)
	, m_Topology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_RSType(RS_TYPE::CULL_BACK)
	, m_BSType(BS_TYPE::DEFAULT)
{
}

AGraphicShader::~AGraphicShader()
{
}

int AGraphicShader::CreateVertexShader(const wstring& _RelativeFilePath, const string& _FuncName)
{
	wstring Path = PathMgr::GetInst()->GetContentPath(_RelativeFilePath);
	ComPtr<ID3DBlob> Err;
	// 엔트리포인트
	if (FAILED(D3DCompileFromFile(Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _FuncName.c_str(), "vs_5_0", D3D10_SHADER_DEBUG, 0, m_VSBlob.GetAddressOf(), nullptr))) {
		const char* pErrMsg = (const char*)Err->GetBufferPointer();
		MessageBoxA(nullptr, pErrMsg, "쉐이더 생성 실패", MB_OK);
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf()))) {
		return E_FAIL;
	}
	// 0		12    20
	// |		|	  |
	// v		v     v
	// ---12---  --8--  ---16----
	// Position |  UV |   Color |
	// --------  -----  ---------
	// Input Layout 생성하기
	D3D11_INPUT_ELEMENT_DESC InputDesc[3] = {};

	InputDesc[0].SemanticName = "POSITION";
	InputDesc[0].SemanticIndex = 0;							// Semantic 이름이 중복되는 경우, 구별하기 위한 숫자
	InputDesc[0].AlignedByteOffset = 0;							// 메모리 시작 위치(offset)
	InputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// offset 으로부터 크기
	InputDesc[0].InputSlot = 0;							// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[0].InstanceDataStepRate = 0;

	InputDesc[1].SemanticName = "TEXCOORD";
	InputDesc[1].SemanticIndex = 0;							// Semantic 이름이 중복되는 경우, 구별하기 위한 숫자
	InputDesc[1].AlignedByteOffset = 12;							// 메모리 시작 위치(offset)
	//InputDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// offset 으로부터 크기
	InputDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;	// offset 으로부터 크기
	InputDesc[1].InputSlot = 0;							// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[1].InstanceDataStepRate = 0;

	InputDesc[2].SemanticName = "COLOR";
	InputDesc[2].SemanticIndex = 0;							// Semantic 이름이 중복되는 경우, 구별하기 위한 숫자
	InputDesc[2].AlignedByteOffset = 20;							// 메모리 시작 위치(offset)
	//InputDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// offset 으로부터 크기
	InputDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// offset 으로부터 크기
	InputDesc[2].InputSlot = 0;							// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[2].InstanceDataStepRate = 0;

	// 레이아웃을 사용할 VertexShader 의 코드 정보가 필요
	if (FAILED(DEVICE->CreateInputLayout(InputDesc, 3
		, m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int AGraphicShader::CreatePixelShader(const wstring& _RelativeFilePath, const string& _FuncName)
{
	wstring Path = PathMgr::GetInst()->GetContentPath(_RelativeFilePath);


	// PixelShader
	ComPtr<ID3DBlob> Err;
	if (FAILED(D3DCompileFromFile(Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _FuncName.c_str(), "ps_5_0", D3D10_SHADER_DEBUG, 0, m_PSBlob.GetAddressOf(), nullptr))) {
		const char* pErrMsg = (const char*)Err->GetBufferPointer();
		MessageBoxA(nullptr, pErrMsg, "쉐이더 생성 실패", MB_OK);
		return E_FAIL;
	}

	// 컴파일한 Shader 코드로 PixelShader 만들기
	if (FAILED(DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer()
		, m_PSBlob->GetBufferSize(), nullptr
		, m_PS.GetAddressOf())))
	{
		return E_FAIL;
	}
	return 0;
}

void AGraphicShader::Binding()
{
	// Graphic Pipeline
	// IA(Input Assembler)
	// 렌더링에 필요한 리소스 입력

	// 정점으로 구성할 도형의 상태, 모양
	// TriangList 는 점 3개를 이어서 만든 삼각형(내부까지 색을 채움)*****************************
	CONTEXT->IASetPrimitiveTopology(m_Topology);

	// 정점 하나 안에 들어있는 데이터를 구분해주는 정보
	CONTEXT->IASetInputLayout(m_Layout.Get());

	// 랜더링에 필요한 리소스 입력
	// Vertex Shader(함수) - 정점 당 연산 수행 정점쉐이더
	// HLSL(High Level 
	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);

	// Rasterizer State - 정점 쉐이더에서 넘겨준 픽셀쉐이더를 호출할 함수를 연결
	// 정점쉬이더에서 반환한 정점의 취치를 NDC 좌표
	// 기준으로 들어오는 픽셀을 계산해서 픽셀 쉐이더로 연계
	// nullptr 입력: 기본 설정 옵션으로 설정
	// CULL_MODE : CULL_BACK
	// FILL_MODE : SOLID
	// Rasterizer State	
	CONTEXT->RSSetState(Device::GetInst()->GetRSState(m_RSType).Get());

	// Pixel Shader
	// 레스터라이저를 거쳐서, 호출될 픽셀마다 실행되는 함수
	// 픽셀 쉐이더에서 리턴한 값이, 렌더타겟 내에서의 해당 픽셀 위체에 색상이 출력된다.
	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	// OM (Output Merge State)
	// 픽셀 쉐이더에서 리턴한 값울, OM 단계에 연결되어있는 RenderTarget, DepthStencilTarget에 기록한다.
	// DepthStencilState - 깊이 비교	


	ComPtr<ID3D11BlendState> pBSState = Device::GetInst()->GetBSState(m_BSType);
	CONTEXT->OMSetBlendState(pBSState.Get(), nullptr, 0xffffffff);
}

