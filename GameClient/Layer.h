#pragma once
#include "Entity.h"
#include "GameObject.h"
class Layer
	: public Entity
{
private:
	vector<Ptr<GameObject>> m_vecObject;
public:
	void addObject(Ptr<GameObject> _Object) { m_vecObject.push_back(_Object); }
	void Tick();
	void FinalTick();
	void Render();
public:
	Layer();
	~Layer();
};

