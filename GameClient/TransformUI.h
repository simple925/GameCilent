#pragma once
#include "ComponentUI.h"
class TransformUI :
    public ComponentUI
{
private:
public:
    virtual void Tick_UI();
public:
    TransformUI();
    virtual ~TransformUI();
};

