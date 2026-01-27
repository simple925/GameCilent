#include "pch.h"
#include "CCameraMoveScript.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "CTransform.h"
CCamraMoveScript::CCamraMoveScript()
{
}

CCamraMoveScript::~CCamraMoveScript()
{
}

void CCamraMoveScript::Tick()
{
	Vec3 vPos = Transform()->GetPos();
	Vec3 vRot = Transform()->GetRotation();
	if (KEY_PRESSED(KEY::W))
		vPos.y += DT * 1000.f;
	if (KEY_PRESSED(KEY::S))
		vPos.y -= DT * 1000.f;
	if (KEY_PRESSED(KEY::A))
		vPos.x -= DT * 1000.f;
	if (KEY_PRESSED(KEY::D))
		vPos.x += DT * 1000.f;
	if (KEY_PRESSED(KEY::Z))
		vPos.z -= DT * 600.f;
	if (KEY_PRESSED(KEY::X))
		vPos.z += DT * 600.f;

	if (KEY_PRESSED(KEY::Q))
		vRot.y -= XM_PI * DT;
	if (KEY_PRESSED(KEY::E))
		vRot.y += XM_PI * DT;
	if (KEY_PRESSED(KEY::R))
		vRot.x -= XM_PI * DT;
	if (KEY_PRESSED(KEY::F))
		vRot.x += XM_PI * DT;
	if (KEY_PRESSED(KEY::C))
		vPos.z -= XM_PI * DT;
	if (KEY_PRESSED(KEY::V))
		vPos.z += XM_PI * DT;
	if (KEY_PRESSED(KEY::M_RBUTTON)) {
		int a = 0;
		a++;
	}
	
	
	Transform()->SetPos(vPos);
	Transform()->SetRotation(vRot);
}