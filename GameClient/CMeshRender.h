#pragma once
#include "CRenderComponent.h"
#include "asserts.h"

class CMeshRender :
    public CRenderComponent
{
public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override {}
public:
    CMeshRender();
    virtual ~CMeshRender();
};

