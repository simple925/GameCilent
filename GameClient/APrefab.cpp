#include "pch.h"
#include "APrefab.h"

APrefab::APrefab()
	: Asset(ASSET_TYPE::PREFAB)
{
}

APrefab::~APrefab()
{
}

GameObject* APrefab::Instantiate()
{
	if (nullptr == m_ProtoObj)
		return nullptr;
	GameObject* obj = m_ProtoObj->Clone();
	obj->FitCollider2D();
	return obj;
}

int APrefab::Save(const wstring& _FilePath)
{
	if (m_ProtoObj == nullptr)
		return E_FAIL;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");

	m_ProtoObj->SaveToLevelFile(pFile);

	fclose(pFile);

	return S_OK;
}

int APrefab::Load(const wstring& _FilePath)
{
 	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb");

	m_ProtoObj = new GameObject;
	m_ProtoObj->LoadFromLevelFile(pFile);

	fclose(pFile);

	return S_OK;
}
