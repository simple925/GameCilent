#pragma once
#include "Component.h"
#include "asserts.h"

class CMeshRender :
    public Component
{
private:
    Ptr<AMesh>  m_Mesh;
    Ptr<AMaterial> m_Material;
public:
    void SetMesh(Ptr<AMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetMaterial(Ptr<AMaterial> _Material) { m_Material = _Material; }

    Ptr<AMesh>  GetMesh() { return m_Mesh; }
    Ptr<AMaterial> GetMaterial() { return m_Material; }
public:
    virtual void FinalTick() override;
    void Render();
public:
    CMeshRender();
    virtual ~CMeshRender();
};

