#include "pch.h"
#include "LevelMgr.h"
#include "GameObject.h"
#include "AssetMgr.h"
#include "Device.h"

LevelMgr::LevelMgr() {
}
LevelMgr::~LevelMgr() {
}
void LevelMgr::Init()
{
	// Level Create
	m_CurLevel = new ALevel;
	m_CurLevel->SetName(L"Current Level");

	Ptr<GameObject> pObject = nullptr;

	// 카메라 역할 Ojbect
	pObject = new GameObject;
	//pObject->SetName(L"Main Camera");
	pObject->SetName(L"메인카메라");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CCamera);
	pObject->AddComponent(new CCamraMoveScript);


	//pObject->Camera()->LayerCheck(0); // 0번을 그려라
	pObject->Camera()->LayerCheckAll(); 
	//pObject->Camera()->LayerCheck(31); // 31 -> UI 레이어

	pObject->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pObject->Camera()->SetFar(10000.f);
	pObject->Camera()->SetFOV(90.f);
	pObject->Camera()->SetOrthoScale(1.f);

	Vec2 vResolution = Device::GetInst()->GetRenderResol();
	pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
	pObject->Camera()->SetWidth(vResolution.x);

	m_CurLevel->AddObject(0, pObject);
	/*
	pObject = new GameObject;
	//pObject->SetName(L"Monster");
	pObject->SetName(L"몬스터");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CPlayerScript);
	// 1. 위치 조정: 카메라가 바라보는 방향(Z+)으로 충분히 밀어줍니다.
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 500.f));

	// 2. 크기 조정: 2D 사각형(100, 100)과 비슷하게 보이려면 Z축도 값을 줍니다.
	pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 80.f));

	pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"CubeMesh").Get());
	pObject->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
	m_CurLevel->AddObject(0, pObject);
	*/

	pObject = new GameObject;
	pObject->SetName(L"Player");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CPlayerScript);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 300.f));
	pObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
	pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
	pObject->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	Ptr<GameObject> pChild = new GameObject;
	pChild->SetName(L"Child");
	pChild->AddComponent(new CTransform);
	pChild->AddComponent(new CMeshRender);

	pChild->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
	pChild->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
	pChild->Transform()->SetIndependentScale(true);

	pChild->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
	pChild->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
	// Player 와 Child 부모자식 연결
	pObject->AddChild(pChild);
	m_CurLevel->AddObject(0, pObject);


	/*
	// 오브젝트 생성
	pObject = new GameObject;
	//pObject->SetName(L"Player");
	pObject->SetName(L"sola");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlayerScript);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(500.f, 500.f));
	pObject->BillboardRender()->SetTexture(AssetMgr::GetInst()->Find<ATexture>(L"sola"));
	m_CurLevel->AddObject(0, pObject);
	pObject = new GameObject;
	pObject->SetName(L"mercury");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlanetControllerScript);
	pObject->Transform()->SetRelativePos(Vec3(100.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(200.f, 200.f));
	m_CurLevel->AddObject(0, pObject);

	pObject = new GameObject;
	pObject->SetName(L"earth");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlanetControllerScript);
	pObject->Transform()->SetRelativePos(Vec3(200.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(200.f, 200.f));
	m_CurLevel->AddObject(0, pObject);

	pObject = new GameObject;
	pObject->SetName(L"venus");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlanetControllerScript);
	pObject->Transform()->SetRelativePos(Vec3(300.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(200.f, 200.f));
	m_CurLevel->AddObject(0, pObject);
	*/




	pObject = new GameObject;
	pObject->SetName(L"배경");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 1000.f));
	pObject->Transform()->SetRelativeScale(Vec3(1600.0f, 900.f, 100.f));

	pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q").Get());
	pObject->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"m_univers"));

	m_CurLevel->AddObject(2, pObject);
	
	/*
	// 3. GameObject 생성 (3개)
	g_Object3 = new GameObject;
	g_Object3->AddComponent(new CTransform);
	g_Object3->AddComponent(new CMeshRender);
	//g_Object3->AddComponent(new CPlayerScript);

	g_Object3->Transform()->SetRelativePos(Vec3(-0.5f, 0.f, 0.f));
	g_Object3->Transform()->SetRelativeScale(Vec3(0.2f, 0.2f, 1.f)); // 반지름 역할

	g_Object3->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->Find<AMesh>(L"c").Get());
	g_Object3->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->Find<AGraphicShader>(L"shader2").Get());
	m_CurLevel->AddObject(0, pObject);
	*/
	//m_CurLevel->Init();

	m_CurLevel->Begin();
}

void LevelMgr::Progress()
{
	// Tick 행동 행위 가 끝난 후 
	m_CurLevel->Tick();
	// FinalTick 에서 행동 행위를 정산 하는 개념임
	m_CurLevel->FinalTick();
	
	
}
