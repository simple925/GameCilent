#pragma once
#include "CScript.h"
class CPlayerScript :
    public CScript
{
private:
    bool m_bSelected;
public:
    void Tick() override;
    bool IsMouseOver();
public:
    CPlayerScript();
    ~CPlayerScript();
};

