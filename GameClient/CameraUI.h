#pragma once
#include "ComponentUI.h"
class CameraUI :
    public ComponentUI
{
private:
public:
    virtual void Tick_UI();
public:
    CameraUI();
    virtual ~CameraUI();
};

