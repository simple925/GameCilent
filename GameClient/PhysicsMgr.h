#pragma once
#include <box2d/box2d.h>
#include "CRigidbody2D.h"
class PhysicsMgr
	: public singleton<PhysicsMgr>
{
    SINGLE(PhysicsMgr);
private:
    b2WorldId m_World;
    vector<Ptr<CRigidbody2D>> m_vecRigidbodies;
    vector<CCollider2D*> m_vecDirtyColliders;
    GameObject* m_Target;
public:
    void Init();
    void Step();

    void RegisterRigidbody(Ptr<CRigidbody2D> rb);
    void RequestShapeRecreate(CCollider2D* col);
    void UnregisterRigidbody(Ptr<CRigidbody2D> rb);

    static bool OnPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void* context);

    b2WorldId GetWorld() { return m_World; }
};

