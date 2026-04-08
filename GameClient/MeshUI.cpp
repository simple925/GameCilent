#include "pch.h"
#include "MeshUI.h"
#include "AMesh.h"

MeshUI::MeshUI()
	: AssetUI(ASSET_TYPE::MESH)
{
}

MeshUI::~MeshUI()
{
}

void MeshUI::Tick_UI()
{
    OutputTitle();

    RenderPreview();    // 1. 렌더타겟에 먼저 그림
    Ptr<AMesh> pMesh = (AMesh*)GetTargetAsset().Get();
    if (!pMesh)
        return;

    // -----------------------------
    // 기본 정보
    // -----------------------------
    ImGui::SeparatorText("Mesh Info");

    int vCount = pMesh->GetVertexCount();
    int iCount = pMesh->GetIndexCount();

    ImGui::Text("Vertex Count");
    ImGui::SameLine(150);
    ImGui::InputInt("##vcount", &vCount, 0, 0, ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Index Count");
    ImGui::SameLine(150);
    ImGui::InputInt("##icount", &iCount, 0, 0, ImGuiInputTextFlags_ReadOnly);

    ImGui::Checkbox("Wireframe Mode", &m_Wireframe);

    // -----------------------------
    // Buffer State
    // -----------------------------
    ImGui::SeparatorText("Buffer State");

    bool hasVB = pMesh->HasVertexBuffer();
    bool hasIB = pMesh->HasIndexBuffer();

    ImGui::Checkbox("Vertex Buffer", &hasVB);
    ImGui::Checkbox("Index Buffer", &hasIB);


    // -----------------------------
    // Bounding Box
    // -----------------------------
    ImGui::SeparatorText("Bounding Box");

    Vec3 min = pMesh->GetMin();
    Vec3 max = pMesh->GetMax();

    ImGui::Text("Min : %.2f, %.2f, %.2f", min.x, min.y, min.z);
    ImGui::Text("Max : %.2f, %.2f, %.2f", max.x, max.y, max.z);


    // -----------------------------
    // Topology
    // -----------------------------
    ImGui::SeparatorText("Topology");

    ImGui::Text("Triangle List");


    ImGui::SeparatorText("Vertex Layout");

    if (ImGui::BeginTable("LayoutTable", 2, ImGuiTableFlags_Borders))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Attribute");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Enabled");

        auto Row = [](const char* name, bool enabled)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(name);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(enabled ? "Yes" : "No");
            };

        Row("Position", true);
        Row("Normal", true);
        Row("UV", true);

        ImGui::EndTable();
    }
}

void MeshUI::RenderPreview()
{
}
