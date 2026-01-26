#include "pch.h"
#include "CCamera.h"

#include "LevelMgr.h"
#include "ALevel.h"
#include "Layer.h"

#include "RenderMgr.h"

#include "CTransform.h"
CCamera::CCamera()
	:Component(COMPONENT_TYPE::CAMERA)
	, m_LayerCheck(0)
{
}

CCamera::~CCamera()
{
}
void CCamera::Begin()
{
	// 레벨이 시작될때 호출됨
	// RenderMgr 에 카메라(본인)를 등록
	RenderMgr::GetInst()->RegisterCamera(this);
}
void CCamera::FinalTick()
{
	g_Trans.matView = m_matView;

	// 월드좌표 -> 뷰 행렬 좌표를 카메라가 계산해서 줌
	// 뷰(View) 행렬 계산

	Vec3 vPos = Transform()->GetPos();

	// 카메라의 위치
	Matrix matTrans = XMMatrixIdentity();

	// 물체의 좌표를 카메라의 좌표를 빼줘서 
	// 이동 (카메라 위치를 우너점으로 되돌리는 만큼의 이동행렬)
	matTrans._41 = -vPos.x;
	matTrans._42 = -vPos.y;
	matTrans._43 = -vPos.z;

	m_matView = matTrans;

	// 회전








	// 투영(Projection) 행렬 계산
}

void CCamera::Render()
{
	g_Trans.matView = m_matView;

	// 카메라가 보는 장면을 화면에 렌더링
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		//if (m_vecObject[i]->IsHidden()) {
			//continue;
		//}
		if (false == (m_LayerCheck & (1 << i)))
		{
			continue;
		}
		Layer* pLayer = pCurLevel->GetLayer(i);
		//Layer* pLayer = LevelMgr::GetInst()->GetLevel()->GetLayer(i);
		pLayer->Render();
	}
}

void CCamera::LayerCheck(int _Idx)
{
	m_LayerCheck ^= (1 << _Idx);
}
