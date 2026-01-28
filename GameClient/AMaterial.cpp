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

void AMaterial::SetScalar(SCALR_PARAM _Param, void* _Data)
{
	switch (_Param)
	{
		case INT_0:
		case INT_1:
		case INT_2:
		case INT_3:
			m_Const.iArr[_Param] = *((int*)_Data);
			break;
		case FLOAT_0:
		case FLOAT_1:
		case FLOAT_2:
		case FLOAT_3:
			m_Const.fArr[_Param - FLOAT_0] = *((float*)_Data);
			break;
		case VEC2_0:
		case VEC2_1:
		case VEC2_2:
		case VEC2_3:
			m_Const.v2Arr[_Param - VEC2_0] = *((Vec2*)_Data);
			break;
		case VEC4_0:
		case VEC4_1:
		case VEC4_2:
		case VEC4_3:
			m_Const.v4Arr[_Param - VEC4_0] = *((Vec4*)_Data);
			break;
		case MAT_0:
		case MAT_1:
			m_Const.mat[_Param - MAT_0] = *((Matrix*)_Data);
			break;
	}

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