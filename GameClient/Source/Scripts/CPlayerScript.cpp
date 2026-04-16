#include "pch.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"

#include "CTransform.h"
#include "GameObject.h"
#include "Source\\Scripts\\CPlayerScript.h"
#include "Source\\Scripts\\CMissileScript.h"

#include "AssetMgr.h"
#include "LevelMgr.h"
#include "TaskMgr.h"



CPlayerScript::CPlayerScript()
	: CScript(SCRIPT_TYPE::PLAYERSCRIPT)
	, m_iPrevDir(-1)
	, m_fLightAngle(180.f)
	, m_Speed(500.f)
	, m_iNextDir(-1)
{
}

CPlayerScript::~CPlayerScript()
{
}

// 초기화
// AddComponent
void CPlayerScript::Init()
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"PlayerSpeed", true, 0.f);
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Limit, L"Limit", true, 0.f);
	AddScriptParam(SCRIPT_PARAM::PREFAB, &m_Missile, L"Missile", true, 0.f);
	AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_Tex, L"ChangeTex");
}

bool CPlayerScript::IsMouseOver()
{
	// 1. 마우스 NDC 좌표 가져오기
	POINT pt;
	GetCursorPos(&pt);
	//ScreenToClient(, &pt);

	float width = (float)Device::GetInst()->GetRenderResolution().x;
	float height = (float)Device::GetInst()->GetRenderResolution().y;

	float x = (pt.x / width) * 2.f - 1.f;
	float y = 1.f - (pt.y / height) * 2.f;
	Vec2 mouseNDC = Vec2(x, y);

	// 2. 자신의 Transform 정보 가져오기
	Ptr<CTransform> trans = GetOwner()->Transform();
	Vec3 vPos = trans->GetRelativePos();
	Vec3 vScale = trans->GetRelativeScale();

	// 3. 충돌 판정 (AABB 기준)
	// 좀 더 정밀하게 하고 싶다면 여기서 m_pMesh 정보를 받아와 메쉬 타입별로 계산 가능
	if (mouseNDC.x >= vPos.x - vScale.x / 2.f && mouseNDC.x <= vPos.x + vScale.x / 2.f &&
		mouseNDC.y >= vPos.y - vScale.y / 2.f && mouseNDC.y <= vPos.y + vScale.y / 2.f)
	{
		return true;
	}

	return false;
}

void CPlayerScript::Move()
{
	Ptr<CRigidbody2D> rb = Rigidbody2D();
	if (!rb) return;

	b2BodyId body = rb->GetBody();

	float velX = 0.f;

	if (KEY_PRESSED(KEY::RIGHT))
	{
		velX = Phys::ToPhys(m_Speed);
		m_iNextDir = 3;
		m_fLightAngle = 0.f;
	}
	else if (KEY_PRESSED(KEY::LEFT))
	{
		velX = -Phys::ToPhys(m_Speed);
		m_iNextDir = 1;
		m_fLightAngle = 180.f;
	}

	b2Vec2 vel = b2Body_GetLinearVelocity(body);

	vel.x = velX;

	if (KEY_TAP(KEY::UP))
	{
		vel.y = Phys::ToPhys(500.f); // ← 단위 변환 추가
	}

	b2Body_SetLinearVelocity(body, vel);

	// 2. 애니메이션 제어
	if (m_iNextDir != -1) // 키가 눌려 있는 경우
	{
		// 방향이 바뀌었을 때만 Play 호출 (인덱스는 iNextDir 사용)
		if (m_iNextDir != m_iPrevDir)
		{
			// 세 번째 인자가 -1이면 무한 반복, 1이면 한 번 재생이라고 하셨죠?
			GetOwner()->FlipbookRender()->Play(m_iNextDir, m_Speed, -1);
			m_iPrevDir = m_iNextDir;
		}
	}
	else // 키를 뗀 경우
	{
		// 이전에 어딘가로 움직이고 있었다면 (m_iPrevDir가 0~3 사이라면)
		if (m_iPrevDir != -1)
		{
			// 마지막으로 바라보던 방향(m_iPrevDir)으로 멈춤(1) 처리
			GetOwner()->FlipbookRender()->Play(m_iPrevDir, m_Speed, 0);
			m_iPrevDir = -1; // 이후엔 다시 키를 누르기 전까지 이 블록 안 들어옴
		}
	}

	if (m_Light != nullptr)
	{
		Vec3 vRot = m_Light->Transform()->GetRelativeRot();
		vRot.z = XMConvertToRadians(m_fLightAngle); // 라디안 변환 필수
		m_Light->Transform()->SetRelativeRot(vRot);
	}

	//Transform()->SetRelativePos(vPos);
	//Transform()->SetRelativeScale(vScale);
	//Transform()->SetRelativeRot(vRotation);
}

void CPlayerScript::Shoot()
{
	if (KEY_TAP(KEY::SPACE))
	{
		// 미사일 역할의 오브젝트 생성
		/*
		GameObject* pObject = new GameObject;
		pObject->SetName(L"Missile");

		pObject->AddComponent(new CTransform);
		pObject->AddComponent(new CMeshRender);
		pObject->AddComponent(new CCollider2D);

		Ptr<CMissileScript> pMissileScript = new CMissileScript;
		pMissileScript->SetTarget(m_Target.Get());
		pObject->AddComponent(pMissileScript.Get());

		Vec3 vMyPos = Transform()->GetRelativePos();
		Vec3 vMyScale = Transform()->GetRelativeScale();
		Vec3 vRotation = Transform()->GetRelativeRot();
		Vec3 vUp = Transform()->GetDir(DIR::UP);

		pObject->Transform()->SetRelativePos(vMyPos + vMyScale * 0.5f * vUp);
		pObject->Transform()->SetRelativeScale(Vec3(10.f, 30.f, 1.f));
		pObject->Transform()->SetRelativeRot(vRotation);

		pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
 		pObject->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

		CreateObject(pObject, 4);
		*/
		Vec3 vMyPos = Transform()->GetRelativePos();
		Vec3 vMyScale = Transform()->GetRelativeScale();
		Vec3 vRotation = Transform()->GetRelativeRot();
		Vec3 vUp = Transform()->GetDir(DIR::UP);

		Instantiate(m_Missile.Get(), 4, vMyPos + vMyScale * 0.5f * vUp);
	}
	if (KEY_TAP(KEY::Z)) {
		DrawDebugCircle(Transform()->GetRelativePos(), 100.f, Vec4(1.f, 0.f, 0.f, 1.f), 2.f);
	}

	if (KEY_TAP(KEY::X)) {
		Destroy();
	}
	
}


// 레벨이 시작할때
void CPlayerScript::Begin()
{
	// 사운드 재생 테스트
	Ptr<ASound> pSound = LOAD(ASound, L"Sound\\DM.wav");

	pSound->Play(0, 0.5f, false);
}

void CPlayerScript::Tick()
{
	Move();
	Shoot();

	//Ptr<GameObject> pChild = GetOwner()->GetChild(0);

	//Vec3 vRelativePos = pChild->Transform()->GetRelativePos();
	//Vec3 vWorldPos = pChild->Transform()->GetWorldPos();
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
}