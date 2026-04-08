#pragma once
#include "CScript.h"
#include "Source\\Scripts\\CMapScript.h"

class CCubeScript :
    public CScript
{
private:
	CMapScript* m_RootMap;
	float m_Z;
public:
	GET_SET(float, Z);
	void SetMapScript(CMapScript* _RootMap) { m_RootMap = _RootMap; }
public:
	virtual void BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override;
	virtual void Overlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override {}
	virtual void EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override {}
public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;
	virtual void SaveToLevelFile(FILE* _File) override;
	virtual void LoadFromLevelFile(FILE* _File) override;
	CLONE(CCubeScript);
public:
	CCubeScript();
	~CCubeScript();
};