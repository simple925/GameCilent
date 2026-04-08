#include "pch.h"
#include "TreeUI.h"
#include "GameObject.h"
// ========
// TreeNode
// ========

TreeNode::TreeNode()
	: Parent(nullptr)
	, m_Owner(nullptr)
	, Framed(false)

{
	// 각 노드가 표기하려는 이름이 같을 수가 있기 때문에, 보여주려는 이름 뒤에 붙을 고유 문자열을 미리 생성해둔다.
	char szKey[50] = {};
	sprintf_s(szKey, 50, "##TreeNode%d", GetId());
	Key = szKey;
}

TreeNode::~TreeNode()
{
}

void TreeNode::Tick()
{
	// TreeNode Flag 설정
	UINT Flags = ImGuiTreeNodeFlags_SpanFullWidth		// 클릭 판정범위 확장
		| ImGuiTreeNodeFlags_OpenOnDoubleClick			// 더블 클릭으로만 열리기
		| ImGuiTreeNodeFlags_OpenOnArrow;				// 화살표 누르면 열리기

	if (vecChildNode.empty()) Flags |= ImGuiTreeNodeFlags_Leaf;

	string NodeName = Str + Key;

	if (m_Owner->GetSelected() == this) Flags |= ImGuiTreeNodeFlags_Selected;
	if (Framed) Flags |= ImGuiTreeNodeFlags_Framed;
	if (Framed && vecChildNode.empty()) NodeName = "      " + NodeName;

	// 트리노드에 등록한 문자열을 Key 로 해서 출력
	if (ImGui::TreeNodeEx(NodeName.c_str(), Flags))
	{
		ClickCheck();

		DragCheck();

		DropCheck();

		for (const auto& childNode : vecChildNode)
		{
			childNode->Tick();
		}
		ImGui::TreePop();
	}
	else
	{
		ClickCheck();

		DragCheck();

		DropCheck();
	}
}

void TreeNode::ClickCheck()
{
	// 1. 마우스 왼쪽 클릭 체크
	bool bLeftClicked = ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left);

	// 2. 엔터 키 체크 (아이템에 포커스가 있거나 마우스가 올라와 있을 때)
	// ImGuiKey_Enter 혹은 ImGuiKey_KeypadEnter 둘 다 대응
	bool bEnterPressed = ImGui::IsItemFocused() && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter));

	// 둘 중 하나라도 발생하면 선택 이벤트 실행
	if (bLeftClicked || bEnterPressed)
	{
		m_Owner->RegisterSelected(this);
	}
}

void TreeNode::DragCheck()
{
	// 드래그
	if (ImGui::BeginDragDropSource())
	{
		// Drag 사이에 Text  넣어주면, 드래그중인 마우스 위치에 Text가 따라 다니면서 렌더링된다.
		ImGui::Text(Str.c_str());

		// payload - 운반할 데이터 == 수하물
		if (0 != Data)
		{
			ImGui::SetDragDropPayload(m_Owner->GetParentUI()->GetUIName().c_str(), &Data, sizeof(DWORD_PTR));
		}

		m_Owner->RegisterDragged(this);

		ImGui::EndDragDropSource();
	}
}

void TreeNode::DropCheck()
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(m_Owner->GetDropKey().c_str());
		if (Payload)
		{
			// Drag Drop 성공
			m_Owner->RegisterDropped(this);
		}
		ImGui::EndDragDropTarget();
	}
}

void TreeUI::HandleKeyInputs()
{
	// 선택된 노드가 없으면 처리할 대상이 없음
	if (nullptr == m_Selected) return;

	ImGuiIO& io = ImGui::GetIO();

	// --- 단일 키 처리 ---
	if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
		SendKeyCommand(KEY_CMD::DEL);
	}
	else if (ImGui::IsKeyPressed(ImGuiKey_F2)) {
		SendKeyCommand(KEY_CMD::RENAME);
	}

	// --- 조합 키 처리 (Ctrl + Key) ---
	if (io.KeyCtrl)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_C)) SendKeyCommand(KEY_CMD::COPY);
		if (ImGui::IsKeyPressed(ImGuiKey_V)) SendKeyCommand(KEY_CMD::PASTE);
		if (ImGui::IsKeyPressed(ImGuiKey_D)) SendKeyCommand(KEY_CMD::CLONE);
	}
}

void TreeUI::SendKeyCommand(KEY_CMD _Cmd)
{
	if (nullptr == m_Selected) return;

	switch (_Cmd)
	{
	case KEY_CMD::DEL:
	{
		Ptr<TreeNode> node = m_Selected;

		Ptr<GameObject> obj = (GameObject*)node->Data;
		obj->Destroy();

		// 🔥 Tree에서도 제거
		if (node->Parent)
		{
			auto& vec = node->Parent->vecChildNode;
			vec.erase(remove(vec.begin(), vec.end(), node), vec.end());
		}
		else
		{
			m_vecNode.erase(remove(m_vecNode.begin(), m_vecNode.end(), node), m_vecNode.end());
		}

		m_Selected = nullptr;
	}
		break;

	case KEY_CMD::RENAME:
		((GameObject*)m_Selected->Data)->SetName(L"");
		break;

	case KEY_CMD::CLONE:
	{
		if (nullptr == m_Selected) return;

		Ptr<GameObject> pSrc = (GameObject*)m_Selected->Data;
		Ptr<GameObject> pClone = pSrc->Clone();
		pClone->SetName(pSrc->GetName() + L"_Copy");
		// 🔥 같은 부모에 붙이기
		Ptr<TreeNode> parent = m_Selected->Parent;

		CreateNodeFromGameObject(parent, pClone);
	}
		break;
	case KEY_CMD::COPY:
	{
		m_ClipboardData = m_Selected->Data;
	}
		break;
	case KEY_CMD::PASTE:
	{
		if (0 == m_ClipboardData) return;

		Ptr<GameObject> pSrc = (GameObject*)m_ClipboardData;
		Ptr<GameObject> pClone = pSrc->Clone();
		pClone->SetName(pSrc->GetName() + L"_Copy");
		Ptr<TreeNode> parent = nullptr;

		// 🔥 선택된 노드 기준으로 붙이기
		if (m_Selected)
			parent = m_Selected;

		CreateNodeFromGameObject(parent, pClone);
	}
		break;
	}
}

Ptr<TreeNode> TreeUI::CreateNodeFromGameObject(Ptr<TreeNode> parent, Ptr<GameObject> obj)
{
	return AddItem(parent, WStrToStr(obj->GetName()), (DWORD_PTR)obj.Get());
}


TreeUI::TreeUI()
	: EditorUI("TreeUI")
	, m_Inst(nullptr)
	, m_MemFunc(nullptr)
	, m_DDInst(nullptr)
	, m_DDMemFunc(nullptr)
{
}

TreeUI::~TreeUI()
{
}

void TreeUI::Tick_UI()
{
	for (const auto& node : m_vecNode)
	{
		node->Tick();
	}

	if (ImGui::IsWindowFocused())
	{
		HandleKeyInputs();
	}

	// Drag 하던 노드를 특정 노드에 Drop 함
	if (	(m_DragNode.Get() && m_DropNode.Get())
		||  (m_DragNode.Get() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)))
	{
		if (m_DDInst && m_DDMemFunc)
		{
			(m_DDInst->*m_DDMemFunc)((DWORD_PTR)m_DragNode.Get(), (DWORD_PTR)m_DropNode.Get());
		}
		m_DragNode = nullptr;
		m_DropNode = nullptr;
	}
}

Ptr<TreeNode> TreeUI::AddItem(Ptr<TreeNode> _ParentNode, string _String, DWORD_PTR _Data)
{
	Ptr<TreeNode> pNewNode = new TreeNode;
	pNewNode->Str = _String;
	pNewNode->m_Owner = this;
	pNewNode->Data = _Data;
	// 최상위 부모노드로 추가
	if (nullptr == _ParentNode)
	{
		pNewNode->Parent = nullptr;
		m_vecNode.push_back(pNewNode);
	}
	// 특정 노드 밑에 자식으로 추가
	else
	{
		_ParentNode->AddChildNode(pNewNode);
	}
	return pNewNode;

}

void TreeUI::RegisterSelected(Ptr<TreeNode> _Node)
{
	m_Selected = _Node;
	if (m_Inst && m_MemFunc)
	{
		(m_Inst->*m_MemFunc)(_Node->Data);
	}
}
