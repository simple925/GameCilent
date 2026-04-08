#pragma once
#include "AssetUI.h"
#include "CTileRender.h"

class TileMapUI :
    public AssetUI
{
private:
    Vec2            m_CurTileUV0;
    Vec2            m_CurTileUV1;
    int              m_EditSelectedTileIdx; // 팔레트에서 선택한 타일 인덱스 (-1은 지우개)

    bool           m_bShowAtlasSelector;  // 아틀라스 선택 팝업창 플래그

    bool            m_bShowFullEditor; // 더블클릭 시 띄울 넓은 창 플래그

private:
    void RenderAtlasSelectorPopup();
    void UpdateTileAtCursor(Ptr<class ATileMap> _pTilemap, Ptr<class ATexture> _pAtlas, int _x, int _y);
    Ptr<class ASprite> GetOrCreateSprite(Ptr<class ATexture> _Atlas, int _TileIdx, int _SliceCol, int _SliceRow);
    void RenderFullEditorWindow();

public:
    virtual void Tick_UI() override;

public:
    TileMapUI();
    virtual ~TileMapUI();
};

