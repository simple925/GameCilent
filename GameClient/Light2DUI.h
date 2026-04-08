#pragma once
#include "ComponentUI.h"
class Light2DUI :
    public ComponentUI
{
private:
public:
    virtual void Tick_UI();
public:
    Light2DUI();
    virtual ~Light2DUI();
};

