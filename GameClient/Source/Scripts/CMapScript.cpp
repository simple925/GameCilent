#include "pch.h"
#include "CMapScript.h"
#include "TimeMgr.h"
#include "LevelMgr.h"
#include "CCubeScript.h"

CMapScript::CMapScript()
    : CScript(SCRIPT_TYPE::MAPSCRIPT)
    , m_NeedUpdateColliders(false)
    , m_CurrentViewAngle(0)
{
}
CMapScript::~CMapScript()
{
}

void CMapScript::Begin()
{
    m_NeedUpdateColliders = true;
}

void CMapScript::Tick()
{
    if (m_NeedUpdateColliders)
    {
        m_NeedUpdateColliders = false;
        UpdateColliders();
    }
}

// 카메라 회전 완료 시 CCamMoveScript에서 호출
void CMapScript::OnCameraRotated(int viewAngle)
{
    m_CurrentViewAngle = ((viewAngle % 360) + 360) % 360;
    m_NeedUpdateColliders = true;
}

void CMapScript::UpdateColliders()
{
    for (auto& col : m_vecColliderObjects)
    {
        if (col) col->Destroy();
    }
    m_vecColliderObjects.clear();

    int angle = m_CurrentViewAngle;

    map<pair<int, int>, vector<Ptr<GameObject>>> xyGroups;
    for (auto& child : GetOwner()->GetChild())
    {
        Vec3 rel = child->Transform()->GetRelativePos();
        pair<int, int> key;
        switch (angle)
        {
        case 0: case 180:
            key = { (int)roundf(rel.x / 50.f), (int)roundf(rel.y / 50.f) };
            break;
        case 90: case 270:
            key = { (int)roundf(rel.z / 50.f), (int)roundf(rel.y / 50.f) };
            break;
        default:
            key = { (int)roundf(rel.x / 50.f), (int)roundf(rel.y / 50.f) };
            break;
        }
        xyGroups[key].push_back(child);
    }

    for (auto& child : GetOwner()->GetChild())
        child->SetActive(false);

    for (auto& [key, group] : xyGroups)
    {
        sort(group.begin(), group.end(),
            [angle](const Ptr<GameObject>& a, const Ptr<GameObject>& b)
            {
                Vec3 pa = a->Transform()->GetRelativePos();
                Vec3 pb = b->Transform()->GetRelativePos();
                switch (angle)
                {
                case 0:   return pa.z < pb.z;
                case 90:  return pa.x > pb.x;
                case 180: return pa.z > pb.z;
                case 270: return pa.x < pb.x;
                default:  return pa.z < pb.z;
                }
            });

        bool frontReached = false;
        for (auto& obj : group)
        {
            TrileMeta& meta = obj->GetTrileMeta();
            if (!frontReached)
            {
                obj->SetActive(true);
                if (!meta.SeeThrough)
                    frontReached = true;
            }
            else
                obj->SetActive(false);
        }

        Ptr<GameObject> front = nullptr;
        for (auto& obj : group)
        {
            if (!obj->GetTrileMeta().SeeThrough)
            {
                front = obj;
                break;
            }
        }
        if (!front) continue;

        FaceOrientation face;
        switch (angle)
        {
        case 0:   face = FaceOrientation::Front; break;
        case 90:  face = FaceOrientation::Left;  break;
        case 180: face = FaceOrientation::Back;  break;
        case 270: face = FaceOrientation::Right; break;
        default:  face = FaceOrientation::Front; break;
        }

        TrileMeta& meta = front->GetTrileMeta();
        auto it = meta.Faces.find(face);
        if (it == meta.Faces.end()) continue;
        if (it->second == CollisionType::None ||
            it->second == CollisionType::Immaterial) continue;

        // 카메라 기준 2D 좌표로 콜라이더 위치 리매핑
        Vec3 worldPos = front->Transform()->GetWorldPos();
        Vec3 colPos;
        colPos.y = worldPos.y;

        switch (angle)
        {
        case 0:   colPos.x = worldPos.x;  colPos.z = 0.f; break; // Box2D X = worldX
        case 90:  colPos.x = worldPos.z;  colPos.z = 0.f; break; // Box2D X = worldZ
        case 180: colPos.x = -worldPos.x; colPos.z = 0.f; break; // Box2D X = -worldX
        case 270: colPos.x = -worldPos.z; colPos.z = 0.f; break; // Box2D X = -worldZ
        default:  colPos.x = worldPos.x;  colPos.z = 0.f; break;
        }

        Vec2 size = Vec2(meta.Size[0], meta.Size[1]);

        Ptr<GameObject> colObj = CreateColliderObject(colPos, size, it->second, meta.Thin);
        colObj->Transform()->SetWorldScale(front->Transform()->GetWorldScale());
        CreateObject(colObj.Get(), 5);
        m_vecColliderObjects.push_back(colObj);

        if (meta.ForceHugging)
        {
            Ptr<GameObject> triggerObj = CreateColliderObject(
                colPos, size, it->second, false, MapColliderType::Trigger);
            triggerObj->SetName(L"LadderTrigger");
            CreateObject(triggerObj.Get(), 5);
            m_vecColliderObjects.push_back(triggerObj);
        }
    }
}

Ptr<GameObject> CMapScript::CreateColliderObject(Vec3 worldPos, Vec2 size, CollisionType colType, bool bThin, MapColliderType mapColType)
{
    Ptr<GameObject> obj = new GameObject;
    obj->AddComponent(new CTransform);
    obj->AddComponent(new CRigidbody2D);
    obj->Rigidbody2D()->SetType(BODY_TYPE::STATIC);
    obj->AddComponent(new CCollider2D);

    Ptr<CCubeScript> cubeScript = new CCubeScript;
    cubeScript->SetMapScript(this);
    cubeScript->SetZ(worldPos.z);
    obj->AddComponent(cubeScript.Get());

    if (bThin)
    {
        constexpr float THIN_HEIGHT = 4.f;
        float offsetY = (size.y * 0.5f) - (THIN_HEIGHT * 0.5f);

        obj->SetName(L"ThinCollider");
        obj->Transform()->SetRelativePos(worldPos);
        obj->Collider2D()->SetScale(Vec2(size.x, THIN_HEIGHT));
        obj->Collider2D()->SetOffset(Vec2(0.f, offsetY));
        obj->Collider2D()->SetOneWay(true);
    }
    else
    {
        obj->SetName(L"MapCollider");
        obj->Transform()->SetRelativePos(worldPos);
        obj->Collider2D()->SetScale(size);

        if (colType == CollisionType::TopOnly ||
            colType == CollisionType::TopNoStraightLedge)
        {
            obj->Collider2D()->SetOneWay(true);
        }
    }

    if (mapColType == MapColliderType::Trigger)
    {
        obj->SetName(L"LadderTrigger");
        obj->Collider2D()->SetTrigger(true);
        obj->Collider2D()->SetOneWay(false);
    }

    return obj;
}

void CMapScript::SaveToLevelFile(FILE* _File)
{
}

void CMapScript::LoadFromLevelFile(FILE* _File)
{
}
