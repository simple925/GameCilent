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
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();
	

	Vec3 vFront = Transform()->GetDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetDir(DIR::RIGHT);

	

	float moveSpeed = 800.f;
	if (1 == KeyMgr::GetInst()->GetWheel())
		vPos += vFront * 10.f;
	if (-1 == KeyMgr::GetInst()->GetWheel())
		vPos -= vFront * 10.f;

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * moveSpeed * DT;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * moveSpeed * DT;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * moveSpeed * DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * moveSpeed * DT;


	
	if (KEY_PRESSED(KEY::M_RBUTTON))
	{
		Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
		vRot.y += vMouseDir.x * DT * XM_2PI * 3.f;
		vRot.x += vMouseDir.y * DT * XM_2PI * 3.f;
	}
	if (KEY_PRESSED(KEY::M_MBUTTON))
	{

	}
	
	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(vRot);
}

// 정점쉐이더 버텍스쉐이더
// 레스터라이져 정점정보를 갖고 픽셀 쉐이더를 호출함, 보간함, 컬링모드 작업 컬링모드: 시계방향으로 접근해야 됨 = 정면, 반시계 방향은 안그려줌
// 기본모드가 컬 백 모드임 뒷면은 반시계로 약속됨
// 모델좌표를 
// 물체위치를 월드행렬로
// 윌드 정점을 뷰행렬로
// 뷰 카메라가 보는 기준으로 옮김 뷰행렬때문에 뒷면이 안보임
// 담기위해 원근을위해 투영
// w로 나눠서 NDC 좌표계