#pragma once
#include "Asset.h"
#include "Layer.h"

class ALevel :
    public Asset
{
private:
    Layer       m_arrLayer[MAX_LAYER]; // 하나의 레벨에
public:
    void AddObject(int _LayerIdx, Ptr<GameObject> _Object);
    void Tick();
    void FinalTick();
    void Render();
public:
    ALevel();
    virtual ~ALevel();
};

