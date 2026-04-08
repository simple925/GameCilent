#include "pch.h"
#include "Source\\Scripts\\CStateScript.h"
#include "GameObject.h"
#include "TimeMgr.h"
CStateScript::CStateScript()
	: CScript(SCRIPT_TYPE::STATESCRIPT)
	, m_iHP(10)
	, m_iMaxHP(10)
{
}

CStateScript::~CStateScript()
{
}

void CStateScript::TakeDamage(int _Damage)
{
	m_iHP -= _Damage;
	if (m_iHP <= 0)
	{
		m_iHP = 0;
		// 여기서 사망 이벤트 등을 발생시킬 수 있음
		Destroy();
	}
} 
void CStateScript::Begin()
{
	Collider2D()->AddDynamicBeginOverlap(this, (COLLISION_EVENT)&CStateScript::BeginOverlap);
	Collider2D()->AddDynamicOverlap(this, (COLLISION_EVENT)&CStateScript::Overlap);
	Collider2D()->AddDynamicEndOverlap(this, (COLLISION_EVENT)&CStateScript::EndOverlap);
}
void CStateScript::Tick()
{
}
void CStateScript::BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
	int OwnerLayerIdx = _OwnCollider->GetOwner()->GetLayerIdx();
	int OtherLayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();
	if(OwnerLayerIdx == 5 && OtherLayerIdx == 4) TakeDamage(1);
	if(OwnerLayerIdx == 3 && OtherLayerIdx == 6) TakeDamage(1);
}
void CStateScript::Overlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
}
void CStateScript::EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
}

