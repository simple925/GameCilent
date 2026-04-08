#include "pch.h"
#include "Asset.h"


Asset::Asset(ASSET_TYPE _Type)
	: m_Type(_Type)
{
}

// 상속받는 객체 생성가를 꼭 호출 해야됨 안그럼 상위 복사생성자 호출됨
Asset::Asset(const Asset& _Origin)
	: Entity(_Origin)
	, m_Key(_Origin.m_Key)
	, m_RelativePath()
	, m_Type(_Origin.m_Type)
{
}

Asset::~Asset()
{
}
