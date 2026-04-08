#pragma once
#include "ComponentUI.h"
class TileRenderUI :
    public ComponentUI
{
public:
    virtual void Tick_UI();
public:
    TileRenderUI();
    virtual ~TileRenderUI();
};

