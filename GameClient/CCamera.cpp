#include "pch.h"
#include "CCamera.h"

#include "LevelMgr.h"
#include "ALevel.h"
#include "Layer.h"

#include "RenderMgr.h"

#include "CTransform.h"
#include "Device.h"

#include "GameObject.h"

CCamera::CCamera()
	:Component(COMPONENT_TYPE::CAMERA)
	, m_LayerCheck(0)
	, m_OrthoScale(1.f)
	, m_Near(1.f)
	, m_fOrbitDist(1000.f)
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
	// 월드좌표 -> 뷰 행렬 좌표를 카메라가 계산해서 줌
	// 뷰(View) 행렬 계산
	Vec3 vPos = Transform()->GetRelativePos();

	// 카메라의 위치
	Matrix matTrans = XMMatrixIdentity();

	// 물체의 좌표를 카메라의 좌표를 빼줘서 
	// 이동 (카메라 위치를 우너점으로 되돌리는 만큼의 이동행렬)
	matTrans._41 = -vPos.x;
	matTrans._42 = -vPos.y;
	matTrans._43 = -vPos.z;

	// 전치 - 트렌스포즈
	// 뒤집다 플립
	// View 회전 (방법1 역행렬로 구하는법, 방법2 직교행렬)
	// - 카메라가 바라보는 방향을 z 축이 되도록 회전하는 부분이 추가
	// 월드상의 카메라의 Right, Up, Front 방향 벡터를 가져와서 회전행렬을 곱하면 이게 다시 x축, y축, z축이 되는 회전행을 구해야함
	
	// vR			  (1 0 0)
	// vU	x	R	= (0 1 0)
	// vZ			  (0 0 1)

	//          vR
	// R 행렬은  vU		의 역행렬
	//          vZ

	// vR
	// vU	행렬은 행끼리 관계가 직교상태이기 때문에, 전치(Transpose)를 통해서 역행렬을 쉽게 구할 수 있음
	// vZ

	// 전치한 행렬과 곱해보면, 자기자신과 내적을 한 경우 결과가 1, 다른 직교벡터랑 내적을 한 경우 0 이 나오기 때문
	// 내적은 각 성분끼리 곱을 합친 결과,
	// 내적 결과값의 의미는 (벡터의 길이) x (길이) x (두 벡터가 이루는 각도의 cos 값)

	//		(vR.x	vU.x	vF.x	0)
	// R == (vR.y	vU.y	vF.y	0)
	//		(vR.z	vU.z	vF.z	0)
	//		(   0	   0	   0	1)

	Vec3 vR = Transform()->GetDir(DIR::RIGHT);
	Vec3 vU = Transform()->GetDir(DIR::UP);
	Vec3 vF = Transform()->GetDir(DIR::FRONT);
	Matrix matRot = XMMatrixIdentity();

	matRot._11 = vR.x;  matRot._12 = vU.x;  matRot._13 = vF.x;
	matRot._21 = vR.y; 	matRot._22 = vU.y; 	matRot._23 = vF.y;
	matRot._31 = vR.z; 	matRot._32 = vU.z; 	matRot._33 = vF.z;

	// 카메라가 원점인 공감으로 이동, 카메라가 바라본든 방향을 z 축으로 회전하는 회전을 적용
	// 이동 -> 회전
	m_matView = matTrans * matRot; // 카메라 월드 행렬의 역행렬

	if(PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// 직교투영(Orthographic) 행렬 계산  near: 카메라 렌즈길이 far
		m_matProj = XMMatrixOrthographicLH(m_Width * m_OrthoScale, (m_Width / m_AspectRatio) * m_OrthoScale, m_Near, m_Far);
	}
	else {
		// near -> Far 가 0 -> 1 로 설정함
		// 원근투영(Perspective)
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_Near, m_Far);
	}

	// 뷰는 월드로 옮김
	// 투영 z 값이 쓰여야 해서 따로 나눠줘야함
}



void CCamera::SortObject()
{

	m_vecOpaque.clear();
	m_vecMasked.clear();
	m_vecTransparent.clear();
	m_vecPostProcess.clear();


	// 카메라가 보는 장면을 화면에 렌더링
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetLevel();
	if (nullptr == pCurLevel) return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 카메라가 레이어를 볼 수 있어야 함
		if (false == (m_LayerCheck & (1 << i)))
		{
			continue;
		}

		// 레이어에 소속된 모든 오브젝트를 가져온다
		Layer* pLayer = pCurLevel->GetLayer(i);
		const vector<Ptr<GameObject>>& vecObject = pLayer->GetAllObjects();

		for (const auto& gameObject : vecObject)
		{
			if (!gameObject->IsActive()) continue;
			// 오브젝트가 렌더링을 할 수 있는 상태인지 확인
			if (nullptr == gameObject->GetRenderCom()
				|| nullptr == gameObject->GetRenderCom()->GetMesh()
				|| nullptr == gameObject->GetRenderCom()->GetMaterial())
			{
				continue;
			}

			RENDER_DOMAIN domain = gameObject->GetRenderCom()->GetMaterial()->GetDomain();

			switch (domain)
			{
			case RENDER_DOMAIN::DOMAIN_OPAQUE:
				m_vecOpaque.push_back(gameObject.Get());
				break;
			case RENDER_DOMAIN::DOMAIN_MASKED:
				m_vecMasked.push_back(gameObject.Get());
				break;
			case RENDER_DOMAIN::DOMAIN_TRANSPARENT:
				m_vecTransparent.push_back(gameObject.Get());
				break;
			case RENDER_DOMAIN::DOMAIN_POSTPROCESS:
				m_vecPostProcess.push_back(gameObject.Get());
				break;
			}
		}
	}
}

void CCamera::Render()
{
	g_Trans.matView = m_matView;
	g_Trans.matProj = m_matProj;

	// Domain 순서대로 렌더링 진행
	for (const auto& opaque : m_vecOpaque) opaque->Render();
	for (const auto& masked : m_vecMasked) masked->Render();
	for (const auto& transparent : m_vecTransparent) transparent->Render();
	for (const auto& postProcess : m_vecPostProcess) postProcess->Render();
}

void CCamera::LayerCheck(int _Idx)
{
	m_LayerCheck ^= (1 << _Idx);
}

void CCamera::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_LayerCheck, sizeof(UINT), 1, _File);
	fwrite(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fwrite(&m_Far, sizeof(float), 1, _File);
	fwrite(&m_Near, sizeof(float), 1, _File);
	fwrite(&m_Width, sizeof(float), 1, _File);
	fwrite(&m_AspectRatio, sizeof(float), 1, _File);
	fwrite(&m_FOV, sizeof(float), 1, _File);
	fwrite(&m_OrthoScale, sizeof(float), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
	fread(&m_LayerCheck, sizeof(UINT), 1, _File);
	fread(&m_ProjType, sizeof(PROJ_TYPE), 1, _File);
	fread(&m_Far, sizeof(float), 1, _File);
	fread(&m_Near, sizeof(float), 1, _File);
	fread(&m_Width, sizeof(float), 1, _File);
	fread(&m_AspectRatio, sizeof(float), 1, _File);
	fread(&m_FOV, sizeof(float), 1, _File);
	fread(&m_OrthoScale, sizeof(float), 1, _File);
}