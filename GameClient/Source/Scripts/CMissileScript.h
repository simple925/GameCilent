#pragma once
#include "CScript.h"

class CMissileScript :
    public CScript
{
    Ptr<GameObject>     m_Target;
    Vec3            m_Dir;
public:
    void SetTarget(Ptr<GameObject> _Target) { m_Target = _Target; }
public:
    virtual void Begin() override;
    virtual void Tick();
    virtual void BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override;
    virtual void Overlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override;
    virtual void EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override;

    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CMissileScript);
public:
    CMissileScript();
    virtual ~CMissileScript();
};

