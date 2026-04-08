#include "pch.h"
#include "CRigidbody2D.h"
#include "PhysicsMgr.h"
CRigidbody2D::CRigidbody2D()
	: Component(COMPONENT_TYPE::RIGIDBODY2D)
{
}

CRigidbody2D::CRigidbody2D(const CRigidbody2D& _Origin)
    : Component(_Origin)
    , m_Body(b2_nullBodyId)
    , m_Type(_Origin.m_Type)
{
}

CRigidbody2D::~CRigidbody2D()
{
    if (B2_IS_NON_NULL(m_Body))
    {
        if (B2_IS_NON_NULL(PhysicsMgr::GetInst()->GetWorld()))
        {
            b2DestroyBody(m_Body);
        }
        m_Body = b2_nullBodyId;
    }
}

void CRigidbody2D::CreateBody()
{
    b2BodyDef def = b2DefaultBodyDef();

    Vec3 pos = Transform()->GetRelativePos();
    def.position = Phys::ToPhys(pos);
    def.userData = this;
    switch (m_Type)
    {
    case BODY_TYPE::DYNAMIC:
        def.type = b2_dynamicBody;
        break;
    case BODY_TYPE::STATIC:
        def.type = b2_staticBody;
        break;
    case BODY_TYPE::KINEMATIC:
        def.type = b2_kinematicBody;
        break;
    }

    def.motionLocks.angularZ = true;
    def.motionLocks.linearX = false; // 명시적으로 false
    def.motionLocks.linearY = false; // ← 이게 잠겨있을 수 있음
    m_Body = b2CreateBody(PhysicsMgr::GetInst()->GetWorld(), &def);
}



void CRigidbody2D::Begin()
{
    CreateBody();

    SetPositionFromTransform();

    PhysicsMgr::GetInst()->RegisterRigidbody(this);
}

/*
    자주 쓰이는 함수
    b2Vec2 pos = b2Body_GetPosition(body);          : 위치
    b2Transform tf = b2Body_GetTransform(body);     : Transform
    float angle = b2Rot_GetAngle(tf.q);             : 각도
    b2Vec2 vel = b2Body_GetLinearVelocity(body);    : 속도
*/
void CRigidbody2D::FinalTick()
{
    if (B2_IS_NULL(m_Body)) return;

    b2Transform tf = b2Body_GetTransform(m_Body);

    b2Vec2 pos = tf.p;

    Vec3 worldPos = Phys::ToWorld(pos);

    Transform()->SetRelativePos(Vec3(worldPos.x, worldPos.y, Transform()->GetRelativePos().z));
}



void CRigidbody2D::AddForce(Vec2 _force)
{
    b2Vec2 force(_force.x, _force.y);
    b2Body_ApplyLinearImpulseToCenter(m_Body, force, true);
}


void CRigidbody2D::SyncTransform() { 
    if (B2_IS_NULL(m_Body))
        return;

    b2Transform tf = b2Body_GetTransform(m_Body);

    Vec3 worldPos = Phys::ToWorld(tf.p);

    float angle = b2Rot_GetAngle(tf.q);

    Transform()->SetRelativePos(worldPos);
    Transform()->SetRelativeRot(Vec3(0.f, 0.f, angle));
}

void CRigidbody2D::SetPositionFromTransform()
{
    if (B2_IS_NULL(m_Body))
        return;

    Vec3 pos = Transform()->GetRelativePos();
    //DebugPrint(L"SetPositionFromTransform: %f, %f\n", pos.x, pos.y);

    b2Body_SetTransform(
        m_Body,
        Phys::ToPhys(Transform()->GetRelativePos()),
        b2MakeRot(Transform()->GetRelativeRot().z)
    );
}

void CRigidbody2D::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Type, sizeof(BODY_TYPE), 1, _File);
}

void CRigidbody2D::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Type, sizeof(BODY_TYPE), 1, _File);
}