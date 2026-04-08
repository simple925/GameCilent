#pragma once
#include "Component.h"
class CLight3D :
    public Component
{
private:
    Light3DInfo     m_Info; // 아까 정의한 3D용 구조체
public:
    void FinalTick() override;
public:
    const Light3DInfo& GetInfo() { return m_Info; }

    // 광원 타입 (Directional, Point, Spot)
    void SetLightType(int _Type) { m_Info.Type = _Type; }
    int GetLightType() { return m_Info.Type; }

    // 1. Diffuse (기본 색상)
    void SetLightColor(Vec4 _Color) { m_Info.Color = _Color; }
    Vec4 GetLightColor() { return m_Info.Color; }

    // 2. Ambient (환경광)
    void SetAmbient(Vec4 _Ambient) { m_Info.Ambient = _Ambient; }
    Vec4 GetAmbient() { return m_Info.Ambient; }

    // 3. Specular (반사광 - 3D 핵심)
    void SetSpecular(Vec4 _Spec) { m_Info.Specular = _Spec; }
    Vec4 GetSpecular() { return m_Info.Specular; }

    // 4. 범위 및 각도
    void SetRadius(float _R) { m_Info.Radius = _R; }
    float GetRadius() { return m_Info.Radius; }

    void SetAngle(float _Angle) { m_Info.Angle = _Angle; }
    float GetAngle() { return m_Info.Angle; }

    // 저장 및 불러오기
    virtual void SaveToLevelFile(FILE* _File);
    virtual void LoadFromLevelFile(FILE* _File);

    CLONE(CLight3D);

public:
    CLight3D();
    virtual ~CLight3D();
};

