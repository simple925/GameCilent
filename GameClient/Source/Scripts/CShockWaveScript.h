#pragma once
#include "CScript.h"
class CShockWaveScript :
    public CScript
{
private:

public:
    virtual void Begin() override;
    virtual void Tick() override;

    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}

    CLONE(CShockWaveScript);
public:
    CShockWaveScript();
    virtual ~CShockWaveScript();
};

