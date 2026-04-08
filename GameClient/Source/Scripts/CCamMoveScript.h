#pragma once
#include "CScript.h"
class CCamMoveScript
	: public CScript
{
private:
	Ptr<GameObject> m_Target;
	float m_fTargetRotY;
	bool m_bIsRotating;
public:
	void SetTarget(Ptr<GameObject> _Target) { m_Target = _Target; }
public:
	virtual void Tick() override;
private:
	//void MovePerspective();
	void MoveOrthographic();
	void MoveOrbit();
	void MoveFree();
	// 저장 불러오기
	virtual void SaveToLevelFile(FILE* _File) override {}
	virtual void LoadFromLevelFile(FILE* _File) override {}

	CLONE(CCamMoveScript);
public:
	CCamMoveScript();
	~CCamMoveScript();
};

