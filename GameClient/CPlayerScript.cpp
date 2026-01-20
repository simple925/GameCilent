#include "pch.h"
#include "CPlayerScript.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

#include "Device.h"
#include "CTransform.h"
#include "GameObject.h"

bool CPlayerScript::IsMouseOver()
{
	// 1. 마우스 NDC 좌표 가져오기
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(Device::GetInst()->GetHwnd(), &pt);

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
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Tick()
{
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
	Ptr<CTransform> trans = GetOwner()->Transform();
	Vec3 vPos = trans->GetPos();
	Vec3 vScale = trans->GetScale();
	Vec3 vRotation = trans->GetRotation();

	if (m_bSelected)
	{
		if (KEY_PRESSED(KEY::RIGHT)) vPos.x += 0.5f * DT;
		if (KEY_PRESSED(KEY::LEFT))  vPos.x -= 0.5f * DT;
		if (KEY_PRESSED(KEY::UP))    vPos.y += 0.5f * DT;
		if (KEY_PRESSED(KEY::DOWN))  vPos.y -= 0.5f * DT;

		if (KEY_PRESSED(KEY::W)) {
			vScale.x += 0.5f * DT; vScale.y += 0.5f * DT;
		}
		if (KEY_PRESSED(KEY::S)) {
			vScale.x -= 0.5f * DT; vScale.y -= 0.5f * DT;
			vScale.x = max(0.f, vScale.x);
			vScale.y = max(0.f, vScale.y);
		}
		if (KEY_PRESSED(KEY::Z)) {
			vRotation.z += DT * XM_PI;
		}

		trans->SetPos(vPos);
		trans->SetScale(vScale);
		trans->SetRotation(vRotation);
	}
}
