#include "pch.h"
#include "FezTrileSet.h"
#include "AssetMgr.h"

void FezTrileSet::Init(const wstring& prefabKey)
{
    if (m_Prefab)
        return;

    m_Prefab = LOAD(APrefab, prefabKey);

    if (!m_Prefab)
    {
        assert(false && "TrileSet Prefab Load 실패");
        return;
    }

    // 🔥 한 번만 Instantiate
    m_Root = m_Prefab->Instantiate();

    BuildTrileMap();
}

Ptr<GameObject> FezTrileSet::CreateTile(int id)
{
    Ptr<GameObject> obj = nullptr;
    auto iter = m_TrileMap.find(id);
    if (iter != m_TrileMap.end())
    {
        obj = iter->second->Clone();
    }
    else {
        obj = new GameObject;
        obj->AddComponent(new CTransform);
        printf("❌ Trile ID 없음: %d\n", id);
    }
    return obj;
}

void FezTrileSet::BuildTrileMap()
{
    m_TrileMap.clear();
    for (auto& child : m_Root->GetChild())
    {
        int id = ExtractID(child->GetName());
        if (id >= 0)
            m_TrileMap[id] = child;
    }
}

int FezTrileSet::ExtractID(const wstring& name)
{
    // 예: "Trile_37"
    size_t pos = name.find(L"_tid");
    if (pos == wstring::npos)
        return -1;

    wstring idStr = name.substr(pos + 4);
    try {
        return stoi(idStr);
    }
    catch (...) {
        // 숫자가 아니거나 비어있을 경우 예외 처리
        return -1;
    }
}
