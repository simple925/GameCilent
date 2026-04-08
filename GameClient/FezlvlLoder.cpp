#include "pch.h"
#include "FezlvlLoder.h"
#include "AssetMgr.h"
#include "GameObject.h"
#include "CTransform.h"
#include "APrefab.h"
#include "ALevel.h"
#include "PathMgr.h"
#include <fstream>
#include "Source/Scripts/CMapScript.h"

void FezlvlLoder::Load(const wstring& prefabPath, const wstring& levelPath, ALevel* level)
{
    // 1. TrileSet 초기화
    FezTrileSet trileSet;
    trileSet.Init(prefabPath);

    // 2. JSON 로드
    ifstream file(WStrToStr(CONTENT_PATH + levelPath));
    json data;
    file >> data;

    // 3. 루트 오브젝트 빌드
    Ptr<GameObject> rootObj = BuildRoot(data, trileSet);
    rootObj->AddComponent(new CMapScript);
    rootObj->SetName(L"rMap");
    // 4. 레벨에 추가
    level->AddObject(5, rootObj);
    level->SetChanged();
}

Ptr<GameObject> FezlvlLoder::BuildRoot(const json& data, FezTrileSet& trileSet)
{
    Ptr<GameObject> rootObj = new GameObject;
    rootObj->AddComponent(new CTransform);
    rootObj->Transform()->SetRelativeScale(Vec3(1.f, 1.f, 1.f));

    // 타일 생성 + pos 저장
    vector<pair<Ptr<GameObject>, Vec3>> objList;
    for (const auto& t : data["Triles"])
    {
        Ptr<GameObject> obj = CreateTrileObject(t, trileSet);
        objList.push_back({ obj, obj->Transform()->GetRelativePos() });
    }

    // 중심 보정
    Vec3 center = CalcCenter(objList);
    rootObj->Transform()->SetRelativePos(Vec3(0.f,0.f,0.f));

    for (auto& [obj, pos] : objList)
    {
        obj->Transform()->SetRelativePos(pos - center);
        rootObj->AddChild(obj);
    }

    return rootObj;
}

Ptr<GameObject> FezlvlLoder::CreateTrileObject(const json& t, FezTrileSet& trileSet)
{
    const float TILE_SCALE = 50.f;
    int id = t["Id"];

    Ptr<GameObject> obj = trileSet.CreateTile(id);

    Vec3 pos(
        t["Position"][0].get<float>() * TILE_SCALE,
        t["Position"][1].get<float>() * TILE_SCALE,
        -t["Position"][2].get<float>() * TILE_SCALE
    );
    obj->Transform()->SetRelativePos(pos);
    obj->Transform()->SetRelativeScale(Vec3(TILE_SCALE, TILE_SCALE, TILE_SCALE));

    float yaw = -t["Phi"].get<int>() * XM_PIDIV2;
    obj->Transform()->SetRelativeRot(Vec3(0.f, yaw, 0.f));

    return obj;
}

Vec3 FezlvlLoder::CalcCenter(const vector<pair<Ptr<GameObject>, Vec3>>& list)
{
    Vec3 center(0.f, 0.f, 0.f);
    for (auto& [obj, pos] : list)
        center += pos;
    return center / (float)list.size();
}