#pragma once
#include "CScript.h"
class CMonsterScript :
    public CScript
{
private:
    float   m_Time;

public:
    void Tick() override;
    void Begin() override;

    virtual void BeginOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override;
    virtual void EndOverlap(CCollider2D* _OwnCollider, CCollider2D* _OtherCollider) override;

    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}

    CLONE(CMonsterScript);
public:
    CMonsterScript();
    virtual ~CMonsterScript();
};

