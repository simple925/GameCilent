#pragma once
#include "Entity.h"
#include "Component.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CScript.h"
class GameObject :
    public Entity
{
private:
    Ptr<Component> m_Com[(UINT)COMPONENT_TYPE::END];
    vector<Ptr<CScript>>        m_vecScript;
public:
    // 매 프레임마다 할 일
    void Tick();
    void FinalTick();
    void Render();
public:
    void AddComponent(Ptr<Component> _Com);
    Ptr<Component> GetComponent(COMPONENT_TYPE _Type) { return m_Com[(UINT)_Type]; }
    Ptr<CTransform> Transform() { return (CTransform*) m_Com[(UINT)COMPONENT_TYPE::TRANSFORM].Get(); }
    Ptr<CMeshRender> MeshRender() { return (CMeshRender*) m_Com[(UINT)COMPONENT_TYPE::MESHRENDER].Get(); }
public:
    GameObject();
    virtual ~GameObject();
};

