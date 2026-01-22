#pragma once
#include "Component.h"
#include "AMesh.h"
#include "AGraphicShader.h"
#include "ATexture.h"

class CMeshRender :
    public Component
{
private:
    Ptr<AMesh>  m_Mesh;
    Ptr<AGraphicShader> m_Shader;
    Ptr<ATexture>       m_Tex;
public:
    void SetMesh(Ptr<AMesh> _Mesh) { m_Mesh = _Mesh; }
    void SetShader(Ptr<AGraphicShader> _Shader) { m_Shader = _Shader; }
    void SetTexture(Ptr<ATexture> _Tex) { m_Tex = _Tex; }
public:
    virtual void FinalTick() override;
    void Render();
public:
    CMeshRender();
    virtual ~CMeshRender();
};

