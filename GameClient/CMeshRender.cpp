#include "pch.h"
#include "CMeshRender.h"

CMeshRender::CMeshRender()
	: Component(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}
void CMeshRender::Render()
{
	m_Shader->Binding();

	m_Mesh->Render();
}