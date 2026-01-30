#include "pch.h"
#include "CPlayerProjectile.h"
#include "CTransform.h"

#include "TimeMgr.h"

#include "GameObject.h"

void CPlayerProjectile::Tick()
{
	// 이동 
	Vec3 vPos = Transform()->GetRelativePos();
	vPos += Transform()->GetDir(DIR::UP) 
		  * m_fSpeed 
		  * DT;
	Transform()->SetRelativePos(vPos);
	// 풀에 반납 (예: z축 기준)
	if (vPos.y > 800.f || vPos.y < -800.f || vPos.x > 800.f || vPos.x < -800.f) {
		GetOwner()->Hide(); // 위치는 발사 시점에 다시 세팅 
	}
}

CPlayerProjectile::CPlayerProjectile()
	: m_fSpeed(500.f)
{
}

CPlayerProjectile::~CPlayerProjectile()
{
}
