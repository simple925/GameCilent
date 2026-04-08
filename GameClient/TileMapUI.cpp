#include "pch.h"
#include "TileMapUI.h"
#include "AssetMgr.h"

TileMapUI::TileMapUI()
	: AssetUI(ASSET_TYPE::TILEMAP)
    , m_CurTileUV0(Vec2(0.f, 0.f))
    , m_CurTileUV1(Vec2(0.f, 0.f))
    , m_EditSelectedTileIdx(-1)
    , m_bShowAtlasSelector(false)
    , m_bShowFullEditor(false)
{
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::Tick_UI()
{
	OutputTitle();

    Ptr<ATileMap> pTilemap = (ATileMap*)GetTargetAsset().Get();
    if (pTilemap == nullptr) return;

    ImGui::Text("=== TileMap Editor ===");
    ImGui::Separator();

    // ==========================================
    // 1. 아틀라스 텍스처 및 슬라이스 설정
    // ==========================================
    ImGui::Text("1. Atlas Texture & Slicing");
    Ptr<ATexture> pAtlas = pTilemap->GetAtlas();

    if (pAtlas != nullptr)
    {
        if (ImGui::ImageButton("##AtlasBtn", (ImTextureID)pAtlas->GetSRV().Get(), ImVec2(100, 100)))
            m_bShowAtlasSelector = true;
    }
    else
    {
        if (ImGui::Button("Select Atlas", ImVec2(100, 100)))
            m_bShowAtlasSelector = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Change Atlas")) m_bShowAtlasSelector = true;

    if (m_bShowAtlasSelector) ImGui::OpenPopup("Select Atlas Texture");
    RenderAtlasSelectorPopup();

    // 슬라이스 설정 (실시간으로 ATileMap 객체에 반영)
    int atlasCol = pTilemap->GetCol();
    int atlasRow = pTilemap->GetRow();
    if (atlasCol <= 0) atlasCol = 1;
    if (atlasRow <= 0) atlasRow = 1;

    if (ImGui::InputInt("Atlas Columns", &atlasCol) || ImGui::InputInt("Atlas Rows", &atlasRow))
    {
        pTilemap->SetAtlasSlice(max(1, atlasCol), max(1, atlasRow));
    }

    if (pAtlas == nullptr) return; // 아틀라스가 없으면 아래 기능을 숨김

    ImGui::Separator();

    // ==========================================
    // 2. 타일맵 크기 설정
    // ==========================================
    ImGui::Text("2. Map Size Settings");
    int mapCol = pTilemap->GetCol();
    int mapRow = pTilemap->GetRow();
    Vec2 tileSize = pTilemap->GetTileSize();

    // 최초 생성 시 기본값 부여
    if (mapCol == 0 || mapRow == 0) { mapCol = 10; mapRow = 10; pTilemap->SetRowCol(10, 10); }
    if (tileSize.x == 0 && tileSize.y == 0) tileSize = Vec2(32.f, 32.f);

    bool bMapResized = false;
    if (ImGui::InputInt("Map Columns", &mapCol)) bMapResized = true;
    if (ImGui::InputInt("Map Rows", &mapRow)) bMapResized = true;
    if (ImGui::InputFloat2("Cell Size (px)", &tileSize.x)) pTilemap->SetTileSize(tileSize);

    // 🌟 행/열이 바뀌어도 기존에 그려둔 데이터가 날아가지 않도록 resize 적용
    if (bMapResized) pTilemap->SetRowCol(max(1, mapRow), max(1, mapCol));

    ImGui::Separator();

    // ==========================================
    // 3. 팔레트 (Palette)
    // ==========================================
    ImGui::Text("3. Tile Palette");

    // 🌟 지우개 기능 (빈칸 처리용)
    if (ImGui::Button("Eraser Tool", ImVec2(120, 30)))
    {
        m_EditSelectedTileIdx = -1;
        m_CurTileUV0 = m_CurTileUV1 = Vec2(0.f, 0.f);
    }
    ImGui::SameLine();
    ImGui::Text(m_EditSelectedTileIdx == -1 ? "[Eraser Selected]" : "[Tile Selected]");

    ImGui::BeginChild("Tile Palette", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (int y = 0; y < atlasRow; y++)
    {
        for (int x = 0; x < atlasCol; x++)
        {
            ImVec2 uv0 = ImVec2((float)x / atlasCol, (float)y / atlasRow);
            ImVec2 uv1 = ImVec2((float)(x + 1) / atlasCol, (float)(y + 1) / atlasRow);
            int idx = y * atlasCol + x;

            ImGui::PushID(idx);
            ImVec4 bgCol = (m_EditSelectedTileIdx == idx) ? ImVec4(1, 1, 0, 1) : ImVec4(0, 0, 0, 0);
            if (ImGui::ImageButton("##tile", (ImTextureRef)pAtlas->GetSRV().Get(), ImVec2(40, 40), uv0, uv1, bgCol))
            {
                m_CurTileUV0 = Vec2(uv0.x, uv0.y);
                m_CurTileUV1 = Vec2(uv1.x, uv1.y);
                m_EditSelectedTileIdx = idx;
            }
            ImGui::PopID();
            if (x < atlasCol - 1) ImGui::SameLine();
        }
    }
    ImGui::EndChild();

    ImGui::Separator();

    // ==========================================
       // 4. 타일맵 그리기 그리드 (더블클릭 시 전체 창 열기)
       // ==========================================
    ImGui::Text("4. Edit Grid (Right-Click: Paint, Double-Click: Open Editor)");

    ImGui::BeginChild("EditMapGrid", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    float cellSize = 32.f; // 미리보기용 격자 크기

    for (int y = 0; y < mapRow; ++y)
    {
        for (int x = 0; x < mapCol; ++x)
        {
            int idx = y * mapCol + x;
            Ptr<ASprite> pCellSprite = pTilemap->GetSprites()[idx];
            ImGui::PushID(idx + 5000);

            // 🌟 핵심: 현재 타일이 그려질 화면 절대 좌표를 가져옵니다.
            ImVec2 curPos = ImGui::GetCursorScreenPos();

            if (pCellSprite == nullptr)
            {
                // 빈 칸 시각화: 배경은 어두운 회색, 테두리는 밝은 회색으로 그립니다.
                ImGui::GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + cellSize, curPos.y + cellSize), IM_COL32(40, 40, 40, 255));
                ImGui::GetWindowDrawList()->AddRect(curPos, ImVec2(curPos.x + cellSize, curPos.y + cellSize), IM_COL32(90, 90, 90, 255));

                ImGui::InvisibleButton("##empty", ImVec2(cellSize, cellSize));
            }
            else
            {
                // 채워진 타일도 테두리를 연하게 그려주면 맵 구조가 훨씬 잘 보입니다.
                ImGui::GetWindowDrawList()->AddRect(curPos, ImVec2(curPos.x + cellSize, curPos.y + cellSize), IM_COL32(90, 90, 90, 100));

                Vec2 vLT = pCellSprite->GetLeftTopUV();
                Vec2 vSlice = pCellSprite->GetSliceUV();
                ImGui::Image((ImTextureID)pAtlas->GetSRV().Get(), ImVec2(cellSize, cellSize),
                    ImVec2(vLT.x, vLT.y), ImVec2(vLT.x + vSlice.x, vLT.y + vSlice.y));
            }

            // 실시간 칠하기
            if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                UpdateTileAtCursor(pTilemap, pAtlas, x, y);
            }

            ImGui::PopID();
            if (x < mapCol - 1) ImGui::SameLine(0, 0);
        }
    }
}

// ==============================================================================
// 팝업, 업데이트, 스프라이트 생성 로직
// ==============================================================================
void TileMapUI::RenderAtlasSelectorPopup()
{
    if (ImGui::BeginPopupModal("Select Atlas Texture", &m_bShowAtlasSelector, ImGuiWindowFlags_AlwaysAutoResize))
    {
        vector<wstring> vecNames;
        AssetMgr::GetInst()->GetAssetNames(ASSET_TYPE::TEXTURE, vecNames);

        ImGui::BeginChild("TexList", ImVec2(300, 400), true);
        for (size_t i = 0; i < vecNames.size(); ++i)
        {
            Ptr<ATexture> pTex = AssetMgr::GetInst()->Find<ATexture>(vecNames[i]);
            if (pTex == nullptr) continue;

            ImGui::PushID((int)i);
            ImGui::Image((ImTextureID)pTex->GetSRV().Get(), ImVec2(50, 50));
            ImGui::SameLine();

            string strName(vecNames[i].begin(), vecNames[i].end());
            if (ImGui::Selectable(strName.c_str(), false, 0, ImVec2(0, 50)))
            {
                // 현재 타일맵에 아틀라스 적용
                Ptr<ATileMap> pTilemap = (ATileMap*)GetTargetAsset().Get();
                pTilemap->SetAtlas(pTex);

                m_bShowAtlasSelector = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopID();
        }
        ImGui::EndChild();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_bShowAtlasSelector = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void TileMapUI::UpdateTileAtCursor(Ptr<ATileMap> _pTilemap, Ptr<ATexture> _pAtlas, int _x, int _y)
{
    if (_pTilemap == nullptr || _pAtlas == nullptr) return;

    int atlasCol = _pTilemap->GetCol();
    int atlasRow = _pTilemap->GetRow();

    Ptr<ASprite> pNewSprite = nullptr;

    // 🌟 m_EditSelectedTileIdx가 -1 이면 pNewSprite가 nullptr인 상태로 유지됨 (지우개 역할)
    if (m_EditSelectedTileIdx != -1)
    {
        pNewSprite = GetOrCreateSprite(_pAtlas, m_EditSelectedTileIdx, atlasCol, atlasRow);
    }

    _pTilemap->SetSprite(_y, _x, pNewSprite);
}

Ptr<ASprite> TileMapUI::GetOrCreateSprite(Ptr<ATexture> _Atlas, int _TileIdx, int _SliceCol, int _SliceRow)
{
    if (_Atlas == nullptr || _TileIdx < 0) return nullptr;

    // 🌟 핵심 수정 포인트: Key 이름에 분할 정보(_SliceCol, _SliceRow)를 포함시킵니다!
    // 예: "MyAtlas_8x6_5", "MyAtlas_40x12_5" 로 고유하게 저장됨
    wstring spriteKey = _Atlas->GetKey() + L"_" + std::to_wstring(_SliceCol) + L"x" + std::to_wstring(_SliceRow) + L"_" + std::to_wstring(_TileIdx);

    Ptr<ASprite> pSprite = AssetMgr::GetInst()->Find<ASprite>(spriteKey);

    if (pSprite == nullptr)
    {
        pSprite = AssetMgr::GetInst()->CreateAsset<ASprite>(spriteKey, L"Sprite\\" + spriteKey + L".sprite");

        pSprite->SetAtlas(_Atlas);

        assert(_SliceCol > 0 && _SliceRow > 0);

        int atlasX = _TileIdx % _SliceCol;
        int atlasY = _TileIdx / _SliceCol;

        Vec2 vLT = Vec2((float)atlasX / _SliceCol, (float)atlasY / _SliceRow);
        Vec2 vSlice = Vec2(1.f / _SliceCol, 1.f / _SliceRow);

        pSprite->SetLeftTopUV(vLT);
        pSprite->SetSliceUV(vSlice);
        pSprite->SetBackgroundUV(Vec2(0.f, 0.f));
        pSprite->SetOffsetUV(Vec2(0.f, 0.f));

        wstring fullPath = wstring(PathMgr::GetInst()->GetContentPath()) + pSprite->GetRelativePath();
        pSprite->Save(fullPath);
    }

    return pSprite;
}
// ==============================================================================
// 🌟 독립된 타일맵 전용 에디터 윈도우 (크게 보기 기능)
// ==============================================================================
void TileMapUI::RenderFullEditorWindow()
{
    Ptr<ATileMap> pTilemap = (ATileMap*)GetTargetAsset().Get();
    if (pTilemap == nullptr) { m_bShowFullEditor = false; return; }
    Ptr<ATexture> pAtlas = pTilemap->GetAtlas();
    if (pAtlas == nullptr) return;

    // 별도의 팝업창(독립 윈도우) 생성
    ImGui::Begin("Workspace: Full TileMap Editor", &m_bShowFullEditor, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Current Selected Brush: ");
    ImGui::SameLine();
    if (m_EditSelectedTileIdx == -1)
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "[ Eraser ]");
    else
        ImGui::TextColored(ImVec4(0.5f, 1, 0.5f, 1), "[ Tile Index: %d ]", m_EditSelectedTileIdx);

    ImGui::Separator();

    int mapCol = pTilemap->GetCol();
    int mapRow = pTilemap->GetRow();

    // 🌟 독립 창에서는 줌(Zoom) 기능을 넣어 더 크게 볼 수 있게 합니다.
    static float fullCellSize = 64.f;
    ImGui::SliderFloat("Zoom Grid", &fullCellSize, 16.f, 128.f, "%.0f px");

    ImGui::BeginChild("FullMapGrid", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    for (int y = 0; y < mapRow; ++y)
    {
        for (int x = 0; x < mapCol; ++x)
        {
            int idx = y * mapCol + x;
            Ptr<ASprite> pCellSprite = pTilemap->GetSprites()[idx];
            ImGui::PushID(idx + 10000); // ID 충돌 방지를 위해 큰 숫자를 더함

            ImVec2 curPos = ImGui::GetCursorScreenPos();

            if (pCellSprite == nullptr)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + fullCellSize, curPos.y + fullCellSize), IM_COL32(40, 40, 40, 255));
                ImGui::GetWindowDrawList()->AddRect(curPos, ImVec2(curPos.x + fullCellSize, curPos.y + fullCellSize), IM_COL32(90, 90, 90, 255));

                ImGui::InvisibleButton("##full_empty", ImVec2(fullCellSize, fullCellSize));
            }
            else
            {
                ImGui::GetWindowDrawList()->AddRect(curPos, ImVec2(curPos.x + fullCellSize, curPos.y + fullCellSize), IM_COL32(90, 90, 90, 100));

                Vec2 vLT = pCellSprite->GetLeftTopUV();
                Vec2 vSlice = pCellSprite->GetSliceUV();
                ImGui::Image((ImTextureID)pAtlas->GetSRV().Get(), ImVec2(fullCellSize, fullCellSize),
                    ImVec2(vLT.x, vLT.y), ImVec2(vLT.x + vSlice.x, vLT.y + vSlice.y));
            }

            // 큰 창에서도 동일하게 실시간 칠하기 적용
            if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                UpdateTileAtCursor(pTilemap, pAtlas, x, y);
            }

            ImGui::PopID();
            if (x < mapCol - 1) ImGui::SameLine(0, 0);
        }
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::End();
}
