#include "pch.h"

#include "Device.h"
#include "PathMgr.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "AssetMgr.h"

#include "AMesh.h"
#include "AGraphicShader.h"

#include "GameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CPlayerScript.h"

#include "ConstBuffer.h"
// 그래픽 파이프라인 문서
//https://learn.microsoft.com/ko-kr/windows/uwp/graphics-concepts/graphics-pipeline


// 상수 버퍼
Ptr<ConstBuffer>		g_CB;

// 파이프라인


// Contant Buffer(상수버퍼)
Ptr<GameObject>				g_Object;
Ptr<GameObject>				g_Object2;
Ptr<GameObject>				g_Object3;
// 오브젝트 위치
Vec4 g_ObjectPos = Vec4(0.f, 0.f, 0.f, 0.f); 
// 오브젝트 크기
Vec4 g_ObjectScale = Vec4(1.f, 1.f, 1.f, 0.f);
// 오브젝트 회전값
Vec4 g_ObjectRot = Vec4(0.f, 0.f, 0.f, 0.f);






int TestInit()
{
	/*
	AMesh* p = new AMesh;
	Ptr<AMesh> pMesh = p;
	p = pMesh.Get();
	p = new AMesh;
	//p->AddRef();
	Ptr<AMesh> pMesh2 = nullptr;
	*pMesh2.GetAdressOf() = p;

	pMesh == pMesh2;
	pMesh == p;
	p == pMesh;
	*/

	// VertexShader
	// 컴파일할 VertexShader 함수가 작성 되어있는 파일의 절대 경로

	// 파이프라인 생성
	

	

	g_Object = new GameObject;
	g_Object->AddComponent(new CTransform);
	g_Object->AddComponent(new CMeshRender);
	g_Object->AddComponent(new CPlayerScript);

	g_Object->Transform()->SetScale(Vec3(0.3f, 0.3f, 1.f));

	g_Object->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->FindAsset<AMesh>(L"q").Get());
	g_Object->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->FindAsset<AGraphicShader>(L"shader2").Get());

	// 오브젝트 생성
	g_Object2 = new GameObject;
	g_Object2->AddComponent(new CTransform);
	g_Object2->AddComponent(new CMeshRender);
	g_Object2->AddComponent(new CPlayerScript);

	g_Object2->Transform()->SetPos(Vec3(0.5f, 0.f, 0.f));
	g_Object2->Transform()->SetScale(Vec3(0.2f, 0.2f, 1.f));

	g_Object2->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->FindAsset<AMesh>(L"t").Get());
	g_Object2->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->FindAsset<AGraphicShader>(L"shader2").Get());

	// 3. GameObject 생성 (3개)
	g_Object3 = new GameObject;
	g_Object3->AddComponent(new CTransform);
	g_Object3->AddComponent(new CMeshRender);
	g_Object3->AddComponent(new CPlayerScript);

	g_Object3->Transform()->SetPos(Vec3(-0.5f, 0.f, 0.f));
	g_Object3->Transform()->SetScale(Vec3(0.2f, 0.2f, 1.f)); // 반지름 역할

	g_Object3->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->FindAsset<AMesh>(L"c").Get());
	g_Object3->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->FindAsset<AGraphicShader>(L"shader2").Get());


	return S_OK;


}



void TestTick()
{
	g_Object->Tick();
	g_Object2->Tick();
	g_Object3->Tick();
}



void TestRender()
{
	Device::GetInst()->ClearTarget();

	g_Object3->Render();
	g_Object2->Render();
	g_Object->Render();
}

int TestFunc()
{
	static bool bInit = false;
	if (!bInit)
	{
		if (FAILED(TestInit()))
			return E_FAIL;

		bInit = true;
	}

	TestTick();

	TestRender();

	return S_OK;
}