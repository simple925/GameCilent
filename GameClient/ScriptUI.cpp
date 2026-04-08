#include "pch.h"
#include "ScriptUI.h"

#include "TreeUI.h"
#include <Source/ScriptMgr.h>

ScriptUI::ScriptUI()
	: ComponentUI(COMPONENT_TYPE::SCRIPT, "ScriptUI")
	, m_ItemHeight(0)
{
	// Inspector 가 표기하려는 GameObject 가 여러개의 Script 를 가지고 있을 수 있기 때문에
	// 각 Script 에 대응하는 ScriptUI 도 여러개가 될 수 있다.
	// 따라서 ScriptUI 끼리 Inspector 의 자식으로서 서로의 이름이 겹치지 않도록 추가로 Key 를 작성한다
	int idx = GetId();
	char szNum[50] = {};
	_itoa_s(idx, szNum, 50, 10);

	SetUIKey(szNum);
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	if (nullptr == m_TargetScript)
		SetActive(false);
	else
		SetActive(true);
}

void ScriptUI::Tick_UI()
{
	m_ItemHeight = 0;

	// 스크립트 이름 출력
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.1f, 0.3f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.3f, 1.f));

	wstring WScriptName = ScriptMgr::GetScriptName(m_TargetScript.Get());
	string ScriptName = string(WScriptName.begin(), WScriptName.end());

	ImGui::Button(ScriptName.c_str());
	AddItemHeight();

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// Script 파라미터
	const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		char ID[255] = {};
		sprintf_s(ID, 255, "%d", i);

		switch (vecParam[i].Param)
		{
		case SCRIPT_PARAM::INT:
			break;
		case SCRIPT_PARAM::FLOAT:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Float";
			Key += ID;

			if (vecParam[i].IsInput)
				ImGui::InputFloat(Key.c_str(), (float*)vecParam[i].Data, vecParam[i].Step);
			else
				ImGui::DragFloat(Key.c_str(), (float*)vecParam[i].Data, vecParam[i].Step);

			AddItemHeight();
		}
		break;
		case SCRIPT_PARAM::VEC2:
			break;
		case SCRIPT_PARAM::VEC4:
			break;
		case SCRIPT_PARAM::MATRIX:
			break;
		case SCRIPT_PARAM::TEXTURE:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			AddItemHeight();

			string Key = "##Tex";
			Key += ID;

			Ptr<ATexture> pTex = *((Ptr<ATexture>*)vecParam[i].Data);

			ImTextureRef TexID = nullptr;
			if (nullptr != pTex)
			{
				TexID = (ImTextureRef)pTex->GetSRV().Get();
			}

			ImGui::ImageWithBg(TexID, ImVec2(200, 200)
				, Vec2(0.f, 0.f), Vec2(1.f, 1.f)
				, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

			// 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content");
				if (PayLoad)
				{
					DWORD_PTR data = *((DWORD_PTR*)PayLoad->Data);
					Ptr<Asset> pAsset = (Asset*)data;

					if (ASSET_TYPE::TEXTURE == pAsset->GetType())
					{
						*((Ptr<ATexture>*)vecParam[i].Data) = ((ATexture*)pAsset.Get());
					}
				}

				ImGui::EndDragDropTarget();
			}

			AddItemHeight();
		}
			break;
		case SCRIPT_PARAM::MATERIAL:
			break;
		case SCRIPT_PARAM::PREFAB:
		{
			ImGui::Text(string(vecParam[i].Desc.begin(), vecParam[i].Desc.end()).c_str());
			ImGui::SameLine(120);

			string Key = "##Prefab";
			Key += ID;

			Ptr<APrefab> pPrefab = *((Ptr<APrefab>*)vecParam[i].Data);

			string PrefabName = "None";
			if (nullptr != pPrefab)
			{
				PrefabName = string(pPrefab->GetKey().begin(), pPrefab->GetKey().end());
			}

			ImGui::InputText(Key.c_str(), PrefabName.data(), PrefabName.length() + 1, ImGuiInputTextFlags_ReadOnly);
			AddItemHeight();

			// 특정 위젯에서 드래그가 발생했고, 해당 위젯 위에 마우스가 호버링 중인지
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* PayLoad = ImGui::AcceptDragDropPayload("Content");
				if (PayLoad)
				{
					DWORD_PTR data = *((DWORD_PTR*)PayLoad->Data);
					Ptr<Asset> pAsset = (Asset*)data;

					if (ASSET_TYPE::PREFAB == pAsset->GetType())
					{
						*((Ptr<APrefab>*)vecParam[i].Data) = ((APrefab*)pAsset.Get());
					}
				}

				ImGui::EndDragDropTarget();
			}

			AddItemHeight();
		}
			break;
		default:
			break;
		}
	}

	SetSizeAsChild(Vec2(0.f, (float)m_ItemHeight));
}

void ScriptUI::AddItemHeight()
{
	ImVec2 vSize = ImGui::GetItemRectSize();
	m_ItemHeight += vSize.y + 5.f;
}
