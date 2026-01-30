#include "pch.h"
#include "GameObject.h"

#include "KeyMgr.h"
#include "TimeMgr.h"

#include "CTransform.h"




GameObject::GameObject()
	: m_Com{}
	, m_Parent(nullptr)
	, m_bIsHidden(false)
{
}

GameObject::~GameObject()
{
}

void GameObject::AddComponent(Ptr<Component> _Com)
{
	// 렌더링 기능 컴포넌트는 하나만 가질 수 있음

	if (dynamic_cast<CRenderComponent*>(_Com.Get())) {
		assert(!m_RenderCom.Get());
		m_RenderCom = (CRenderComponent*)_Com.Get();
	}

	// 입력으로 들어온 컴퍼넌트가 스트립트면, vector 로 관리
	if (_Com->GetType() == COMPONENT_TYPE::SCRIPT) {
		m_vecScripts.push_back((CScript*)_Com.Get());
	}
	// 입력으로 들어온 컴포넌트가 스크립트가 아니면, 알맞은 배열 포인터로 가리킴
	else {
		// 해당 컴포넌트를 이미 가지고 있지 않아야 한다.
		assert(nullptr == m_Com[(UINT)_Com->GetType()]);
		m_Com[(UINT)_Com->GetType()] = _Com;
	}
	_Com->m_Owner = this;
}

void GameObject::Begin()
{
	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->Begin();
	}
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i) {
		if (nullptr != m_Com[i]) {
			m_Com[i]->Begin();
		}
	}

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->Begin();
	}
}

void GameObject::Tick()
{
	for (size_t i = 0; i < m_vecScripts.size(); ++i) {
		m_vecScripts[i]->Tick();
	}
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->Tick();
	}
}

void GameObject::FinalTick()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END;++i) {
		if (nullptr != m_Com[i]) {
			m_Com[i]->FinalTick();
		}
	}
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->FinalTick();
	}
}

void GameObject::Render()
{
	// 렌더링 관련 기능을 보유한 컴포넌트가 없으면 GameObject는 Rendering 될 수 없다.
	if (nullptr != m_RenderCom)
	{
		Transform()->Binding();

		m_RenderCom->Render();
	}
	for (size_t i = 0; i < m_vecChild.size(); ++i) {
		m_vecChild[i]->Render();
	}
}
