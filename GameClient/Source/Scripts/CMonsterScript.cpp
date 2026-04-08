#include "pch.h"
#include "Source\\Scripts\\CMonsterScript.h"
#include "GameObject.h"
#include "AssetMgr.h"
#include "TimeMgr.h"
#include "LevelMgr.h"
CMonsterScript::CMonsterScript()
	: CScript(SCRIPT_TYPE::MONSTERSCRIPT)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::Begin()
{
	ADD_DYNAMIC_BEGIN_OVERLAP(CMonsterScript::BeginOverlap);
	ADD_DYNAMIC_END_OVERLAP(CMonsterScript::EndOverlap);
}

void CMonsterScript::BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
	if (nullptr != GetOwner()->GetRenderCom() || nullptr != GetOwner()->GetRenderCom()->GetMaterial())
	{
		// 가리키고 있던 원본재질을 복사해서 반환
		Ptr<AMaterial> pDynamicMtrl = GetOwner()->GetRenderCom()->CreateDynamicMaterial();
		pDynamicMtrl->SetScalar(VEC4_0, Vec4(2.f, 1.f, 1.f, 1.f));
	}
}

void CMonsterScript::EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
	if (nullptr != GetOwner()->GetRenderCom() || nullptr != GetOwner()->GetRenderCom()->GetMaterial())
	{
		// 현재 사용중인 재질을 원본 재질로 변경함
		GetOwner()->GetRenderCom()->GetSharedMaterial();
	}
}

void CMonsterScript::Tick()
{

}