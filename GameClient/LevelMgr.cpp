#include "pch.h"
#include "LevelMgr.h"
#include "GameObject.h"
#include "CPlayerScript.h"
#include "AssetMgr.h"
#include "Device.h"
LevelMgr::LevelMgr() {}
LevelMgr::~LevelMgr() {}
void LevelMgr::Init()
{
	// Level Create
	m_CurLevel = new ALevel;
	m_CurLevel->setName(L"Current Level");

	Ptr<GameObject> pObject = nullptr;
	pObject = new GameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetScale(Vec3(0.3f, 0.3f, 1.f));

	pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->FindAsset<AMesh>(L"q").Get());
	pObject->MeshRender()->SetShader(AssetMgr::GetInst()->FindAsset<AGraphicShader>(L"shader3").Get());
	pObject->MeshRender()->SetTexture(AssetMgr::GetInst()->FindAsset<ATexture>(L"PlayerImage").Get());

	m_CurLevel->AddObject(0, pObject);

	// 오브젝트 생성
	pObject = new GameObject;
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CPlayerScript);

	pObject->Transform()->SetPos(Vec3(0.5f, 0.f, 0.f));
	pObject->Transform()->SetScale(Vec3(0.2f, 0.2f, 1.f));

	pObject->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->FindAsset<AMesh>(L"t").Get());
	pObject->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->FindAsset<AGraphicShader>(L"shader2").Get());
	/*
	// 3. GameObject 생성 (3개)
	g_Object3 = new GameObject;
	g_Object3->AddComponent(new CTransform);
	g_Object3->AddComponent(new CMeshRender);
	//g_Object3->AddComponent(new CPlayerScript);

	g_Object3->Transform()->SetPos(Vec3(-0.5f, 0.f, 0.f));
	g_Object3->Transform()->SetScale(Vec3(0.2f, 0.2f, 1.f)); // 반지름 역할

	g_Object3->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->FindAsset<AMesh>(L"c").Get());
	g_Object3->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->FindAsset<AGraphicShader>(L"shader2").Get());
	*/
	m_CurLevel->AddObject(0, pObject);
}

void LevelMgr::Progress()
{
	// Tick 행동 행위 가 끝난 후 
	m_CurLevel->Tick();
	// FinalTick 에서 행동 행위를 정산 하는 개념임
	m_CurLevel->FinalTick();
	Device::GetInst()->ClearTarget();
	m_CurLevel->Render();
	
}
