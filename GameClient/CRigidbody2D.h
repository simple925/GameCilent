#pragma once
#include "Component.h"
#include "PhysicsUtil.h"
class CRigidbody2D :
    public Component
{
private:
    b2BodyId m_Body;
    BODY_TYPE m_Type;
public:
    void CreateBody();
    b2BodyId GetBody() { return m_Body; }
    void SetType(BODY_TYPE type) { m_Type = type; }
    void AddForce(Vec2 force);

    void SyncTransform();
    void SetPositionFromTransform();
public:
    void Begin() override;
    void FinalTick() override;
    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    CLONE(CRigidbody2D);

public:
    CRigidbody2D();
    CRigidbody2D(const CRigidbody2D& _Origin);
    virtual ~CRigidbody2D();
};

