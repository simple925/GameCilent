#pragma once
#include "Component.h"
class CClickable :
    public Component

{
private:
    Ptr<GameObject> m_Target;

public:
    void OnClick();
    void OnHover();
public:
    virtual void FinalTick() override;
    virtual void SaveToLevelFile(FILE* _File) {}
    virtual void LoadFromLevelFile(FILE* _File) {}
    CLONE(CClickable);
public:
    CClickable();
    virtual ~CClickable();

};

