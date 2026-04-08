#pragma once
#include "AssetUI.h"
class MeshUI :
    public AssetUI
{
private:
    bool        m_Wireframe = false;
public:
    virtual void Tick_UI() override;
    void RenderPreview();
public:
    MeshUI();
    virtual ~MeshUI();
};

