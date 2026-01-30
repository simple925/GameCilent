#include "pch.h"
#include "AMaterial.h"
#include "Device.h"





AMaterial::AMaterial()
	: Asset(ASSET_TYPE::MATERIAL)
	, m_Const{}
{
}

AMaterial::~AMaterial()
{
}

void AMaterial::Binding()
{
	m_Shader->Binding();
	for (int i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_Tex[i]) continue;
		m_Tex[i]->Binding(i);
	}

	Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&m_Const);
	Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();
}

void AMaterial::Clear()
{
	for (int i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_Tex[i]) continue;
		m_Tex[i]->Clear();
	}
}
