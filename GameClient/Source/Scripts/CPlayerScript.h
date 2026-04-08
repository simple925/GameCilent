#pragma once
#include "CScript.h"

enum class PLAYER_STATE
{
	IDLE,
	MOVE,
	DASH,
};

struct DashInfo
{
	bool    bDashing = false;

	Vec3    vDir = Vec3(0.f, 0.f, 0.f);

	float   fDashTime = 0.f;
	float   fDashDuration = 0.2f;

	float   fDashSpeed = 1200.f;

	float   fCoolTime = 0.f;
	float   fCoolDuration = 1.f;

	float   fAfterImageAcc = 0.f;
	float   fAfterImageInterval = 0.03f;

};


class CPlayerScript :
	public CScript
{
private:
	Ptr<GameObject>     m_Target;
	Ptr<GameObject>		m_Light;
	int					m_iPrevDir;
	float				m_fLightAngle;
	float				m_Speed;

	Ptr<ATexture>        m_Tex;
	Ptr<APrefab>         m_Missile;

	float               m_Limit;

	int m_iNextDir;
private:
	void Move();
	void Shoot();
private:
public:
	virtual void Init() override;
	virtual void Begin() override;
	virtual void Tick() override;
	bool IsMouseOver();
	void SetTarget(Ptr<GameObject> _Target) { m_Target = _Target; }
	void SetLight(Ptr<GameObject> _Light) { m_Light = _Light; }

	// 저장 불러오기
	virtual void SaveToLevelFile(FILE* _File) override;
	virtual void LoadFromLevelFile(FILE* _File) override;

	CLONE(CPlayerScript);
public:
	CPlayerScript();
	~CPlayerScript();
};




