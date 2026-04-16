#include "pch.h"
#include "CShockWaveScript.h"

#include "TimeMgr.h"
#include "CTransform.h"

CShockWaveScript::CShockWaveScript()
	: CScript(SCRIPT_TYPE::SHOCKWAVESCRIPT)
{
}

CShockWaveScript::~CShockWaveScript()
{
}

void CShockWaveScript::Begin()
{
}

void CShockWaveScript::Tick()
{
	Vec3 vScale = Transform()->GetRelativeScale();

	vScale += Vec3(1300.f * DT, 1300.f * DT, 0.f);

	if (vScale.x >= 500.f)
		vScale = Vec3(-150.f, -150.f, 1.f);

	Transform()->SetRelativeScale(vScale);
}
