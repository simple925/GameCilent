#pragma once
#include "Component.h"

#include "AMesh.h"
#include "AMaterial.h"
class CRenderComponent :
    public Component
{
private:
    Ptr<AMesh>      m_Mesh;
    Ptr<AMaterial>  m_Mtrl;

public:
    GET_SET(Ptr<AMesh>, Mesh)
    GET_SET(Ptr<AMaterial>,  Mtrl)
public:
    virtual void Begin() override;
    virtual void Render() = 0;

    // 자신이 사용할 재질 로딩 및 생성
    virtual void CreateMaterial() = 0;

public:
    CRenderComponent(COMPONENT_TYPE _Type);
    virtual ~CRenderComponent();
};

