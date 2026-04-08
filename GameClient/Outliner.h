#pragma once
#include "EditorUI.h"
#include "TreeUI.h"
class Outliner :
    public EditorUI
{
private:
    Ptr<TreeUI> m_Tree;
    Ptr<GameObject> m_Target;
public:
    virtual void Tick_UI() override;
    void Renew();
    void AddGameObjectRecursive(const Ptr<GameObject>& pObj, const Ptr<TreeNode>& pParentNode);
    void SelectGameObject(DWORD_PTR _Object);

    void AddChild(DWORD_PTR _Src, DWORD_PTR _Dest);
public:
    Outliner();
    virtual ~Outliner();
};

