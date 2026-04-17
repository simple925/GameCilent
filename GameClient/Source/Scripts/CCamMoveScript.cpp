#include "pch.h"
#include "Source\\Scripts\\CCamMoveScript.h"
#include "Source\\Scripts\\CPlayerScript.h"
#include "Source\\Scripts\\CMapScript.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "LevelMgr.h"
#include "CTransform.h"
CCamMoveScript::CCamMoveScript()
	: CScript(SCRIPT_TYPE::CAMMOVESCRIPT)
	, m_Target(nullptr)
	, m_MapScript(nullptr)
	, m_PlayerScript(nullptr)
	, m_fTargetRotY(0.f)
	, m_fCurrentRotY(0.f)
	, m_bIsRotating(false)
	, m_fRotationSpeed(5.f)       // 보간 속도 (클수록 빠름)
	, m_fRotationThreshold(0.001f)
	, m_iViewAngle(0)
{
}

CCamMoveScript::~CCamMoveScript()
{
}

void CCamMoveScript::Begin()
{
	// Map 오브젝트에서 CMapScript 탐색
	Ptr<GameObject> mapObj = LevelMgr::GetInst()->GetLevel()->FindObjectByName(L"Map");
	if (mapObj)
		m_MapScript = mapObj->GetScript<CMapScript>();

	// Player 오브젝트에서 CPlayerScript 탐색
	Ptr<GameObject> playerObj = LevelMgr::GetInst()->GetLevel()->FindObjectByName(L"Player");
	if (playerObj)
		m_PlayerScript = playerObj->GetScript<CPlayerScript>();
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
	else
	{
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

	// 회전 중 추가 입력 무시
	if (KEY_TAP(KEY::F1))
	{
		m_iPrevViewAngle = m_iViewAngle;
		m_fTargetRotY += XM_PIDIV2;
		m_iViewAngle = (m_iViewAngle + 90) % 360;
		m_bIsRotating = true;
	}
	if (KEY_TAP(KEY::F2))
	{
		m_iPrevViewAngle = m_iViewAngle;
		m_fTargetRotY -= XM_PIDIV2;
		m_iViewAngle = ((m_iViewAngle - 90) % 360 + 360) % 360;
		m_bIsRotating = true;
	}

	// ── 각도 보간 ─────────────────────────────────────────
	if (m_bIsRotating)
	{
		m_fCurrentRotY = XMVectorGetX(XMVectorLerp(
			XMLoadFloat(&m_fCurrentRotY),
			XMLoadFloat(&m_fTargetRotY),
			m_fRotationSpeed * DT));

		if (fabsf(m_fTargetRotY - m_fCurrentRotY) < m_fRotationThreshold)
		{
			m_fCurrentRotY = m_fTargetRotY;
			m_bIsRotating = false;

			OnRotationCompleted();
		}
	}

	// 거리 및 휠 줌 기능
	//float fDist = Camera()->GetfOrbitDist();
	//if (1 == KeyMgr::GetInst()->GetWheel()) fDist -= 100.f;
	//if (-1 == KeyMgr::GetInst()->GetWheel()) fDist += 100.f;
	//Camera()->SetfOrbitDist(fDist);

	// ── 공전 위치 계산 (구면 좌표계) ──────────────────────
	Vec3  vRot = Transform()->GetRelativeRot();
	Vec3  vTargetPos = m_Target->Transform()->GetWorldPos();
	Vec3  vCurPos = Transform()->GetRelativePos();
	float fDist = Camera()->GetfOrbitDist();

	Vec3 vPos;
	vPos.x = vTargetPos.x + sinf(m_fCurrentRotY) * fDist * cosf(vRot.x);
	vPos.y = vTargetPos.y - sinf(vRot.x) * fDist;
	vPos.z = vTargetPos.z - cosf(m_fCurrentRotY) * fDist * cosf(vRot.x);

	Vec3 vSmoothPos = Vec3(
		XMVectorLerp(XMLoadFloat3(&vCurPos), XMLoadFloat3(&vPos), 10.f * DT)
	);

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

void CCamMoveScript::OnRotationCompleted()
{
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
