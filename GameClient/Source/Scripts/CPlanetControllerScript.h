#pragma once
#include "CScript.h"
class CPlanetControllerScript :
    public CScript
{
private:
    float m_distanceFromSun;
    float m_offsetFromParent;
    float m_fOrbitAngle;   // 현재 공전 각도 (누적)
    float m_fOrbitSpeed;   // 공전 속도 (입력값)
    float m_fRotationSpeed;
    float m_fSelfRotationAngle;
    bool m_bInitialized;
public:
    void Tick() override;
    GET_SET(float, distanceFromSun);
    GET_SET(float, offsetFromParent);
    GET_SET(float, fOrbitAngle);
    GET_SET(float, fOrbitSpeed);

    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}

    CLONE(CPlanetControllerScript);
public:
    CPlanetControllerScript();
    ~CPlanetControllerScript();
};

