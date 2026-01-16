#pragma once
#include "Asset.h"

// vertex(정점), index(인덱스) 정보로 구성된 모델 데이터

class AMesh :
	public Asset
{
private:
	// 정점(vertex) 버퍼 점 세개로 하나의 면
	// 정점을 저장하는 버퍼(3개의 정점을 저장시킬 예정, 삼각형을 표현하기 위해서)
	ComPtr<ID3D11Buffer>		m_VB;       // 정점(Vertex) 버퍼
	D3D11_BUFFER_DESC           m_VBDesc;     // 정점 버터 생성
	UINT                        m_VtxCount; // 정점 개수
	// IndexBuffer
	ComPtr<ID3D11Buffer>		m_IB;       // 인덱스 버퍼
	D3D11_BUFFER_DESC           m_IBDesc;     // 인덱스 버퍼 생성 옵션
	UINT                        m_IdxCount; // 인덱스 개수
public:
	int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
	void Binding();
	void Render();
public:
	AMesh();
	virtual ~AMesh();
};

