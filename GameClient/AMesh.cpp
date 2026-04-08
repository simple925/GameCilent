#include "pch.h"
#include "AMesh.h"
#include "Device.h"

AMesh::AMesh()
	: Asset(ASSET_TYPE::MESH)
	, m_VBDesc{}
	, m_IBDesc{}
	, m_VtxSysMem(0)
	, m_IdxSysMem(0)
{
}

AMesh::~AMesh()
{
}

int AMesh::Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount)
{
	m_VtxCount = _VtxCount;
	m_IdxCount = _IdxCount;

	// CPU 메모리 저장
	m_VtxSysMem.assign(_VtxSysMem, _VtxSysMem + _VtxCount);
	m_IdxSysMem.assign(_IdxSysMem, _IdxSysMem + _IdxCount);

	// Vertex Buffer 생성
	m_VBDesc = {};
	m_VBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount;
	m_VBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sub = {};
	sub.pSysMem = m_VtxSysMem.data();

	if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &sub, m_VB.GetAddressOf())))
		return E_FAIL;

	// Index Buffer 생성
	m_IBDesc = {};
	m_IBDesc.ByteWidth = sizeof(UINT) * m_IdxCount;
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sub = {};
	sub.pSysMem = m_IdxSysMem.data();

	if (FAILED(DEVICE->CreateBuffer(&m_IBDesc, &sub, m_IB.GetAddressOf())))
		return E_FAIL;

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
	//CONTEXT->DrawIndexed(1500, 0, 0);
	//m_IdxCount = m_IdxCount / sizeof(UINT);
	CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}



Vec3 AMesh::GetMin() const
{
	//if (!m_VtxSysMem || m_VtxCount == 0) return Vec3(0.f, 0.f, 0.f);
	if (m_VtxSysMem.empty())
		return Vec3(0.f, 0.f, 0.f);

	Vec3 minPos = m_VtxSysMem[0].vPos;

	for (UINT i = 1; i < m_VtxCount; ++i)
	{
		minPos.x = min(minPos.x, m_VtxSysMem[i].vPos.x);
		minPos.y = min(minPos.y, m_VtxSysMem[i].vPos.y);
		minPos.z = min(minPos.z, m_VtxSysMem[i].vPos.z);
	}

	return minPos;
}

Vec3 AMesh::GetMax() const
{
	//if (!m_VtxSysMem || m_VtxCount == 0) return Vec3(0.f, 0.f, 0.f);
	if (m_VtxSysMem.empty())
		return Vec3(0.f, 0.f, 0.f);
	Vec3 maxPos = m_VtxSysMem[0].vPos;

	for (UINT i = 1; i < m_VtxCount; ++i)
	{
		maxPos.x = max(maxPos.x, m_VtxSysMem[i].vPos.x);
		maxPos.y = max(maxPos.y, m_VtxSysMem[i].vPos.y);
		maxPos.z = max(maxPos.z, m_VtxSysMem[i].vPos.z);
	}

	return maxPos;
}

int AMesh::Save(const wstring& _FilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	// vertex / index 개수 저장
	fwrite(&m_VtxCount, sizeof(UINT), 1, pFile);
	fwrite(&m_IdxCount, sizeof(UINT), 1, pFile);

	// vertex 데이터 저장
	fwrite(m_VtxSysMem.data(), sizeof(Vtx), m_VtxCount, pFile);

	// index 데이터 저장
	fwrite(m_IdxSysMem.data(), sizeof(UINT), m_IdxCount, pFile);

	fclose(pFile);

	return 0;
}

int AMesh::Load(const wstring& _FilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	// 개수 읽기
	fread(&m_VtxCount, sizeof(UINT), 1, pFile);
	fread(&m_IdxCount, sizeof(UINT), 1, pFile);

	// 시스템 메모리 할당
	m_VtxSysMem.resize(m_VtxCount);
	m_IdxSysMem.resize(m_IdxCount);

	// 데이터 읽기
	fread(m_VtxSysMem.data(), sizeof(Vtx), m_VtxCount, pFile);
	fread(m_IdxSysMem.data(), sizeof(UINT), m_IdxCount, pFile);

	fclose(pFile);

	// GPU 버퍼 재생성
	Create(m_VtxSysMem.data(), m_VtxCount, m_IdxSysMem.data(), m_IdxCount);

	return 0;
}