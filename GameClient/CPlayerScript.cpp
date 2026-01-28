#include "pch.h"
#include "CPlayerScript.h"
#include "CPlayerProjectile.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

#include "Device.h"
#include "CTransform.h"
#include "GameObject.h"

#include "ALevel.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
bool CPlayerScript::IsMouseOver()
{
	// 1. 마우스 NDC 좌표 가져오기
	POINT pt;
	GetCursorPos(&pt);
	//ScreenToClient(, &pt);

	float width = (float)Device::GetInst()->GetRenderResol().x;
	float height = (float)Device::GetInst()->GetRenderResol().y;

	float x = (pt.x / width) * 2.f - 1.f;
	float y = 1.f - (pt.y / height) * 2.f;
	Vec2 mouseNDC = Vec2(x, y);

	// 2. 자신의 Transform 정보 가져오기
	Ptr<CTransform> trans = GetOwner()->Transform();
	Vec3 vPos = trans->GetPos();
	Vec3 vScale = trans->GetScale();

	// 3. 충돌 판정 (AABB 기준)
	// 좀 더 정밀하게 하고 싶다면 여기서 m_pMesh 정보를 받아와 메쉬 타입별로 계산 가능
	if (mouseNDC.x >= vPos.x - vScale.x / 2.f && mouseNDC.x <= vPos.x + vScale.x / 2.f &&
		mouseNDC.y >= vPos.y - vScale.y / 2.f && mouseNDC.y <= vPos.y + vScale.y / 2.f)
	{
		return true;
	}

	return false;
}

CPlayerScript::CPlayerScript()
	: m_bSelected(false)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Move()
{
	/*
	// 1. 클릭 시 선택 여부 판단 (Self-Picking)
	if (KEY_TAP(KEY::LBUTTON))
	{
		bool bMouseOver = IsMouseOver(); // 마우스가 내 영역 안에 있는지 확인

		if (KEY_PRESSED(KEY::CTRL))
		{
			// [컨트롤 클릭] : 나를 클릭했다면 상태를 반전(Toggle) 시킴
			// 마우스가 없는 곳을 클릭했다면 기존 상태를 그대로 유지함
			if (bMouseOver)
			{
				m_bSelected = !m_bSelected;
			}
		}
		else
		{
			// [일반 클릭] : 컨트롤을 안 눌렀을 때
			// 마우스가 내 위에 있으면 선택, 없으면 무조건 해제
			m_bSelected = bMouseOver;
		}
	}
	*/
	// down casting
	//Ptr<CTransform> pTrans;
	//if (COMPONENT_TYPE::TRANSFORM == m_Com[(UINT)COMPONENT_TYPE::TRANSFORM]->GetType()) {
		//pTrans = (CTransform*) m_Com[(UINT)COMPONENT_TYPE::TRANSFORM].Get();
	//}

	// 실패하면 return nullptr;
	// 상속 관계에서 부모 클래스에 가장함수가 1개라도 있으면, C++ 에서는 타입 정보를 자료형 별로 생성한다.
	// 이것을 기반으로 다운캐스팅 성공 여부를 판단할 수 있다.
	//pTrans = dynamic_cast<CTransform*>(m_Com[(UINT)COMPONENT_TYPE::TRANSFORM].Get());

	//CTransform* trans = pTrans.Get();
	Vec3 vPos = Transform()->GetPos();
	Vec3 vScale = Transform()->GetScale();
	Vec3 vRotation = Transform()->GetRotation();

	Vec3 vUp = Transform()->GetDir(DIR::UP);
	Vec3 vDown = -vUp;

	if (KEY_PRESSED(KEY::UP))
		vPos += vUp * 400.5f * DT;
	if (KEY_PRESSED(KEY::DOWN))
		vPos += vDown * 400.5f * DT;

	if (KEY_PRESSED(KEY::RIGHT))
		vRotation.z -= XM_PI * DT;
	if (KEY_PRESSED(KEY::LEFT))
		vRotation.z += XM_PI * DT;

	Transform()->SetPos(vPos);
	Transform()->SetScale(vScale);
	Transform()->SetRotation(vRotation);
}

void CPlayerScript::Shoot()
{
	if (KEY_TAP(KEY::SPACE)) {
		 const vector<Ptr<GameObject>>& vecObj = LevelMgr::GetInst()->GetLevel()->GetLayer(1)->GetVecObject();

		for (const auto& pObj : vecObj) {
			if (pObj.Get()->IsHidden()) {
				pObj.Get()->Transform()->SetPos(Transform()->GetPos()
											  + Transform()->GetScale()
											  * 0.5f
											  * Transform()->GetDir(DIR::UP)); // 발사 위치 설정 비행기 몸체 보다 살짝 앞으로 위치 값을 넘김
				pObj.Get()->Transform()->SetRotation(Transform()->GetRotation());
				pObj.Get()->Show();                  // 이제부터 Tick과 Render가 돌아감
				break;                         // 하나만 발사하고 루프 탈출
			}
		}

	}
}

void CPlayerScript::Tick()
{
	Move();

	Shoot();

	if (KEY_PRESSED(KEY::X))
	{
		int i = 1;
		MeshRender()->GetMaterial()->SetScalar(INT_0, &i);
	}
	else
	{
		int i = 0;
		MeshRender()->GetMaterial()->SetScalar(INT_0, &i);
	}
}
