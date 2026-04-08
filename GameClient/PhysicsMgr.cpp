#include "pch.h"
#include "PhysicsMgr.h"
#include "TimeMgr.h"
#include "CRigidbody2D.h"
#include "LevelMgr.h"
PhysicsMgr::PhysicsMgr()
    : m_World(b2_nullWorldId)
{

}
PhysicsMgr::~PhysicsMgr()
{
    if (B2_IS_NON_NULL(m_World))
    {
        b2DestroyWorld(m_World);
        // double destroy
        // dangling handle
        // 방지용
        m_World = b2_nullWorldId;
    }
}

void PhysicsMgr::Init()
{
    b2WorldDef def = b2DefaultWorldDef();
    def.gravity = b2Vec2(0.0f, -9.8f);
    m_World = b2CreateWorld(&def);

    // 🔥 PreSolve 콜백 등록
    b2World_SetPreSolveCallback(m_World, OnPreSolve, nullptr);
}

void PhysicsMgr::Step()
{
    if (B2_IS_NULL(m_World)) return;

    b2World_Step(m_World, DT, 8);

    for (auto rb : m_vecRigidbodies)
    {
        if(rb) rb->SyncTransform();
    }
    for (auto col : m_vecDirtyColliders)
    {
        if (col)
        {
            col->DestroyShape();
            col->CreateShape();
        }
    }
    m_vecDirtyColliders.clear();
}

void PhysicsMgr::RegisterRigidbody(Ptr<CRigidbody2D> rb)
{
    if (!rb) return;

    m_vecRigidbodies.push_back(rb);
}

void PhysicsMgr::RequestShapeRecreate(CCollider2D* col)
{
    if (col)m_vecDirtyColliders.push_back(col);
}

void PhysicsMgr::UnregisterRigidbody(Ptr<CRigidbody2D> rb)
{
    if (!rb) return;

    m_vecRigidbodies.erase(
        std::remove(m_vecRigidbodies.begin(), m_vecRigidbodies.end(), rb),
        m_vecRigidbodies.end()
    );
}

bool PhysicsMgr::OnPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void* context)
{
    CCollider2D* colA = (CCollider2D*)b2Shape_GetUserData(shapeIdA);
    CCollider2D* colB = (CCollider2D*)b2Shape_GetUserData(shapeIdB);

    if (!colA || !colB) return true;

    CCollider2D* oneWayCol = nullptr;
    CCollider2D* otherCol = nullptr;

    if (colA->IsOneWay()) { oneWayCol = colA; otherCol = colB; }
    else if (colB->IsOneWay()) { oneWayCol = colB; otherCol = colA; }
    else return true; // 둘 다 OneWay 아니면 정상 충돌

    // 🔥 normal이 위를 향하는지 확인 (OneWay 플랫폼 윗면)
    // colB가 OneWay면 법선 방향 반전
    b2Vec2 n = (oneWayCol == colB) ? b2Vec2{ -normal.x, -normal.y } : normal;

    if (n.y <= 0.f) return false; // 아래에서 올라오는 충돌 무시

    // 🔥 상대방이 위에서 내려오는지 확인
    b2BodyId body = b2Shape_GetBody(otherCol->GetShape());
    b2Vec2 vel = b2Body_GetLinearVelocity(body);

    if (vel.y > 0.f) return false; // 위로 올라가는 중이면 무시 (점프 통과)

    return true; // 충돌 허용 (위에서 내려오는 중)
}
