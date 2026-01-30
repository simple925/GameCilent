#include "pch.h"
#include "ALevel.h"
//#include "CPlayerProjectile.h"
#include "AssetMgr.h"
//#include "LevelMgr.h"

ALevel::ALevel()
	:Asset(ASSET_TYPE::LEVEL)
{}
ALevel::~ALevel() {}
void ALevel::Begin()
{
	for (int i = 0; i < 100; ++i) {
		Ptr<GameObject> pBullet = new GameObject;
		pBullet->SetName(L"¹Ì»çÀÏ");
		pBullet->AddComponent(new CTransform);
		pBullet->AddComponent(new CMeshRender);
		pBullet->AddComponent(new CPlayerProjectile);
		//pBullet->Transform()->SetRelativePos(vPos);
		pBullet->Transform()->SetRelativeScale(Vec3(80.f, 80.f, 1.f));
		pBullet->Hide();
		pBullet->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q").Get());
		pBullet->MeshRender()->SetMtrl(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
		this->AddObject(1, pBullet);
	}
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].Begin();
	}
}
void ALevel::AddObject(int _LayerIdx, Ptr<GameObject> _Object)
{
	m_arrLayer[_LayerIdx].addObject(_Object);
}

void ALevel::Tick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].Tick();
	}
}

void ALevel::FinalTick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].FinalTick();
	}
}