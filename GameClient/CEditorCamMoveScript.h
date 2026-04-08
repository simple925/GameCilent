#pragma once
#include "CScript.h"
class CEditorCamMoveScript :
    public CScript
{
private:
    float   m_Speed;

public:
    virtual void Tick() override;

private:
    void MovePerspective();
    void MoveOrthographic();

    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}

    CLONE(CEditorCamMoveScript);
public:
    CEditorCamMoveScript();
    virtual ~CEditorCamMoveScript();
};