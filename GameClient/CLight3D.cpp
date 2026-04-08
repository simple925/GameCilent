#include "pch.h"
#include "CLight3D.h"
#include "RenderMgr.h"
CLight3D::CLight3D()
	: Component(COMPONENT_TYPE::LIGHT3D)
{
}

CLight3D::~CLight3D()
{
}

void CLight3D::FinalTick()
{
    // 1. 광원의 위치 업데이트 (World Position)
    m_Info.LightPos = Transform()->GetWorldPos();

    // 2. 광원의 방향 업데이트 (Forward 방향)
    // 3D에서는 물체의 정면 방향을 빛의 방향으로 사용합니다.
    m_Info.LightDir = Transform()->GetDir(DIR::RIGHT); // 또는 FORWARD (엔진 기준에 맞춰 사용)

    // 3. RenderMgr에 이 광원을 등록하여 이번 프레임 렌더링에 사용하게 함
    RenderMgr::GetInst()->RegisterLight3D(this);
}

void CLight3D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Info, sizeof(Light3DInfo), 1, _File);
}

void CLight3D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Info, sizeof(Light3DInfo), 1, _File);
}