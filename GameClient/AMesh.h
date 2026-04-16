#pragma once
#include "Asset.h"

// vertex(정점), index(인덱스) 정보로 구성된 모델 데이터

class AMesh :
	public Asset
{
private:
	// 정점(vertex) 버퍼 점 세개로 하나의 면
	// 정점을 저장하는 버퍼(3개의 정점을 저장시킬 예정, 삼각형을 표현하기 위해서)
	ComPtr<ID3D11Buffer>		m_VB;        // 정점(Vertex) 버퍼
	D3D11_BUFFER_DESC           m_VBDesc;    // 정점 버퍼 생성 옵션
	UINT                        m_VtxCount;  // 정점 개수
	vector<Vtx>						m_VtxSysMem; // 정점 데이터 시스템메모리 저장

	ComPtr<ID3D11Buffer>		m_IB;        // 인덱스 버퍼
	D3D11_BUFFER_DESC           m_IBDesc;    // 인덱스 버퍼 생성 옵션
	UINT                        m_IdxCount;  // 인덱스 개수
	vector<UINT>						m_IdxSysMem; // 인덱스 게이터 시스템 메모리 저장
public:
	int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
	void Binding();
	void Render();
	void Render_Particle(UINT _Count);

	vector<Vtx> GetVtxSysMem() { return m_VtxSysMem; }

	virtual int Save(const wstring& _FilePath);
	virtual int Load(const wstring& _FilePath);
public:
	UINT GetVertexCount() const { return m_VtxCount; }
	UINT GetIndexCount() const { return m_IdxCount; }

	bool HasVertexBuffer() const { return m_VB != nullptr; }
	bool HasIndexBuffer() const { return m_IB != nullptr; }

	D3D11_PRIMITIVE_TOPOLOGY GetTopology() const
	{
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	// 바운딩 박스 계산용
	Vec3 GetMin() const;
	Vec3 GetMax() const;
public:
	AMesh();
	virtual ~AMesh();
};

