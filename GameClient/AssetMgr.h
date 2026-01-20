#pragma once
#include "single.h"
#include "Asset.h"
#include "AMesh.h"
#include "AGraphicShader.h"
class AssetMgr
    : public singleton<AssetMgr>
{
    SINGLE(AssetMgr)
private:
    map<wstring, Ptr<Asset>>        m_mapAsset[(UINT)ASSET_TYPE::END];
public:
    void Init();
public:
    Ptr<Asset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);
    void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);

    template<typename T>
    Ptr<T> FindAsset(const wstring& _Key);
};

template<typename T>
inline Ptr<T> AssetMgr::FindAsset(const wstring& _Key)
{
    const type_info& info = typeid(T);

    ASSET_TYPE Type = ASSET_TYPE::END;

    if (info.hash_code() == typeid(AMesh).hash_code()) {
        Type = ASSET_TYPE::MESH;
    }
    else if (info.hash_code() == typeid(AGraphicShader).hash_code()) {
        Type = ASSET_TYPE::GRAPHICSHADER;
    }
    map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);
    if (iter == m_mapAsset[(UINT)Type].end()) {
        return nullptr;
    }
    return (T*)iter->second.Get();
}
