#pragma once
#include "CScript.h"

class CMapScript;
class CPlayerScript;

class CCamMoveScript
	: public CScript
{
private:
	Ptr<GameObject> m_Target;
	Ptr<CMapScript> m_MapScript;
	Ptr<CPlayerScript> m_PlayerScript;

	float			m_fTargetRotY;
	float           m_fCurrentRotY;     // 현재 보간 중인 각도
	bool            m_bIsRotating;
	float           m_fRotationSpeed;   // 보간 속도
	float           m_fRotationThreshold;

	int             m_iViewAngle;       // 현재 뷰 각도 (0 / 90 / 180 / 270)
	int             m_iPrevViewAngle;  // 이전 뷰 각도 (역변환용)
public:
	void SetTarget(Ptr<GameObject> _Target) { m_Target = _Target; }
public:
	virtual void Begin() override;
	virtual void Tick() override;
private:
	void MoveOrthographic();
	void MoveOrbit();
	void MoveFree();

	void OnRotationCompleted(); // 회전 완료 시 콜백 일괄 처리

	// 저장 불러오기
	virtual void SaveToLevelFile(FILE* _File) override {}
	virtual void LoadFromLevelFile(FILE* _File) override {}

	CLONE(CCamMoveScript);
public:
	CCamMoveScript();
	~CCamMoveScript();
};

