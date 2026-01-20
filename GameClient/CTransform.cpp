#include "pch.h"
#include "CTransform.h"
#include "Device.h"
CTransform::CTransform()
	: Component(COMPONENT_TYPE::TRANSFORM)
	, m_Scale(Vec3(1.f, 1.f, 1.f))
{
}

CTransform::~CTransform()
{
}

void CTransform::FinalTick()
{
}

void CTransform::Binding()
{
	Transform Trans = {};
	Trans.Pos = m_Pos;
	Trans.Scale = m_Scale;
	Trans.Rot = m_Rotation;

	// 전역변수에 들어있는 오브젝트 위치 정보를 상수버퍼로 복사
	Device::GetInst()->GetTransformBuffer()->SetData(&Trans);
	Device::GetInst()->GetTransformBuffer()->Binding();
}
