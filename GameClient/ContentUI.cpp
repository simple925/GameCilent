#include "pch.h"
#include "ContentUI.h"
#include "TreeUI.h"
#include "AssetMgr.h"

#include "EditorMgr.h"
#include "Inspector.h"

ContentUI::ContentUI()
	: EditorUI("Content")
{
	m_Tree = new TreeUI;
    m_Tree->SetSaperator(false);
    m_Tree->AddDynamicSelect(this, (DELEGATE_1)&ContentUI::SelectAsset);
	AddChildUI(m_Tree.Get());

}

ContentUI::~ContentUI()
{
}

void ContentUI::Tick_UI()
{
    if(AssetMgr::GetInst()->IsChanged()) Renew();
}

void ContentUI::Renew()
{
    m_Tree->Clear();
    for (int i = 0; i < (int)ASSET_TYPE::END; ++i)
    {
        ASSET_TYPE type = (ASSET_TYPE)i;

        vector<wstring> vecNames;
        AssetMgr::GetInst()->GetAssetNames(type, vecNames);

        //if (vecNames.empty())
            //continue;
        
        Ptr<TreeNode> pNode = m_Tree->AddItem(nullptr, ToString(type));
        pNode->SetFramed(true);
        for (const wstring& name : vecNames)
        {
            Ptr<Asset> pAsset = AssetMgr::GetInst()->FindAsset(type, name);
            m_Tree->AddItem(pNode, WStrToStr(name), (DWORD_PTR)pAsset.Get());
        }
    }
}

void ContentUI::SelectAsset(DWORD_PTR _Asset)
{
    if (0 == _Asset) return;

    Ptr<Asset> pAsset = (Asset*)_Asset;
    Ptr<Inspector> pInspector = (Inspector*)EditorMgr::GetInst()->FindUI("Inspector").Get();
    assert(pInspector.Get());
    pInspector->SetTargetAsset(pAsset);
}
