#pragma once
#include "Entity.h"
#include "GameObject.h"
class Layer
	: public Entity
{
private:
	vector<Ptr<GameObject>> m_vecParents;	// Layer 에 소속된 최상위 부모타입 오브젝트들
	vector<Ptr<GameObject>> m_vecAllObject;	// Layer 에 소속된 모든(부모, 자식타입) 오브젝트 // 파이널틱에 등록 될 오브젝트
	int						m_LayerIdx;		// Layer 본인의 인덱스(0 ~ 31)

public:
	void AddObject(Ptr<GameObject> _Object);
	vector<Ptr<GameObject>>& GetParentObjects() { return m_vecParents; }
	vector<Ptr<GameObject>>& GetAllObjects() { return m_vecAllObject; }
	void RegisterLayer(Ptr<GameObject> _Object) { m_vecAllObject.push_back(_Object); }
	void DeregisterObject() { m_vecAllObject.clear(); }
	void DeregisterAsParent(Ptr<GameObject> _Object);
public:
	void Begin();
	void Tick();
	void FinalTick();
	void Render();
public:
	Layer();
	Layer(const Layer& _Origin);
	virtual ~Layer();
	friend class ALevel;
};

