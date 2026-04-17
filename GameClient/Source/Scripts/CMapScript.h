#pragma once
#include "CScript.h"

enum class MapColliderType { Default, Trigger };

class CMapScript : public CScript
{
private:
    vector<Ptr<GameObject>>     m_vecColliderObjects;
    bool                        m_NeedUpdateColliders;
    int                         m_CurrentViewAngle;     // 0 / 90 / 180 / 270

public:
    // CCamMoveScript에서 카메라 회전 완료 시 호출
    void OnCameraRotated(int viewAngle);

private:
    void UpdateColliders();
    Ptr<GameObject> CreateColliderObject(Vec3 worldPos, Vec2 size,
        CollisionType colType, bool bThin,
        MapColliderType mapColType = MapColliderType::Default);

public:
    CLONE(CMapScript);
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CMapScript();
    ~CMapScript();
};
