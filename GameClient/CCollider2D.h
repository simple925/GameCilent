#pragma once
#include "Component.h"

class CScript;
class CCollider2D;
typedef void(CScript::* COLLISION_EVENT)(CCollider2D*, CCollider2D*);
struct COLLISION_DELEGATE
{
    CScript* Inst; // 호출자
    COLLISION_EVENT MemFunc;
};

class CCollider2D :
    public Component
{
private:
    Vec2        m_Offset;
    Vec2        m_Scale;

    int         m_OverlapCount;
    Matrix      m_matWorld;

    // Box2D
    b2ShapeId   m_Shape;
    b2BodyId    m_Body;
    bool        m_ShapeDirty;
    bool        m_Active;

    // Collider
    bool        m_OneWay;
    bool        m_Trigger;

    vector<COLLISION_DELEGATE> m_vecBeginDel;
    vector<COLLISION_DELEGATE> m_vecOverDel;
    vector<COLLISION_DELEGATE> m_vecEndDel;
public:
    void CreateShape();
    void DestroyShape();

    b2ShapeId GetShape() { return m_Shape; }
public:
    void AddDynamicBeginOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc) { 
        m_vecBeginDel.push_back(COLLISION_DELEGATE{_Inst, _MemFunc});
    }
    void AddDynamicOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc) { m_vecOverDel.push_back(COLLISION_DELEGATE{_Inst, _MemFunc}); }
    void AddDynamicEndOverlap(CScript* _Inst, COLLISION_EVENT _MemFunc) { m_vecEndDel.push_back(COLLISION_DELEGATE{_Inst, _MemFunc}); }
public:
    void SetActive(bool active)
    {
        m_Active = active;
        m_ShapeDirty = true; // 재생성 트리거
    }
    void SetOffset(Vec2 _Offset)
    {
        m_Offset = _Offset;
        m_ShapeDirty = true;
    }
    void SetOneWay(bool oneWay) { m_OneWay = oneWay; m_ShapeDirty = true; }
    bool IsOneWay() const { return m_OneWay; }

    Vec2 GetOffset() { return m_Offset; }
    void SetScale(Vec2 _Scale)
    {
        m_Scale = _Scale;
        m_ShapeDirty = true;
    }
    Vec2 GetScale() { return m_Scale; }
    b2ShapeId GetShape() const { return m_Shape; }

    void SetTrigger(bool _b) { m_Trigger = _b; m_ShapeDirty = true; }
    bool IsTrigger() const { return m_Trigger; }
private:
    void BeginOverlap(Ptr<CCollider2D> _Other);
    void Overlap(Ptr<CCollider2D> _Other);
    void EndOverlap(Ptr<CCollider2D> _Other);
    const Matrix& GetWorldMatrix() { return m_matWorld; }
    //public: bool Contains(Vec3 worldPos);
public:
    virtual void Begin() override;
    virtual void FinalTick() override;

    virtual void SaveToLevelFile(FILE* _File);
    virtual void LoadFromLevelFile(FILE* _File);

    CLONE(CCollider2D);
public:
    CCollider2D();
    CCollider2D(const CCollider2D& _Origin);
    virtual ~CCollider2D();

    friend class CollisionMgr;
};

