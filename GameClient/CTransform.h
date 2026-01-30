#pragma once
#include "Component.h"
class CTransform :
    public Component
{
private:
    Vec3    m_RelativePos;
    Vec3    m_RelativeScale;
    Vec3    m_RelativeRot;

    // 단위벡터(길이가 1)
    Vec3    m_Dir[(UINT)DIR::END]; // 방향 벡터

    Matrix  m_matWorld;

    bool    m_IndependentScale; // 부모 오브젝트의 크기는 무시
public:
    // 세팅된 묶어주는 역할
    virtual void FinalTick() override;
    // 데이터를 GPU 메모리로 전송
    void Binding();

    void SetRelativePos(Vec3 _Pos){ m_RelativePos = _Pos ;}
    void SetRelativeScale(Vec3 _Scale) { m_RelativeScale = _Scale;}
    void SetRelativeRot(Vec3 _Rotation) { m_RelativeRot = _Rotation; }

    Vec3 GetRelativePos(){ return m_RelativePos;}
    Vec3 GetRelativeScale() { return m_RelativeScale;}
    Vec3 GetRelativeRot() { return m_RelativeRot; }

    // Translation() 이동정보를 뽑아옴 _41 _42 _43
    Vec3 GetWorldPos() { return m_matWorld.Translation(); }

    void SetIndependentScale(bool _On) { m_IndependentScale = !m_IndependentScale; }

    Vec3 GetDir(DIR _Type) { return m_Dir[(UINT)_Type]; }

    const Matrix& GetWorldMat() { return m_matWorld; }

    Vec3 GetWorldScale();
public:
    CTransform();
    virtual ~CTransform();
};

