#pragma once
#include "CRenderComponent.h"
class CBillboardRender :
    public CRenderComponent
{
private:
    Vec2            m_BillboardScale;
    Ptr<ATexture>   m_Texture;
public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;
public:
    GET_SET(Vec2, BillboardScale)
    GET_SET(Ptr<ATexture>, Texture)
public:
    CBillboardRender();
    virtual ~CBillboardRender();
};

