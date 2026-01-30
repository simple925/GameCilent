#pragma once
#include "Entity.h"
#include "components.h"

#define GET_COMPONENT(COM_NAME, COM_TYPE)     Ptr<C##COM_NAME> COM_NAME() { return (C##COM_NAME*)m_Com[(UINT)COMPONENT_TYPE::COM_TYPE].Get(); }

class GameObject :
    public Entity
{
private:
    Ptr<Component>              m_Com[(UINT)COMPONENT_TYPE::END];
    Ptr<CRenderComponent>       m_RenderCom;
    vector<Ptr<CScript>>        m_vecScripts;

    // 종속관계에서는 스마트 포인터를 쓰면 삭제 되지 않는다!!!!! 영원히
    GameObject*                 m_Parent; // 이건 부모 객체를 가리키기 때문에 스마트 포인터 인씀
    vector<Ptr<GameObject>>     m_vecChild;
    bool                        m_bIsHidden;
public:
    // 레벨이 처음 시작될때 호출되는 함수
    void Begin();

    // 매 프레임마다 할 일
    void Tick();

    // 매 프레임마다 Tick 이후에 뒷 수습작업 수행
    void FinalTick();

    // 자신을 타겟에 그림
    void Render();

    bool IsHidden() const { return m_bIsHidden; }
    void Hide() { m_bIsHidden = true; }
    void Show() { m_bIsHidden = false; }
public:
    void AddComponent(Ptr<Component> _Com);
    Ptr<Component> GetComponent(COMPONENT_TYPE _Type) { return m_Com[(UINT)_Type]; }

    void AddChild(Ptr<GameObject> _Child) { m_vecChild.push_back(_Child); _Child->m_Parent = this; }

    Ptr<GameObject> GetParent() { return m_Parent; }
    Ptr<GameObject> GetChild(int _idx) { return m_vecChild[_idx]; }
public:
    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(BillboardRender, BILLBOARD_RENDER);
    GET_COMPONENT(Camera, CAMERA);

    Ptr<CRenderComponent> GetRenderCom() { return m_RenderCom; }
public:
    GameObject();
    virtual ~GameObject();
};

