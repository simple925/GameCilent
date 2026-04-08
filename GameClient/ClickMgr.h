#pragma once
#include "single.h"
#include "GameObject.h"
class ClickMgr :
    public singleton<ClickMgr>
{
    SINGLE(ClickMgr);
private:
    Ptr<GameObject> m_Hovered;
public:
    void Init();
    void Progress();
};

