#pragma once
#include "APrefab.h"
class FezTrileSet
{
public:
    void Init(const wstring& prefabKey);
    Ptr<GameObject> CreateTile(int id);  // Clone 반환

private:
    void BuildTrileMap();
    int  ExtractID(const wstring& name);

    Ptr<APrefab>                        m_Prefab;
    Ptr<GameObject>                     m_Root;
    unordered_map<int, Ptr<GameObject>> m_TrileMap;
};
