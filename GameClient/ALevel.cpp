#include "pch.h"
#include "ALevel.h"
#include "AssetMgr.h"

ALevel::ALevel()
	:Asset(ASSET_TYPE::LEVEL)
	, m_Matrix{}
	, m_Changed(false)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].m_LayerIdx = i;
	}
}
ALevel::~ALevel() {}


void ALevel::AddObject(int _LayerIdx, Ptr<GameObject> _Object)
{
	m_arrLayer[_LayerIdx].AddObject(_Object);
}

void ALevel::Deregister()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].DeregisterObject();
	}
}

void ALevel::Begin()
{
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].Begin();
	}
}

void ALevel::Tick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].Tick();
	}
}

void ALevel::FinalTick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i) {
		m_arrLayer[i].FinalTick();
	}
}

void ALevel::CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2)
{
	UINT Row = _LayerIdx1;
	UINT Col = _LayerIdx2;

	// 더 작은 레이어 인덱스를 행으로 사용.
	if (_LayerIdx2 < _LayerIdx1)
	{
		Row = _LayerIdx2;
		Col = _LayerIdx1;
	}
	// 비트를 넣을땐 
	// 제거 &= -
	//

	m_Matrix[Row] ^= (1 << Col);
}

void ALevel::CheckCollisionLayer(const wstring& _LayerName1, UINT _LayerName2)
{
}

Ptr<GameObject> ALevel::FindObjectByName(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<Ptr<GameObject>>& vecParent =  m_arrLayer[i].GetParentObjects();
		for (const auto& gObj : vecParent) {
			list<Ptr<GameObject>> queue;
			queue.push_back(gObj);

			while (!queue.empty())
			{
				Ptr<GameObject> pObj = queue.front();
				queue.pop_front();

				// 찾음
				if (pObj->GetName() == _Name)
				{
					return pObj;
				}
				const vector<Ptr<GameObject>>& vecChild = pObj->GetChild();
				for (const auto& cgObj : vecChild)
				{
					queue.push_back(cgObj);
				}
			}
		}
	}
	// 없음
	return nullptr;
}

int ALevel::Save(const wstring& _FilePath)
{
	// 
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	wstring LevelName = GetName();
	SaveWString(pFile, LevelName);

	// 충돌 체크 정보
	fwrite(m_Matrix, sizeof(UINT), MAX_LAYER, pFile);

	// 레이어 정보
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어의 이름 저장
		SaveWString(pFile, m_arrLayer[i].GetName());

		// 레이어 소속 최상위 부모 오브젝트를 계층구조로 저장한다.
		const vector<Ptr<GameObject>>& vecParents = m_arrLayer[i].GetParentObjects();

		// 오브젝트 개수
		size_t parentCount = vecParents.size();
		fwrite(&parentCount, sizeof(size_t), 1, pFile);

		for (const auto& Object : vecParents)
		{
			Object->SaveToLevelFile(pFile);
		}
	}

	fclose(pFile);
	return S_OK;
}

int ALevel::Load(const wstring& _FilePath)
{
	// 
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	wstring LevelName = LoadWString(pFile);
	SetName(LevelName);

	// 충돌 체크 정보
	fread(m_Matrix, sizeof(UINT), MAX_LAYER, pFile);

	// 레이어 정보
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어의 이름
		wstring LayerName = LoadWString(pFile);
		m_arrLayer[i].SetName(LayerName);

		// 레이어 소속 최상위 부모 오브젝트를 계층구조로 불러온다.
		// 오브젝트 개수
		size_t parentCount = 0;
		fread(&parentCount, sizeof(size_t), 1, pFile);

		for (size_t j = 0; j < parentCount; ++j)
		{
			Ptr<GameObject> pObject = new GameObject;
			pObject->LoadFromLevelFile(pFile);
			AddObject(i, pObject);
		}
	}

	fclose(pFile);
	return S_OK;
}