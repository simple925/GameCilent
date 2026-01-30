#include "pch.h"
#include "CMeshRender.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}
void CMeshRender::FinalTick()
{
}
void CMeshRender::Render()
{
	// Mesh or Shader 가 미설정 상태
	if (nullptr == GetMesh() || GetMtrl() == nullptr)
		return;

	GetMtrl()->Binding();
	GetMesh()->Render();
	GetMtrl()->Clear();
}