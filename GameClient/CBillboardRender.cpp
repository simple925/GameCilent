#include "pch.h"
#include "CBillboardRender.h"
#include "AssetMgr.h"

CBillboardRender::CBillboardRender()
	: CRenderComponent(COMPONENT_TYPE::BILLBOARD_RENDER)
{
}

CBillboardRender::~CBillboardRender()
{
}

void CBillboardRender::FinalTick()
{
}

void CBillboardRender::Render()
{
	if (nullptr == GetMaterial() || nullptr == GetMesh())
	{
		return;
	}
	GetMaterial()->SetScalar(VEC2_0, m_BillboardScale);
	GetMaterial()->Binding();
	GetMesh()->Render();
	GetMaterial()->Clear();
}

void CBillboardRender::CreateMaterial()
{
	// RectMesh 설정
	SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));

	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(L"BillboardMtrl");
	if (nullptr == pMtrl) {
		pMtrl = new AMaterial;
		pMtrl->SetName(L"BillboardMtrl");

		// 쉐이더 생성
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(L"BillboardShader");

		// 찾은 or 생성한 쉐이더를 재질에 설정해주고, 재질도 에셋매니저에 등록한다.
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
		pMtrl->SetTexture(TEX_0, m_Texture);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}
	SetMaterial(pMtrl);
}

void CBillboardRender::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);
	fwrite(&m_BillboardScale, sizeof(Vec2), 1, _File);
	SaveAssetRef(_File, m_Texture.Get());
}

void CBillboardRender::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);
	fread(&m_BillboardScale, sizeof(Vec2), 1, _File);
	m_Texture = LoadAssetRef<ATexture>(_File);
}
