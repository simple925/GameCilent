#include "pch.h"
#include "CMapScript.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "LevelMgr.h"
#include "CCubeScript.h"

CMapScript::CMapScript()
	: CScript(SCRIPT_TYPE::MAPSCRIPT)
    , m_RotationSpeed(100.f)
	, m_TargetAngle(0.f)
	, m_CurrentAngle(0.f)
	, m_IsRotating(false)
    , m_NeedUpdateColliders(false)
    , m_NeedSetPivot(false)
    , m_PendingAngle(0.f)
{
}
CMapScript::~CMapScript()
{
}

void CMapScript::Begin()
{
    m_Player = LevelMgr::GetInst()->GetLevel()->FindObjectByName(L"Player");
    m_NeedUpdateColliders = true;
}
void CMapScript::Tick()
{
    HandleInput();

    // 🔥 전 프레임 회전 완료 후 이번 프레임에 UpdateColliders
    if (m_NeedUpdateColliders)
    {
        m_NeedUpdateColliders = false;
        UpdateColliders();
    }

    if (!m_IsRotating) {
        return;
    }


    float step = m_RotationSpeed * DT;
    float delta = m_TargetAngle - m_CurrentAngle;

    if (fabsf(delta) <= step)
    {
        step = delta;
        m_IsRotating = false;
        //if (m_Camera)
            //m_Camera->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
    }
    else
    {
        step *= (delta > 0.f ? 1.f : -1.f);
    }

    m_CurrentAngle += step;

    Vec3 playerWorld = m_Player->Transform()->GetWorldPos();

    Vec3 rot = GetOwner()->Transform()->GetRelativeRot();
    rot.y -= XMConvertToRadians(step);
    GetOwner()->Transform()->SetRelativeRot(rot);
    //DebugPrint(L"x %f, y %f, z %f \n", rot.x, rot.y, rot.z);
    if (!m_IsRotating)
        m_NeedUpdateColliders = true;
}

void CMapScript::RotateBy(float angle)
{
    Vec3 playerWorld = m_Player->Transform()->GetWorldPos();
    Vec3 mapPos = GetOwner()->Transform()->GetRelativePos();
    DebugPrint(L"Z : %f \n", m_PlayerZ);
    
    // 맵을 플레이어 XZ로 이동, Y는 유지
    Vec3 pivot = Vec3(playerWorld.x, mapPos.y, m_PlayerZ);
    DebugPrint(L"after pivot x:%.1f y:%.1f z:%.1f\n",
        pivot.x,
        pivot.y,
        pivot.z);

    // 여기서 맵 WorldPos 찍어보기
    DebugPrint(L"after pivot - mapWorld x:%.1f y:%.1f z:%.1f\n",
        GetOwner()->Transform()->GetWorldPos().x,
        GetOwner()->Transform()->GetWorldPos().y,
        GetOwner()->Transform()->GetWorldPos().z);
    Vec3 diff = pivot - mapPos;

    DebugPrint(L"=================================== \n");

    for (auto& child : GetOwner()->GetChild())
    {
        Vec3 world = child->Transform()->GetWorldPos();
        child->Transform()->SetWorldPos(world - diff);
    }

    GetOwner()->Transform()->SetRelativePos(pivot);

    

    m_Camera = RenderMgr::GetInst()->GetPOVCamera();
    if (m_Camera)
        m_Camera->SetProjType(PROJ_TYPE::PERSPECTIVE);

    for (auto& child : GetOwner()->GetChild())
        child->SetActive(true);

    m_TargetAngle += angle;
    m_IsRotating = true;
}

void CMapScript::UpdateColliders()
{
    for (auto& col : m_vecColliderObjects)
    {
        if (col) col->Destroy();
    }
    m_vecColliderObjects.clear();

    int angle = ((int)m_TargetAngle % 360 + 360) % 360;

    // 🔥 회전 완료 후 현재 WorldPos 기준으로 갱신
    map<pair<int, int>, vector<Ptr<GameObject>>> xyGroups;
    for (auto& child : GetOwner()->GetChild())
    {
        //Vec3 pos = child->Transform()->GetWorldPos();
        Vec3 rel = child->Transform()->GetRelativePos(); // 🔥 WorldPos → RelativePos
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

        // 🔥 실제 WorldPos 기준으로 콜라이더 위치 계산
        Vec3 worldPos = front->Transform()->GetWorldPos();

        Vec3 colPos = worldPos;

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

void CMapScript::HandleInput()
{
    if (KEY_TAP(KEY::F1))
    {
        RotateBy(90.f); // 시계방향
    }
    if (KEY_TAP(KEY::F2))
    {
        RotateBy(-90.f); // 반시계방향
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
    // Thin 처리 - 콜라이더를 얇게, 상단에 정렬
    // 예: 사다리 발판, 얇은 선반
    if (bThin)
    {
        constexpr float THIN_HEIGHT = 4.f;  // 얇은 두께 (월드 유닛)

        // 상단 정렬: 오프셋을 위로 올려서 큐브 상단에 붙임
        float offsetY = (size.y * 0.5f) - (THIN_HEIGHT * 0.5f);

        obj->SetName(L"ThinCollider");
        obj->Transform()->SetRelativePos(worldPos);
        obj->Collider2D()->SetScale(Vec2(size.x, THIN_HEIGHT));
        obj->Collider2D()->SetOffset(Vec2(0.f, offsetY));
        obj->Collider2D()->SetOneWay(true); // Thin은 항상 위에서만 충돌
    }
    else
    {
        obj->SetName(L"MapCollider");
        obj->Transform()->SetRelativePos(worldPos);
        obj->Collider2D()->SetScale(size);

        // TopOnly / TopNoStraightLedge → 단방향 플랫폼
        if (colType == CollisionType::TopOnly ||
            colType == CollisionType::TopNoStraightLedge)
        {
            obj->Collider2D()->SetOneWay(true);
        }
    }

    // 🔥 Trigger 모드 (ForceHugging 감지용)
    if (mapColType == MapColliderType::Trigger)
    {
        obj->SetName(L"LadderTrigger");
        obj->Collider2D()->SetTrigger(true);  // 물리 충돌 없이 겹침만 감지
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