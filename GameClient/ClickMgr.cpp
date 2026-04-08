#include "pch.h"
#include "ClickMgr.h"
#include "KeyMgr.h"
#include "RenderMgr.h"
#include "CollisionMgr.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "CCollider2D.h"
ClickMgr::ClickMgr()
{
}
ClickMgr::~ClickMgr()
{
}
void ClickMgr::Init()
{
}

void ClickMgr::Progress()
{
    /*
	if (nullptr == RenderMgr::GetInst()->GetPOVCamera()) return;

    Vec2 mouse = KeyMgr::GetInst()->GetMousePos();
    Vec3 worldPos = RenderMgr::GetInst()->ScreenToWorld2D(mouse);

    DebugPrint(L"[Mouse] screen: %.1f %.1f\n", mouse.x, mouse.y);
    DebugPrint(L"[Mouse] world : %.1f %.1f %.1f\n", worldPos.x, worldPos.y, worldPos.z);

    // 🔥 Hover
    Ptr<GameObject> hovered = CollisionMgr::GetInst()->Pick(worldPos);

    Ptr<GameObject> prev = m_Hovered;
    m_Hovered = hovered;

    if (hovered)
    {
        bool inside = hovered->Collider2D()->Contains(worldPos);

        DebugPrint(L"[Contains] %d\n", inside);
        DebugPrint(L"[Pick] SUCCESS\n");
    }
    else
    {
        DebugPrint(L"[Pick] FAIL\n");
    }

    if (prev != m_Hovered)
    {
        if (prev) {
            // Hover Exit
        }
        if (m_Hovered) {
            // Hover Enter
        }
    }

    // 🔥 Click
    if (KEY_TAP(KEY::M_LBUTTON))
    {
        Ptr<GameObject> clicked = CollisionMgr::GetInst()->Pick(worldPos);

        RenderMgr::GetInst()->GetPOVCamera()
            ->GetOwner()
            ->GetScript<CCamMoveScript>()
            ->SetTarget(clicked);
    }
    */
}
