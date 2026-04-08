#pragma once
#include "ComponentUI.h"
class FlipbookRenderUI :
    public ComponentUI
{
public:
    virtual void Tick_UI();
public:
    FlipbookRenderUI();
    virtual ~FlipbookRenderUI();
};

