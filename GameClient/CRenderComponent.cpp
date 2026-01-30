#include "pch.h"
#include "CRenderComponent.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _Type)
	: Component(_Type)
{
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::Begin()
{
	CreateMaterial();
}