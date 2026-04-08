#include "pch.h"
#include "Outliner.h"
#include "TreeUI.h"
#include "LevelMgr.h"

#include "EditorMgr.h"
#include "Inspector.h"
#include "RenderMgr.h"
#include "Source/Scripts/CCamMoveScript.h"

#include "KeyMgr.h"

Outliner::Outliner()
	: EditorUI("Outliner")
{
	m_Tree = new TreeUI;
	m_Tree->SetSaperator(false);
	m_Tree->AddDynamicSelect(this, (DELEGATE_1)&Outliner::SelectGameObject);
	m_Tree->SetDropKey("Outliner");  // Self DragDrop 사용
	m_Tree->AddDynamicDragDrop(this, (DELEGATE_2)&Outliner::AddChild);
	AddChildUI(m_Tree.Get());
}

Outliner::~Outliner()
{
}

void Outliner::Tick_UI()
{
	// 매 프레임당 호출됨
	Ptr <ALevel> pCurLevel = LevelMgr::GetInst()->GetLevel();
	if (nullptr != pCurLevel)
	{
		if (pCurLevel->IsChanged())
		{
			Renew();
			Ptr<Inspector> pInspector = (Inspector*)EditorMgr::GetInst()->FindUI("Inspector").Get();
			pInspector->SetTargetObject(nullptr);
		}
	}
}

void Outliner::SelectGameObject(DWORD_PTR _Object)
{
	m_Target = (GameObject*)_Object;
	Ptr<Inspector> pInspector = (Inspector*)EditorMgr::GetInst()->FindUI("Inspector").Get();
	assert(pInspector.Get());
	pInspector->SetTargetObject(m_Target);

	//-----------------------------------
	// Camera Target 처리
	//-----------------------------------
	auto pCam = RenderMgr::GetInst()->GetPOVCamera();

	if (pCam != nullptr)
	{
		auto pCamObj = pCam->GetOwner();
		auto camScript = pCamObj->GetScript<CCamMoveScript>();

		if (camScript)
		{
			// 🔥 핵심
			if (m_Target == pCamObj)
				camScript->SetTarget(nullptr);   // 자기 자신이면 해제
			else
				camScript->SetTarget(m_Target);
		}
	}
}

void Outliner::AddChild(DWORD_PTR _Src, DWORD_PTR _Dest)
{
	Ptr<TreeNode> pDragNode = (TreeNode*)_Src;  // 옮긴거
	Ptr<TreeNode> pDropNode = (TreeNode*)_Dest; // 놓는곳

	Ptr<GameObject> SrcObj = (GameObject*)pDragNode->Data;

	Ptr<GameObject> DestObj = nullptr;
	
	if (nullptr != pDropNode)
	{
		DestObj = (GameObject*)pDropNode->Data;
	}

	// 목적지가 없고, 자식타입 오브젝트인 경우
	if (nullptr == DestObj)
	{
		if (nullptr != SrcObj->GetParent()) {
			// 자식오브젝트를 최상위 부모 타입으로 뺀다
			SrcObj->DisconnectWithParent();
			SrcObj->RegisterAsParent();
		}
	}
	else
	{
		// SrcObj 가 DestObj 의 Ancetor 이면 안된다.
		DestObj->AddChild(SrcObj);
	}
}

void Outliner::Renew()
{
	m_Tree->Clear();

	Ptr<ALevel> pLevel = LevelMgr::GetInst()->GetLevel();
	if (nullptr == pLevel) return;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<Ptr<GameObject>>& parentObj = pLevel->GetLayer(i)->GetParentObjects();

		for (const Ptr<GameObject> pObj : parentObj)
		{
			AddGameObjectRecursive(pObj, nullptr);
		}
	}
}

void Outliner::AddGameObjectRecursive(const Ptr<GameObject>& pObj, const Ptr<TreeNode>& pParentNode)
{
	string name = WStrToStr(pObj->GetName());
	if (name.empty()) name = "No Name";
	// 현재 GameObject 노드 생성
	//Ptr<TreeNode> pCurrentNode = m_Tree->AddItem(pParentNode, string(pObj->GetName().begin(), pObj->GetName().end()));
	Ptr<TreeNode> pCurrentNode = m_Tree->AddItem(pParentNode, name.c_str(), (DWORD_PTR)pObj.Get()); // 한글 이름 때문에 추가

	const vector<Ptr<GameObject>>& vecChild = pObj->GetChild();

	for (const Ptr<GameObject>& pChild : vecChild)
	{
		// 자식들 재귀 처리
		AddGameObjectRecursive(pChild, pCurrentNode);
	}
}