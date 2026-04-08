#include "pch.h"
#include "CTransform.h"
#include "Device.h"

CTransform::CTransform()
	: Component(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.f, 1.f, 1.f))
	, m_Dir{}
	, m_IndependentScale(false)
{
	m_Dir[(UINT)DIR::RIGHT] = Vec3(1.f, 0.f, 0.f);
	m_Dir[(UINT)DIR::UP]	= Vec3(0.f, 1.f, 0.f);
	m_Dir[(UINT)DIR::FRONT] = Vec3(0.f, 0.f, 1.f);
}

CTransform::~CTransform()
{
}

Vec3 CTransform::GetWorldScale()
{
	Vec3 vWorldScale = m_RelativeScale;

	if (m_IndependentScale) return vWorldScale;

	Ptr<GameObject> pParent = GetOwner()->GetParent();
	while (nullptr != pParent)
	{

		vWorldScale *= pParent->Transform()->GetRelativeScale();

		if (pParent->Transform()->m_IndependentScale) break;

		pParent = pParent->GetParent();
	}
	return vWorldScale;
}

void CTransform::SetWorldPos(Vec3 _WorldPos)
{
	if (GetOwner()->GetParent())
    {
        // 부모의 월드 역행렬로 월드 좌표 → 로컬 좌표 변환
        Matrix invParent = XMMatrixInverse(nullptr, 
            GetOwner()->GetParent()->Transform()->GetWorldMat());
        Vec3 localPos = XMVector3TransformCoord(_WorldPos, invParent);
        m_RelativePos = localPos;
    }
    else
    {
        // 부모 없으면 월드 = 로컬
        m_RelativePos = _WorldPos;
    }
}

void CTransform::SetWorldScale(Vec3 _WorldScale)
{
	// 부모 없거나 IndependentScale이면 그대로 사용
	if (nullptr == GetOwner()->GetParent() || m_IndependentScale)
	{
		m_RelativeScale = _WorldScale;
		return;
	}

	Ptr<GameObject> pParent = GetOwner()->GetParent();

	// 부모 월드 스케일 구하기
	Vec3 parentScale = Vec3(1.f, 1.f, 1.f);

	while (pParent != nullptr)
	{
		parentScale *= pParent->Transform()->GetRelativeScale();

		if (pParent->Transform()->GetIndependentScale())
			break;

		pParent = pParent->GetParent();
	}

	// 0 나누기 방지
	if (parentScale.x == 0.f) parentScale.x = 1.f;
	if (parentScale.y == 0.f) parentScale.y = 1.f;
	if (parentScale.z == 0.f) parentScale.z = 1.f;

	// 월드 → 로컬
	m_RelativeScale = Vec3(
		_WorldScale.x / parentScale.x,
		_WorldScale.y / parentScale.y,
		_WorldScale.z / parentScale.z
	);
}

void CTransform::FinalTick()
{
	

	// 행열의 4행은 이동정보를 갖도록 약속됨
	// 단위 행열 == 항등행열
	/*
		1 0 0 0
		0 1 0 0
		0 0 1 0
		0 0 0 1
	*/

	// 변환 적용 순서
	// 크(크기, 배율) -> 자(회전, 자전) -> 이(이동) -> 공(회전, 공전)
	// 크기 > 자전 > 이동 > 공전


	Matrix matTrans = XMMatrixIdentity(); // 단위행열로 초기화
	matTrans._41 = m_RelativePos.x;
	matTrans._42 = m_RelativePos.y;
	matTrans._43 = m_RelativePos.z;

	Matrix matScale = XMMatrixIdentity(); // 단위행열로 초기화
	matScale._11 = m_RelativeScale.x;
	matScale._22 = m_RelativeScale.y;
	matScale._33 = m_RelativeScale.z;

	Matrix matRot = XMMatrixRotationX(m_RelativeRot.x) 
				  * XMMatrixRotationY(m_RelativeRot.y) 
				  * XMMatrixRotationZ(m_RelativeRot.z);


	// 방향벡터 계산 초기값으로
	m_Dir[(UINT)DIR::RIGHT] = Vec3(1.f, 0.f, 0.f);
	m_Dir[(UINT)DIR::UP]	= Vec3(0.f, 1.f, 0.f);
	m_Dir[(UINT)DIR::FRONT] = Vec3(0.f, 0.f, 1.f);
	/*
		// coord 행렬
		// 변환행렬을 적용할 Vec3 벡터를 좌표성 데이터로 본다(동차좌표 1로 확장 -> 4행 이동정보 적용)
		//XMVector3TransformCoord(m_Dir[(UINT)DIR::RIGHT], matRot); // 이동정보를 곱함 동차좌표가 1.f 임

		// 변환행렬을 적용할 Vec3 벡터를 방향성 데이터로 본다(동차좌표 0로 확장 -> 4행 이동정보 무시)
		//m_Dir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(m_Dir[(UINT)DIR::RIGHT], matRot);
	*/
	
	m_Dir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(m_Dir[(UINT)DIR::RIGHT], matRot); // 동차좌표를 0으로 적용해서 곱함
	m_Dir[(UINT)DIR::UP]	= XMVector3TransformNormal(m_Dir[(UINT)DIR::UP], matRot); // 동차좌표를 0으로 적용해서 곱함
	m_Dir[(UINT)DIR::FRONT] = XMVector3TransformNormal(m_Dir[(UINT)DIR::FRONT], matRot); // 동차좌표를 0으로 적용해서 곱함

	// 월드행렬 계산 ( 크기 x 회전 x 이동 )
	m_matWorld = matScale * matRot * matTrans;

	// 부모 오브젝트가 있었다면
	if (nullptr != GetOwner()->GetParent())
	{
		// 부모 오브젝트의 크기에 영향을 받지 않는다.
		if (m_IndependentScale)
		{
			Vec3 ParentScale = GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScale = XMMatrixScaling(ParentScale.x, ParentScale.y, ParentScale.z);
			// 역행열 구하는법
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			m_matWorld = m_matWorld * matParentScaleInv * GetOwner()->GetParent()->Transform()->GetWorldMat();
		}
		// 부모 오브젝트의 크기에 영향을 받지 않는다.
		else
			m_matWorld *= GetOwner()->GetParent()->Transform()->GetWorldMat();
	}
}

void CTransform::Binding()
{
	g_Trans.matWorld = m_matWorld;

	// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
	Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->SetData(&g_Trans);
	Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->Binding();
}

void CTransform::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_RelativePos, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeRot, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CTransform::LoadFromLevelFile(FILE* _File)
{
	fread(&m_RelativePos, sizeof(Vec3), 1, _File);
	fread(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fread(&m_RelativeRot, sizeof(Vec3), 1, _File);
	fread(&m_IndependentScale, sizeof(bool), 1, _File);
}

