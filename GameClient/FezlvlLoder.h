#pragma once
#include "FezTrileSet.h"
#include "tinygltf/json.hpp"
using json = nlohmann::json;

class FezlvlLoder
{
public:
    static void Load(const wstring& prefabPath,
        const wstring& levelPath,
        ALevel* level);

private:
    static Ptr<GameObject> BuildRoot(const json& data, FezTrileSet& trileSet);
    static Ptr<GameObject> CreateTrileObject(const json& t, FezTrileSet& trileSet);
    static Vec3            CalcCenter(const vector<pair<Ptr<GameObject>, Vec3>>& list);
};

