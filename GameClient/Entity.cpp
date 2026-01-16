#include "pch.h"
#include "Entity.h"

// id 순차적으로 증가하도록 시퀸셜id 생성
UINT Entity::g_NextID = 0;

Entity::Entity()
	: m_InstID(g_NextID++)
	, m_Name()
{
}

Entity::Entity(const Entity& _Other)
	: m_InstID(g_NextID++)
	, m_Name(_Other.m_Name)
{
}

Entity::~Entity()
{
}
