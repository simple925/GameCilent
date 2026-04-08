#pragma once
#include "ComponentUI.h"
class Collider2DUI :
    public ComponentUI
{
private:
public:
    virtual void Tick_UI();
public:
    Collider2DUI();
    virtual ~Collider2DUI();
};

