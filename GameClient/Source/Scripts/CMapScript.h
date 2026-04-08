#pragma once
#include "CScript.h"

enum class MapColliderType
{
    Normal,
    Trigger,    // ForceHugging - 사다리/덩굴 감지용
};

class CMapScript :
    public CScript
{
private:
    float m_RotationSpeed;
    float m_TargetAngle;
    float m_CurrentAngle;
    bool m_IsRotating;
    vector<Ptr<GameObject>> m_vecColliderObjects;
    bool m_bInitialized;
    bool m_NeedUpdateColliders;

    bool m_NeedSetPivot;
    float m_PendingAngle;

    Ptr<CCamera> m_Camera;
    Ptr<GameObject> m_Player;

    Vec3 m_PivotPos;
    float m_PlayerZ;
    float m_SavedMapZ;
public:
    virtual void Begin() override;
    virtual void Tick() override;
private:
    void HandleInput();
    Ptr<GameObject> CreateColliderObject(
        Vec3 worldPos, Vec2 size, CollisionType colType,
        bool bThin = false, MapColliderType mapColType = MapColliderType::Normal
    );
public:
    void RotateBy(float angle);
    void UpdateColliders();
public:
    void SetPlayer(Ptr<GameObject> _Player) { m_Player = _Player; }
    GET_SET(float, PlayerZ);
public:
    // 저장 불러오기
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CMapScript);
    CMapScript();
    virtual ~CMapScript();
};

