#include "pch.h"
#include "Source\\Scripts\\CCamMoveScript.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "CTransform.h"
CCamMoveScript::CCamMoveScript()
	: CScript(SCRIPT_TYPE::CAMMOVESCRIPT)
	, m_Target(nullptr)
	, m_fTargetRotY(0.f)
	, m_bIsRotating(false)
{
}

CCamMoveScript::~CCamMoveScript()
{
}

void CCamMoveScript::Tick()
{
	if (PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
		// MovePerspective()
		// 타겟이 지정되어 있다면 추적/공전 모드, 없으면 자유 이동 모드
		if (nullptr != m_Target)
			MoveOrbit();
		else
			MoveFree();
	else
		MoveOrthographic();
	
}

void CCamMoveScript::MoveOrthographic()
{
	Vec3 vPos = Transform()->GetRelativePos();

	if (nullptr != m_Target)
	{
		vPos = m_Target->Transform()->GetRelativePos();
	}
	else {
		if (KEY_PRESSED(KEY::W))
			vPos.y += DT * 100.f;
		if (KEY_PRESSED(KEY::S))
			vPos.y -= DT * 100.f;
		if (KEY_PRESSED(KEY::A))
			vPos.x -= DT * 100.f;
		if (KEY_PRESSED(KEY::D))
			vPos.x += DT * 100.f;
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(Vec3(0.f, 0.f, 0.f));
}

void CCamMoveScript::MoveOrbit()
{
	if (nullptr == Transform()) return;
	Vec3 vRot = Transform()->GetRelativeRot();
	Vec3 vTargetPos = m_Target->Transform()->GetWorldPos();
	Vec3 vCurPos = Transform()->GetRelativePos();

	//if (KEY_TAP(KEY::F1)) m_fTargetRotY += XM_PIDIV2;
	//if (KEY_TAP(KEY::F2)) m_fTargetRotY -= XM_PIDIV2;

	// 마우스 우클릭으로 궤도 수정 시 목표값 갱신
	//if (KEY_PRESSED(KEY::M_RBUTTON))
	//{
		//Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
		//m_fTargetRotY += vMouseDir.x * DT * XM_2PI;
	//}

	
	// 거리 및 휠 줌 기능
	float fDist = Camera()->GetfOrbitDist();
	//if (1 == KeyMgr::GetInst()->GetWheel()) fDist -= 100.f;
	//if (-1 == KeyMgr::GetInst()->GetWheel()) fDist += 100.f;
	//Camera()->SetfOrbitDist(fDist);

	// 구면 좌표계 위치 계산
	Vec3 vPos;
	vPos.x = vTargetPos.x + sinf(m_fTargetRotY) * fDist * cosf(vRot.x);
	vPos.y = vTargetPos.y - sinf(vRot.x) * fDist;
	vPos.z = vTargetPos.z - cosf(m_fTargetRotY) * fDist * cosf(vRot.x);

	// 위치 보간 부드럽게 움직이도록
	Vec3 vSmoothPos = Vec3(
		XMVectorLerp(XMLoadFloat3(&vCurPos), XMLoadFloat3(&vPos), 10.f * DT)
	);

	// LookAt용 방향 계산 (보간된 위치 기준!)
	Vec3 vDir = vTargetPos - vSmoothPos;
	vDir.Normalize();

	float yaw = atan2f(vDir.x, vDir.z);
	float pitch = -asinf(vDir.y);

	Transform()->SetRelativeRot(Vec3(pitch, yaw, 0.f));


	

	Transform()->SetRelativePos(vSmoothPos);
}

void CCamMoveScript::MoveFree()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();

	Vec3 vFront = Transform()->GetDir(DIR::FRONT);
	Vec3 vUp = Transform()->GetDir(DIR::UP);
	Vec3 vRight = Transform()->GetDir(DIR::RIGHT);

	float moveSpeed = 100.f;

	if (1 == KeyMgr::GetInst()->GetWheel())
		vPos += vFront * 100.f;
	if (-1 == KeyMgr::GetInst()->GetWheel())
		vPos -= vFront * 100.f;

	if (KEY_PRESSED(KEY::W))
		vPos += vFront * moveSpeed * DT;
	if (KEY_PRESSED(KEY::S))
		vPos -= vFront * moveSpeed * DT;
	if (KEY_PRESSED(KEY::A))
		vPos -= vRight * moveSpeed * DT;
	if (KEY_PRESSED(KEY::D))
		vPos += vRight * moveSpeed * DT;
	if (KEY_PRESSED(KEY::Q))
		vRot.y -= moveSpeed * DT;
	if(KEY_PRESSED(KEY::E))
		vRot.y += moveSpeed * DT;


	if (KEY_PRESSED(KEY::M_RBUTTON))
	{
		Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
		vRot.y += vMouseDir.x * DT * XM_2PI * 1.f;
		vRot.x += vMouseDir.y * DT * XM_2PI * 1.f;
	}
	if (KEY_PRESSED(KEY::M_MBUTTON))
	{
		Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();

		// 마우스가 왼쪽으로 가면 카메라는 오른쪽 벡터의 반대방향으로 이동해야 화면이 밀립니다.
		// 마우스 이동량에 적절한 가중치(여기서는 500.f)를 곱해 속도를 조절하세요.
		vPos -= vRight * vMouseDir.x * DT * 100.f;
		vPos += vUp * vMouseDir.y * DT * 100.f;
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
