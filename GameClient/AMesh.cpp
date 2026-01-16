#include "pch.h"
#include "AMesh.h"
#include "Device.h"

AMesh::AMesh()
	: Asset(ASSET_TYPE::MESH)
{
}

AMesh::~AMesh()
{
}

int AMesh::Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount)
{
	m_VBDesc.ByteWidth = _VtxCount; // 버퍼 크기
	m_IBDesc.ByteWidth = _IdxCount;

	// cpu를 통해서 버퍼의 내용을 쓰거나, 읽을 수 있는지
	// D3D11_USAGE_DYNAMIC + D3D11_CPU_ACCESS_WRITE ==> 버퍼를 생성한 이후에도, cpu를 통해서 버퍼의 내용을 수정할 수 있다.
	m_VBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_VBDesc.CPUAccessFlags = 0;
	// 버퍼용도
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;


	// 처음 버퍼 생성할때 전달시킬 데이터의 시작주소를 Sub 구조체에 담아서 CreateBuffer 함수에 넣어준다.
	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _VtxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSub, m_VB.GetAddressOf()))) {
		return E_FAIL;
	}

	// cpu를 통해서 버퍼의 내용을 쓰거나, 읽을 수 있는지
	// D3D11_USAGE_DEFAULT + 0
	// 버퍼를 생성한 이후에 수정할 수 없다.
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_IBDesc.CPUAccessFlags = 0;
	// 버퍼용도
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// 처음 버퍼 생성할때 전달시킬 데이터의 시작주소를 Sub 구조체에 담아서 CreateBuffer 함수에 넣어준다.
	tSub = {}; // 초기화
	tSub.pSysMem = _IdxSysMem; // 명시적으로 주소 대입
	if (FAILED(DEVICE->CreateBuffer(&m_IBDesc, &tSub, m_IB.GetAddressOf()))) {
		return E_FAIL;
	}
	return S_OK;
}

void AMesh::Binding()
{
	UINT Stride = sizeof(Vtx);
	UINT Offset = 0; // 어떤 기준위치 시작을 할지 시작점 변경값
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &Stride, &Offset);

	// Index buffer setting, 정점 버퍼안에 있는 정점을 가리키는 인덱스 정보,
	// 인덱스 숫자 
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void AMesh::Render()
{
	Binding();
	// 랜더링 파이프라인 시작
	// Draw 가 호출되기 전까지 설정해놓은 세팅을 기반으로 실제 렌더링 파이프라인이 실행됨
	// 그 이전까지는 각 단계별로 실행할 옵션을 설정
	//CONTEXT->Draw(6, 0); // vertex buffer 의 정점을 그려줌

	//CONTEXT->DrawIndexed(IDXCOUNT, 0, 0);
	CONTEXT->DrawIndexed(1500, 0, 0);
	//m_IdxCount = m_IdxCount / sizeof(UINT);
	//CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

