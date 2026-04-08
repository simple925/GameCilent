#include "pch.h"
#include "CFlipbookRender.h"
#include "AssetMgr.h"
#include "TimeMgr.h"
CFlipbookRender::CFlipbookRender()
	: CRenderComponent(COMPONENT_TYPE::FLIPBOOK_RENDER)
	, m_FPS(0.f)
	, m_AccTime(0.f)
	, m_CurSprite(0)
	, m_CurFlipbook(0)
	, m_Finish(false)
	, m_RepeatCount(0)
{
}

CFlipbookRender::~CFlipbookRender()
{
}


bool CFlipbookRender::CheckFinish()
{
	// 마지막에 들어옴
	if (m_Finish)
	{
		if (0 < m_RepeatCount)
		{
			m_CurSprite = 0;
			m_Finish = false;
			--m_RepeatCount;
			return false;
		}
		else if (-1 == m_RepeatCount)
		{
			m_CurSprite = 0;
			m_Finish = false;
			return false;
		}
		else
		{
			return true;
		}
	}
}

void CFlipbookRender::FinalTick()
{
	if (CheckFinish()) return;

	float fLmit = 1.f / m_FPS; // 한장면당 1/10 초를 만들어줌
	m_AccTime += DT;

	if (fLmit < m_AccTime) {
		m_AccTime -= fLmit;
		++m_CurSprite;

		if ((int)m_vecFlipbook[m_CurFlipbook]->GetSpriteCount() <= m_CurSprite)
		{
			m_Finish = true;
			--m_CurSprite;
		}
	}
}

void CFlipbookRender::Render()
{
	Ptr<AFlipbook> pCurFlipbook = m_vecFlipbook[m_CurFlipbook];
	Ptr<ASprite> pCurSprite = pCurFlipbook->GetSprite(m_CurSprite);

	// 쉐이더로 보냄 .fx ~
	GetMaterial()->SetTexture(TEX_0, pCurSprite->GetAtlas());
	GetMaterial()->SetScalar(VEC2_0, pCurSprite->GetLeftTopUV());
	GetMaterial()->SetScalar(VEC2_1, pCurSprite->GetSliceUV());
	GetMaterial()->SetScalar(VEC2_2, pCurSprite->GetBackgroundUV());
	GetMaterial()->SetScalar(VEC2_3, pCurSprite->GetOffsetUV());
	GetMaterial()->Binding();
	GetMesh()->Render();
}

void CFlipbookRender::CreateMaterial()
{
	wstring MeshName = L"q";
	wstring MtrlName = L"FlipbookMtrl";
	wstring ShaderName = L"FlipbookShader";
	wstring FilePath = L"Shader\\flipbook.fx";
	string VS = "VS_Flipbook";
	string PS = "PS_Flipbook";

	// RectMesh 설정
	SetMesh(AssetMgr::GetInst()->Find<AMesh>(MeshName));

	// 재질 생성
	Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);

	// 찾는 재질이 없으면 생성한다.
	if (nullptr == pMtrl)
	{
		pMtrl = new AMaterial;
		pMtrl->SetName(MtrlName);

		// 쉐이더를 찾아서 재질에 세팅해준다.
		Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);

		// 찾은 or 생성한 쉐이더를 재질에 설정해주고, 재질도 에셋매니저에 등록한다.
		pMtrl->SetShader(pShader);
		pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
		AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get());
	}

	SetMaterial(pMtrl);
}

void CFlipbookRender::SaveToLevelFile(FILE* _File)
{
	CRenderComponent::SaveToLevelFile(_File);

	size_t FlipbookCount = m_vecFlipbook.size();
	fwrite(&FlipbookCount, sizeof(size_t), 1, _File);
	for (const auto& Flipbook : m_vecFlipbook)
	{
		SaveAssetRef(_File, Flipbook.Get());
	}

	fwrite(&m_CurFlipbook, sizeof(int), 1, _File);
	fwrite(&m_CurSprite, sizeof(int), 1, _File);
	fwrite(&m_FPS, sizeof(int), 1, _File);
}

void CFlipbookRender::LoadFromLevelFile(FILE* _File)
{
	CRenderComponent::LoadFromLevelFile(_File);

	size_t FlipbookCount = 0;
	fread(&FlipbookCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < FlipbookCount; ++i)
	{
		m_vecFlipbook.push_back(LoadAssetRef<AFlipbook>(_File));
	}

	fread(&m_CurFlipbook, sizeof(int), 1, _File);
	fread(&m_CurSprite, sizeof(int), 1, _File);
	fread(&m_FPS, sizeof(int), 1, _File);
}
