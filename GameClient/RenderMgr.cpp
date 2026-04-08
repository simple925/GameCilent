#include "pch.h"
#include "RenderMgr.h"
#include "Device.h"
#include "AssetMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "LevelMgr.h"
RenderMgr::RenderMgr() 
	:m_bDebugRender(true)
{}
RenderMgr::~RenderMgr() {}

void RenderMgr::Init()
{
	m_DbgObj = new GameObject;
	m_DbgObj->AddComponent(new CTransform);
	m_DbgObj->AddComponent(new CMeshRender);

	m_DbgObj->MeshRender()->SetMaterial(FIND(AMaterial, L"DbgMtrl"));

	m_Light2DBuffer = new StructuredBuffer;
}
void RenderMgr::Progress()
{
	if (KEY_TAP(KEY::F9))
		m_bDebugRender ? m_bDebugRender = false : m_bDebugRender = true;


	//렌더링 시적전에 할 일
	Render_Start();

	// Level 의 상태가 Play 상태면, 등록된 MainCam 로 렌더링
	if (LEVEL_STATE::PLAY == LevelMgr::GetInst()->GetLevelState())
	{
		// 카메라 기반 렌더링
		if (nullptr == m_MainCam) {
			return;
		}
		// 카메라를 이용해서 그림
		m_MainCam->SortObject();
		m_MainCam->Render();
	}
	// Level 의 상태가 Pause, Stop 상태면, 등록된 EditorCam 로 렌더링
	else
	{
		// 카메라 기반 렌더링
		if (nullptr == m_EditorCam) {
			return;
		}
		// 카메라를 이용해서 그림
		m_EditorCam->SortObject();
		m_EditorCam->Render();
	}

	
	// 디버그 렌더링 요청 처리
	if (m_bDebugRender) {
		Render_Debug();
	}
	Render_End();
}

void RenderMgr::Render_Debug()
{
	list<DbgInfo>::iterator iter = m_DbgInfoList.begin();
	while (iter != m_DbgInfoList.end())
	{
		switch ((*iter).Shape)
		{
		case DBG_SHAPE::RECT:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"q_debug"));
			break;
		case DBG_SHAPE::CIRCLE:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"CircleMesh_LineStrip"));
			break;
		case DBG_SHAPE::CUBE:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"CubeMesh"));
			break;
		case DBG_SHAPE::SPHERE:
			m_DbgObj->MeshRender()->SetMesh(FIND(AMesh, L"SphereMesh"));
			break;
		}

		if ((*iter).matWorld == XMMatrixIdentity())
		{
			m_DbgObj->Transform()->SetRelativePos((*iter).Pos);
			m_DbgObj->Transform()->SetRelativeScale((*iter).Scale);
			m_DbgObj->Transform()->SetRelativeRot((*iter).Rotation);
			m_DbgObj->Transform()->FinalTick();
		}
		else
		{
			m_DbgObj->Transform()->SetWorldMat((*iter).matWorld);
		}

		if ((*iter).DepthTest) m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::LESS);
		else m_DbgObj->MeshRender()->GetMaterial()->GetShader()->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

		// Material 설정
		m_DbgObj->MeshRender()->GetMaterial()->SetScalar(VEC4_0, (*iter).Color);

		// Render 요청
		m_DbgObj->Render();

		// 렌더링 시간 누적
		(*iter).Age += E_DT;

		// 최대 수명에 도달하면 정보 삭제
		if ((*iter).Life < (*iter).Age)
		{
			// 다음 이터레이터를 줌 삭제하면 그래서 반복문 안에서 iter 를 삭제하면 else 로 구분해서 증가 시켜야함
			iter = m_DbgInfoList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void RenderMgr::Render_Start()
{

	Device::GetInst()->OMSetTarget();

	// 렌더타겟 클리어
	Device::GetInst()->ClearTarget();

	// --- [3D 광원 처리 추가] ---
	vector<Light3DInfo> vec3DInfo;
	for (const auto& light3D : m_vecLight3D) vec3DInfo.push_back(light3D->GetInfo());

	if (!vec3DInfo.empty())
	{
		// 3D 버퍼 공간 확인 및 생성
		if (vec3DInfo.size() > m_Light3DBuffer->GetElementCount())
			m_Light3DBuffer->Create(sizeof(Light3DInfo), (UINT)vec3DInfo.size(), SB_TYPE::SRV_ONLY, true, vec3DInfo.data());
		else
			m_Light3DBuffer->SetData(vec3DInfo.data(), (UINT)(sizeof(Light3DInfo) * vec3DInfo.size()));

		// t13 레지스터로 바인딩 (value.fx에서 t13으로 쓰기로 했다면)
		m_Light3DBuffer->Binding(13);
	}
	// Global 변수에 개수 업데이트
	g_Global.Light3DCount = (int)m_vecLight3D.size();
	// --------------------------

	// 등록받은 Light2D 의 광원 정보를 구조화버퍼에 담는다.
	// 구조화버퍼를 특정 t 레지스터에 바인딩 한다.
	vector<Light2DInfo>	vecInfo;

	for (const auto& light2D : m_vecLight2D) vecInfo.push_back(light2D->GetInfo());

	if (!vecInfo.empty())
	{
		// 구조화버퍼 공간이 모자라면 재확장
		if (vecInfo.size() > m_Light2DBuffer->GetElementCount())
			m_Light2DBuffer->Create(sizeof(Light2DInfo), (UINT)vecInfo.size(), SB_TYPE::SRV_ONLY, true, vecInfo.data());
		// 공간이 여우가 있으면 바로 광원데이터 전달
		else
			m_Light2DBuffer->SetData(vecInfo.data(), (UINT)(sizeof(Light2DInfo) * vecInfo.size()));

		// t12 레지스터로 바인딩
		m_Light2DBuffer->Binding(12);
	}

	g_Global.Light2DCount = (int)m_vecLight2D.size();

	// Global 데이터를 상수버퍼를 통해서 b2 레지스터에 바인딩
	Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->SetData(&g_Global);
	Device::GetInst()->GetCB(CB_TYPE::GLOBAL)->Binding();
}

void RenderMgr::Render_End()
{
	// 구조화버퍼 클리어
	// 등록 받았던 광원들 해제
	m_Light2DBuffer->Clear();
	m_vecLight2D.clear();
}