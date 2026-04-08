#include "pch.h"
#include "ATileMap.h"

ATileMap::ATileMap()
	: Asset(ASSET_TYPE::TILEMAP)
{
}

ATileMap::~ATileMap()
{
}

void ATileMap::SetRowCol(UINT _Row, UINT _Col)
{
	m_Row = _Row;
	m_Col = _Col;
	m_vecSpriteInfo.resize(m_Row * m_Col);
}

void ATileMap::SetSprite(UINT _Row, UINT _Col, Ptr<ASprite> _Sprite)
{
	if (nullptr == m_Atlas || _Sprite->GetAtlas() != m_Atlas)
		return;

	// 2 차원 행렬 좌표를 1차원 인덱스로 변환
	int Idx = _Row * m_Col + _Col;
	m_vecSpriteInfo[Idx] = _Sprite;
}

void ATileMap::SetAtlasSlice(UINT _Col, UINT _Row)
{
    // 1. 최소값 보장 및 멤버 변수 갱신
    m_Col = (_Col <= 0) ? 1 : _Col;
    m_Row = (_Row <= 0) ? 1 : _Row;

    // 2. 기존 스프라이트 정보 초기화
    m_vecSpriteInfo.clear();
    m_vecSpriteInfo.reserve(m_Col * m_Row);

    if (nullptr == m_Atlas)
        return;

    // 3. UV 단위 간격 계산 (전체 1.0을 등분)
    float fU_Step = 1.0f / (float)m_Col;
    float fV_Step = 1.0f / (float)m_Row;

    // 4. 루프를 돌며 각 칸에 해당하는 ASprite 생성 및 설정
    for (UINT y = 0; y < m_Row; ++y)
    {
        for (UINT x = 0; x < m_Col; ++x)
        {
            // 새로운 스프라이트 객체 생성
            Ptr<ASprite> pSprite = new ASprite;

            // 아틀라스 텍스처 연결
            pSprite->SetAtlas(m_Atlas);

            // UV 좌표 계산 및 설정
            // LeftTopUV: 해당 타일의 좌상단 시작 지점 (0.0 ~ 1.0)
            // SliceUV: 해당 타일이 차지하는 가로/세로 비율 (1/Col, 1/Row)
            pSprite->SetLeftTopUV(Vec2((float)x * fU_Step, (float)y * fV_Step));
            pSprite->SetSliceUV(Vec2(fU_Step, fV_Step));

            // 기본값 설정 (필요 시)
            pSprite->SetBackgroundUV(Vec2(fU_Step, fV_Step));
            pSprite->SetOffsetUV(Vec2(0.f, 0.f));

            // 벡터에 보관
            m_vecSpriteInfo.push_back(pSprite);
        }
    }
}

int ATileMap::Save(const wstring& _FilePath)
{
	return 0;
}

int ATileMap::Load(const wstring& _FilePath)
{
	return 0;
}