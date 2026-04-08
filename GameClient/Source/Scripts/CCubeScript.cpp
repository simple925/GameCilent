#include "pch.h"
#include "CCubeScript.h"
#include "LevelMgr.h"
CCubeScript::CCubeScript()
	: CScript(SCRIPT_TYPE::CUBESCRIPT)
	, m_RootMap(nullptr)
	, m_Z(0.f)
{
}

CCubeScript::~CCubeScript()
{
}



void CCubeScript::Init()
{
}

void CCubeScript::Begin()
{
	ADD_DYNAMIC_BEGIN_OVERLAP(CCubeScript::BeginOverlap);
	//m_RootMap = LevelMgr::GetInst()->GetLevel()->FindObjectByName(L"rMap");
}

void CCubeScript::Tick()
{
}

void CCubeScript::BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider)
{
	if (_OtherCollider->GetOwner()->GetName() == L"Player")
	{
		if (m_RootMap)
		{
			m_RootMap->SetPlayerZ(m_Z);
		}
	}
}



void CCubeScript::SaveToLevelFile(FILE* _File)
{
}

void CCubeScript::LoadFromLevelFile(FILE* _File)
{
}


