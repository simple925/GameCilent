#pragma once
#include "Component.h"
class CScript :
    public Component
{
private:

public:
    virtual void Tick() = 0;
public:
    CScript();
    virtual ~CScript();
};

