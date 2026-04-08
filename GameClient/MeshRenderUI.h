#pragma once
#include "ComponentUI.h"
class MeshRenderUI :
    public ComponentUI
{
private:
    void SelectMesh(DWORD_PTR _ListUI);
    void SelectMtrl(DWORD_PTR _ListUI);
public:
    virtual void Tick_UI();
    MeshRenderUI();
    virtual ~MeshRenderUI();
};

