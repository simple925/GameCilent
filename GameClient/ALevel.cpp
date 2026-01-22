#include "pch.h"
#include "ALevel.h"
ALevel::ALevel()
	:Asset(ASSET_TYPE::LEVEL)
{}
ALevel::~ALevel() {};
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

void ALevel::Render()
{
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].Render();
	}
}
