#include "pch.h"
#include "MeshRenderUI.h"

#include "AssetMgr.h"
#include "EditorMgr.h"
#include "ListUI.h"
#include "Asset.h"

MeshRenderUI::MeshRenderUI()
	: ComponentUI(COMPONENT_TYPE::MESHRENDER, "MeshRenderUI")
{
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::Tick_UI()
{
	OutputTitle("MeshRender");

	Ptr<CMeshRender> pMeshRender = GetTarget()->MeshRender();

	// ====
	// Mesh
	// ====
	ImGui::Text("Mesh");
	ImGui::SameLine(120);

	Ptr<AMesh> pMesh = pMeshRender->GetMesh();
	string MeshKey = pMesh ? WStrToStr(pMesh->GetKey()) : "empty";
	ImGui::InputText("##MeshName", MeshKey.data(), MeshKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

	// 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
	if (ImGui::BeginDragDropTarget())
	{
		// 데이터를 복사해서 힙에 저장
		const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content");
		if (Payload) // 마우스를 내 위에서 뗫는지 검사
		{
			DWORD_PTR data = *((DWORD_PTR*)Payload->Data);
			Ptr<Asset> pAsset = (Asset*)data;

			if (ASSET_TYPE::MESH == pAsset->GetType())
			{
				pMeshRender->SetMesh((AMesh*)pAsset.Get());
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("+##MeshBtn", Vec2(20.f, 20.f)))
	{
		// 버튼이 눌리면, 리스트UI 를 찾아서 활성화 시키고, 출력시키고 싶은 문자열을 ListUI 에 등록시킨다.
		Ptr<ListUI> pUI = dynamic_cast<ListUI*>(EditorMgr::GetInst()->FindUI("ListUI").Get());
		assert(pUI.Get());
		if (!pUI->IsActive())
		{
			vector<wstring> vecMeshNames;
			AssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MESH, vecMeshNames);
			pUI->SetUIName("Mesh List");
			pUI->AddString(vecMeshNames);
			pUI->AddDelegate(this, (DELEGATE_1)&MeshRenderUI::SelectMesh);
			pUI->SetActive(true);
		}
	}

	ImGui::Text("Material");
	ImGui::SameLine(120);
	Ptr<AMaterial> pMtrl = pMeshRender->GetMaterial();
	string MtrlKey = pMtrl ? WStrToStr(pMtrl->GetKey()) : "empty";
	ImGui::InputText("##MtrlName", MtrlKey.data(), MtrlKey.length() + 1, ImGuiInputTextFlags_ReadOnly);

	// 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
	if (ImGui::BeginDragDropTarget())
	{
		// 데이터를 복사해서 힙에 저장
		const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("Content");
		if (Payload) // 마우스를 내 위에서 뗫는지 검사
		{
			DWORD_PTR data = *((DWORD_PTR*)Payload->Data);
			Ptr<Asset> pAsset = (Asset*)data;

			if (ASSET_TYPE::MATERIAL == pAsset->GetType())
			{
				pMeshRender->SetMaterial((AMaterial*)pAsset.Get());
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("+##MtlBtn", Vec2(20.f, 20.f)))
	{
		// 버튼이 눌리면, 리스트UI 를 찾아서 활성화 시키고, 출력시키고 싶은 문자열을 ListUI 에 등록시킨다.
		Ptr<ListUI> pUI = dynamic_cast<ListUI*>(EditorMgr::GetInst()->FindUI("ListUI").Get());
		assert(pUI.Get());
		if (!pUI->IsActive())
		{
			vector<wstring> vecMtrlNames;
			AssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::MATERIAL, vecMtrlNames);
			pUI->SetUIName("Material List");
			pUI->AddString(vecMtrlNames);
			pUI->AddDelegate(this, (DELEGATE_1)&MeshRenderUI::SelectMtrl);
			pUI->SetActive(true);
		}
	}
}

void MeshRenderUI::SelectMesh(DWORD_PTR _ListUI)
{
	Ptr<ListUI> pListUI = ((ListUI*)_ListUI);

	wstring key = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());

	Ptr<AMesh> pMesh = FIND(AMesh, key);

	GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMtrl(DWORD_PTR _ListUI)
{
	Ptr<ListUI> pListUI = ((ListUI*)_ListUI);
	wstring key = wstring(pListUI->GetSelectedString().begin(), pListUI->GetSelectedString().end());
	Ptr<AMaterial> pMtrl = FIND(AMaterial, key);
	GetTarget()->MeshRender()->SetMaterial(pMtrl);
}
