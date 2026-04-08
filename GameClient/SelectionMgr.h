#pragma once
#include "single.h"
class SelectionMgr :
    public singleton<SelectionMgr>
{
    SINGLE(SelectionMgr)
private:
    Ptr<GameObject> m_Selected;
    Ptr<GameObject> m_Hovered;

public:
    GET_SET(Ptr<GameObject>, Selected);
    GET_SET(Ptr<GameObject>, Hovered);
};

