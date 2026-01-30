#include "pch.h"
#include "CPlanetControllerScript.h"
#include "TimeMgr.h"
#include "LevelMgr.h"
CPlanetControllerScript::CPlanetControllerScript()
    : m_distanceFromSun(300.f) // 거리를 좀 더 크게 (눈에 띄게)
    , m_fOrbitSpeed(1.f)       // 공전 속도 1.0f
    , m_fRotationSpeed(2.f)    // 자전 속도 2.0f
    , m_fOrbitAngle(0.f)
    , m_fSelfRotationAngle(0.f)
{
}

CPlanetControllerScript::~CPlanetControllerScript()
{
}

void CPlanetControllerScript::Tick()
{
    // 1. 태양(부모) 위치 가져오기
    // 인덱스 1번이 태양이라고 가정하신 로직 유지
    Ptr<GameObject> sola = LevelMgr::GetInst()->GetLevel()->GetLayer(0)->GetVecObject()[1];
    Vec3 solaPos = sola->Transform()->GetRelativePos();

    if (!m_bInitialized)
    {
        Vec3 myPos = Transform()->GetRelativePos();
        Vec3 diff = myPos - solaPos;

        // 현재 배치된 곳까지의 거리를 궤도 반지름으로 설정
        m_distanceFromSun = sqrtf(diff.x * diff.x + diff.z * diff.z);

        // 현재 배치된 방향을 시작 각도로 설정 (atan2f 사용)
        m_fOrbitAngle = atan2f(diff.z, diff.x);

        m_bInitialized = true; // 다음 틱부터는 실행 안 됨
    }

    // 2. 시간(DT)에 따라 멤버 변수인 각도를 업데이트
    // fAngle이 멤버 변수여야 매 프레임 이어서 계산됩니다.
    m_fOrbitAngle += DT * m_fOrbitSpeed;

    float fSemiMajor = m_distanceFromSun;        // 가로 반지름
    float fSemiMinor = m_distanceFromSun * 0.8f; // 세로 반지름 (약간 찌그러진 타원)

    // 3. 삼각함수로 궤도 위치 계산 (X, Z 평면 공전)
    Vec3 vNewPos;
    vNewPos.x = solaPos.x + cosf(m_fOrbitAngle) * fSemiMajor;
    vNewPos.z = solaPos.z + sinf(m_fOrbitAngle) * fSemiMinor;
    vNewPos.y = solaPos.y; // 태양의 높이에 맞춤

    // 1. 자전 각도 업데이트 (멤버 변수 m_fSelfRotationAngle 추가 추천)
    m_fSelfRotationAngle += DT * m_fRotationSpeed;

    // 2. 위치 설정
    Transform()->SetRelativePos(vNewPos);

    // 3. 회전 설정 (Y축 중심으로 자전)
    // 기존의 회전값에 자전 각도를 적용합니다.
    Vec3 vRot = Transform()->GetRelativeRot();
    vRot.y = m_fSelfRotationAngle;

    // 4. 최종 위치 설정
    Transform()->SetRelativeRot(vRot);
    Transform()->SetRelativePos(vNewPos);
}