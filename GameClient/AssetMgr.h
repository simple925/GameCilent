#pragma once
#include "PathMgr.h"
#include "asserts.h"

class AssetMgr
    : public singleton<AssetMgr>
{
    SINGLE(AssetMgr)
private:
    map<wstring, Ptr<Asset>>        m_mapAsset[(UINT)ASSET_TYPE::END];
    bool                            m_Changed;
public:
    void Init();
    bool IsChanged()
    {
        bool Changed = m_Changed;
        m_Changed = false;
        return Changed;
    }
public:
    void CreateEngineMesh();
    void CreateEngineShader();
    void CreateEngineTexture();
    void CreateEngineMaterial();
    void CreateEngineSprite();
    void CreateEnginePrefab();
    void CreatePrefabFromGLB();
public:
    void ApplyTransform(Ptr<GameObject> obj, const GLB_MeshContainer& data);
    Ptr<ATexture> CreateTexture(const wstring& name, int idx, const GLB_MeshContainer& data);
    Ptr<AMaterial> CreateMaterial(const wstring& name, int idx, const GLB_MeshContainer& data, Ptr<ATexture> tex);
    Ptr<AMesh> CreateMesh(const wstring& name, int idx, GLB_MeshContainer& data);
public:
    void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);
    void GetAssetNames(ASSET_TYPE _Type, vector<wstring>& _vec);
    Ptr<Asset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);

    template<typename T>
    Ptr<T> Find(const wstring& _Key);
    template<typename T>
    Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

public:
public:
    template<typename T>
    void LoadAssetFolder(const wstring& _strFolderName, const wstring& _strExt)
    {
        wstring contentPath = wstring(PathMgr::GetInst()->GetContentPath());
        wstring folderPath = contentPath + _strFolderName + L"\\";

        if (!std::filesystem::exists(folderPath))
            return;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath))
        {
            if (entry.is_regular_file())
            {
                if (entry.path().extension().wstring() == _strExt)
                {
                    // 🌟 핵심 수정 포인트: filename() 대신 stem()을 사용합니다!
                    // stem()은 "NewTileMap.tilemap" 에서 ".tilemap"을 떼고 "NewTileMap"만 반환합니다.
                    wstring keyName = entry.path().stem().wstring();

                    wstring relativePath = std::filesystem::relative(entry.path(), contentPath).wstring();

                    // 파일명(확장자 제거됨)을 Key로 사용하여 로드
                    Load<T>(keyName, relativePath);
                }
            }
        }
    }

    void LoadTextureFolder()
    {
        // Content 폴더 경로 (예: "C:\...\Game\Content\")
        wstring contentPath = wstring(PathMgr::GetInst()->GetContentPath());
        wstring texFolderPath = contentPath + L"Texture\\";

        if (!filesystem::exists(texFolderPath))
            return;

        // 핵심: 하위 폴더까지 싹 다 뒤지는 recursive_directory_iterator 로 변경!
        for (const auto& entry : filesystem::recursive_directory_iterator(texFolderPath))
        {
            if (entry.is_regular_file())
            {
                wstring ext = entry.path().extension().wstring();

                if (ext == L".bmp" || ext == L".png" || ext == L".jpg" || ext == L".tga")
                {
                    // 파일명 (예: "map_a1.cns.bmp") - Key값
                    wstring fileName = entry.path().filename().wstring();

                    // 핵심 2: Content 폴더를 기준으로 한 깔끔한 상대 경로 구하기 
                    // filesystem::relative 를 사용하면 알아서 "Texture\하위폴더\이미지.bmp" 형태로 만들어줍니다.
                    wstring relativePath = filesystem::relative(entry.path(), contentPath).wstring();

                    // 에셋 로드 (키값은 파일명, 경로는 하위폴더가 포함된 상대경로)
                    Load<ATexture>(fileName, relativePath);
                }
            }
        }
    }

    template<typename T>
    inline Ptr<T> CreateAsset(const wstring& _Key, const wstring& _RelativePath)
    {
        wstring finalKey = _Key;
        wstring finalPath = _RelativePath;

        ASSET_TYPE type = GetAssetType<T>();
        int suffix = 0;

        // 1. 고유한 이름과 경로 생성 (중복 검사)
        // 동일한 키가 이미 AssetMgr에 등록되어 있다면, 겹치지 않을 때까지 번호를 증가시킵니다.
        while (FindAsset(type, finalKey) != nullptr)
        {
            suffix++;

            // Key 변경 (예: "DefaultMap" -> "DefaultMap_1")
            finalKey = _Key + L"_" + std::to_wstring(suffix);

            // RelativePath 변경 (확장자 유지)
            if (!_RelativePath.empty())
            {
                // 뒤에서부터 첫 번째 '.' 의 위치를 찾습니다.
                size_t dotPos = _RelativePath.find_last_of(L".");

                if (dotPos != wstring::npos)
                {
                    // 확장자가 있는 경우 (예: "tilemap\map.tm" -> "tilemap\map_1.tm")
                    wstring pathWithoutExt = _RelativePath.substr(0, dotPos);
                    wstring ext = _RelativePath.substr(dotPos);
                    finalPath = pathWithoutExt + L"_" + std::to_wstring(suffix) + ext;
                }
                else
                {
                    // 확장자가 없는 경우 (예: "tilemap\map" -> "tilemap\map_1")
                    finalPath = _RelativePath + L"_" + std::to_wstring(suffix);
                }
            }
        }

        // 2. 에셋 객체 생성
        Ptr<T> pAsset = new T;

        // 3. 겹치지 않게 보정된 경로 세팅
        if (!finalPath.empty())
        {
            pAsset->SetRelativePath(finalPath);
        }

        // 4. 에셋 매니저에 고유해진 키로 등록
        // AddAsset 내부에서 m_Changed = true 처리가 진행됩니다.
        AddAsset(finalKey, pAsset.Get());

        return pAsset;
    }
};


template<typename T>
ASSET_TYPE GetAssetType()
{
    if constexpr (std::is_same_v<T, AMesh>) return ASSET_TYPE::MESH;
    else if constexpr (std::is_same_v<T, AGraphicShader>) return ASSET_TYPE::GRAPHICSHADER;
    else if constexpr (std::is_same_v<T, ATexture>) return ASSET_TYPE::TEXTURE;
    else if constexpr (std::is_same_v<T, AMaterial>) return ASSET_TYPE::MATERIAL;
    else if constexpr (std::is_same_v<T, ASprite>) return ASSET_TYPE::SPRITE;
    else if constexpr (std::is_same_v<T, AFlipbook>) return ASSET_TYPE::FLIPBOOK;
    else if constexpr (std::is_same_v<T, ATileMap>) return ASSET_TYPE::TILEMAP;
    else if constexpr (std::is_same_v<T, ALevel>) return ASSET_TYPE::LEVEL;
    else if constexpr (std::is_same_v<T, APrefab>)return ASSET_TYPE::PREFAB;
    else if constexpr (std::is_same_v<T, ASound>) return ASSET_TYPE::SOUND;
    else if constexpr (std::is_same_v<T, AComputeShader>) return ASSET_TYPE::COMPUTESHADER;
    return ASSET_TYPE::END;
}

template<typename T>
Ptr<T> AssetMgr::Find(const wstring& _Key)
{
    ASSET_TYPE Type = GetAssetType<T>();

    map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);
    if (iter == m_mapAsset[(UINT)Type].end()) {
        return nullptr;
    }
    return (T*)iter->second.Get();
}

template<typename T>
Ptr<T> AssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{

    // 동일키로 먼저 등록된 에셋이 있는지 확인
    Ptr<T> pAsset = Find<T>(_Key);

    // 동일키로 먼저 등록된 에셋이 있으면, 그걸 반환
    if (nullptr != pAsset) return pAsset;

    if constexpr (!std::is_same_v<T, AComputeShader>)
    {
        // 에셋 객체 생성
        pAsset = new T;

        // 입력된 경로로부터 에셋 로딩작업 진행	
        pAsset->Load(CONTENT_PATH + _RelativePath);

        // T 타입에 해당하는 실제 AssetType 확인
        ASSET_TYPE type = GetAssetType<T>();

        // 맵에 에셋등록
        m_mapAsset[(UINT)type].insert(make_pair(_Key, pAsset.Get()));

        // 에셋이 자신이 매니저에 등롣될때 상요된 Key 와, 
        // 자신이 어떤 경로에 있는 파일로부터 로딩된 에셋인지 스스로 알 수 있도록 해줌
        pAsset->SetKey(_Key);
        pAsset->SetRelativePath(_RelativePath);

        m_Changed = true;
    }

    return pAsset;
}

template<typename T>
Ptr<T> LoadAssetRef(FILE* _File)
{
    // Asset 이 Null 인지 아닌지 저장
    bool IsNull = false;
    fread(&IsNull, sizeof(bool), 1, _File);

    // Asset 의 Key, RelativePath 저장
    if (IsNull)
    {
        wstring Key = LoadWString(_File);
        wstring RelativePath = LoadWString(_File);
        //if (L"" == RelativePath) {
            //return nullptr;
        //}
        return AssetMgr::GetInst()->Load<T>(Key, RelativePath);
    }

    return nullptr;
}

#define FIND(Type, Key) AssetMgr::GetInst()->Find<Type>(Key)
#define LOAD(Type, AssetPath) AssetMgr::GetInst()->Load<Type>(AssetPath, AssetPath)