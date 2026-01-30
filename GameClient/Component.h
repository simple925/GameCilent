#pragma once
#include "pch.h"
#include "CTransform.h"

#include "Device.h"
#include "GameObject.h"
#define GET_OTHER_COMPONENT(COM_NAME) class C##COM_NAME* COM_NAME(); // 전방 선언을함

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

	//class CTransform* Transform(); // 전방 선언을함
	GET_OTHER_COMPONENT(Transform);
	GET_OTHER_COMPONENT(MeshRender);
	GET_OTHER_COMPONENT(BillboardRender);
	GET_OTHER_COMPONENT(Camera);

	virtual void Begin() {}		  // 구현 해도 되고 안해도 그만~
	virtual void FinalTick() = 0; // 각자 하위 클래스에서 구현되어야함 무조건
public:
	Component(COMPONENT_TYPE _Type);
	virtual ~Component();

	friend class GameObject;
};

