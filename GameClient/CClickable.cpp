#include "pch.h"
#include "CClickable.h"

CClickable::CClickable()
	: Component(COMPONENT_TYPE::CLICKABLE)
{
}

CClickable::~CClickable()
{
}

void CClickable::OnClick()
{
	m_Target = GetOwner();

}

void CClickable::OnHover()
{
}

void CClickable::FinalTick()
{
}