#pragma once
#include "Entity.h"

class GameObject; // 헤더가 서로를 참조하는 문제를 피하기 위해 링킹으로 해결 GameObject 보다 먼저 만들어짐

class Component :
	public Entity
{
private:
	const COMPONENT_TYPE		m_Type;  // 컴포넌트 타입
	GameObject*					m_Owner; // 컴포넌트를 소유한 게임오브젝트
public:
	COMPONENT_TYPE GetType() { return m_Type; }
	GameObject* GetOwner() { return m_Owner; }

	virtual void FinalTick() = 0;
public:
	Component(COMPONENT_TYPE _Type);
	virtual ~Component();

	friend class GameObject;
};

