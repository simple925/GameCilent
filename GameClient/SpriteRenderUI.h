#pragma once
#include "ComponentUI.h"
class SpriteRenderUI :
    public ComponentUI
{
public:
    virtual void Tick_UI();
public:
    SpriteRenderUI();
    virtual ~SpriteRenderUI();
};

