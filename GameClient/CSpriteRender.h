#pragma once

#include "CRenderComponent.h"
#include "ASprite.h"

class CSpriteRender :
    public CRenderComponent
{
private:
    Ptr<ASprite>        m_Sprite;

public:
    void SetSprite(Ptr<ASprite> _Sprite) { m_Sprite = _Sprite; }
    Ptr<ASprite> GetSprite() { return m_Sprite; }

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void CreateMaterial() override;
public:
    virtual void SaveToLevelFile(FILE* _File);
    virtual void LoadFromLevelFile(FILE* _File);

    CLONE(CSpriteRender);
public:
    CSpriteRender();
    virtual ~CSpriteRender();
};

