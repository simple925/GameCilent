#include "pch.h"
#include "Layer.h"
Layer::Layer()
{
}

Layer::~Layer()
{
}
void Layer::Tick()
{
	for (int i = 0; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->Tick();
	}
}

void Layer::FinalTick()
{
	for (int i = 0; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->FinalTick();
	}
}

void Layer::Render()
{
	for (int i = 0; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->Render();
	}
}
