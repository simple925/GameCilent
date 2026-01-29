#include "pch.h"
#include "CTransform.h"
#include "Device.h"
CTransform::CTransform()
	: Component(COMPONENT_TYPE::TRANSFORM)
	, m_Scale(Vec3(1.f, 1.f, 1.f))
	, m_Dir{}
{
	m_Dir[(UINT)DIR::RIGHT] = Vec3(1.f, 0.f, 0.f);
	m_Dir[(UINT)DIR::UP]	= Vec3(0.f, 1.f, 0.f);
	m_Dir[(UINT)DIR::FRONT] = Vec3(0.f, 0.f, 1.f);
}

CTransform::~CTransform()
{
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
	matTrans._41 = m_Pos.x;
	matTrans._42 = m_Pos.y;
	matTrans._43 = m_Pos.z;

	Matrix matScale = XMMatrixIdentity(); // 단위행열로 초기화
	matScale._11 = m_Scale.x;
	matScale._22 = m_Scale.y;
	matScale._33 = m_Scale.z;

	Matrix matRot = XMMatrixRotationX(m_Rotation.x) 
				  * XMMatrixRotationY(m_Rotation.y) 
				  * XMMatrixRotationZ(m_Rotation.z);


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




}

void CTransform::Binding()
{
	g_Trans.matWorld = m_matWorld;

	// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
	Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->SetData(&g_Trans);
	Device::GetInst()->GetCB(CB_TYPE::TRANSFORM)->Binding();
}
