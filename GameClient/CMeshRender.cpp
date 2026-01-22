#include "pch.h"
#include "CMeshRender.h"

CMeshRender::CMeshRender()
	: Component(COMPONENT_TYPE::MESHRENDER)
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
	// Mesh Shader 미등록 시
	if (nullptr == m_Mesh || m_Shader == nullptr) return;
	if (nullptr != m_Tex) {
		// t0에 바인딩
		m_Tex->Binding(0);
	}
	m_Shader->Binding();

	m_Mesh->Render();
}