#pragma once
#include "Entity.h"
#include "components.h"
#include "Component.h"
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
    // GameObject 본인이 속한 Layer, Index,
    // -1 인 경우, 어떤 레이어에도 속하지 않는다 == 레벨안에 있지 않은 오브젝트
    int                         m_LayerIdx;
    bool                        m_Dead;

    TrileMeta                   m_trileMeta;
    bool                        m_Active;
public:
    // 레벨이 처음 시작될때 호출되는 함수
    void Begin();

    // 매 프레임마다 할 일
    void Tick();

    // 매 프레임마다 Tick 이후에 뒷 수습작업 수행
    void FinalTick();
    void FinalTick_Editor();

    // 자신을 타겟에 그림
    void Render();

    int GetLayerIdx() { return m_LayerIdx; }

    void Destroy();
public:
    void AddComponent(Ptr<Component> _Com);
    Ptr<Component> GetComponent(COMPONENT_TYPE _Type) { return m_Com[(UINT)_Type]; }

    vector<Ptr<CScript>> GetScripts() { return m_vecScripts; }

    template<typename T>
    Ptr<T> GetScript();

    void AddChild(Ptr<GameObject> _Child);

    Ptr<GameObject> GetParent() { return m_Parent; }
    Ptr<GameObject> GetChild(int _idx) { return m_vecChild[_idx]; }
    const vector<Ptr<GameObject>>& GetChild() { return m_vecChild; }

    bool IsDead() { return m_Dead; }

    void DisconnectWithParent();
    void DeregisterAsParent();
    void RegisterAsParent();
    bool IsAncestor(Ptr<GameObject> target);

    void SetTrileMeta(TrileMeta _trileMeta) { m_trileMeta = _trileMeta; }
    TrileMeta& GetTrileMeta() { return m_trileMeta; }
    void SetActive(bool _Active) { m_Active = _Active; }
    bool IsActive() const { return m_Active; }
private:
    AABB GetBoundingBox();
public:
    void FitCollider2D();
public:
    //Component Create Return
    GET_COMPONENT(Clickable, CLICKABLE);
    GET_COMPONENT(Light3D, LIGHT3D);
    GET_COMPONENT(Rigidbody2D, RIGIDBODY2D);
    GET_COMPONENT(Light2D, LIGHT2D);
    GET_COMPONENT(TileRender, TILE_RENDER);
    GET_COMPONENT(FlipbookRender, FLIPBOOK_RENDER);
    GET_COMPONENT(SpriteRender, SPRITE_RENDER);
    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(BillboardRender, BILLBOARD_RENDER);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    Ptr<CRenderComponent> GetRenderCom() { return m_RenderCom; }
public:
    void SaveToLevelFile(FILE* _File);
    void LoadFromLevelFile(FILE* _File);
private:
    void RegisterLayer();

    CLONE(GameObject);
public:
    GameObject();
    // 복사 생성시 고려 할 사항이 원본이랑 같은 느낌이 필요함 그래서 이걸 같은 주소를 가리키는게 아니라
    // 아에 새 주소로 할당된 같은 객체가 필요하기 때문에
    // 구현을 안해주면 단순 복사로 주소만 복사하게됨
    // 그래서 복사생성자 직접 구현을 통해 주소가 다른 같은 객체를 만들어야함
    GameObject(const GameObject& _Origin);
    virtual ~GameObject();

    friend class Layer;
    friend class TaskMgr;
};
bool IsValid(Ptr<GameObject>& _Object);

template<typename T>
inline Ptr<T> GameObject::GetScript()
{
    for (const auto& script : m_vecScripts)
    {
        T* pScript = dynamic_cast<T*>(script.Get());
        if (nullptr == pScript) continue;

        return pScript;
    }

    return nullptr;
}
