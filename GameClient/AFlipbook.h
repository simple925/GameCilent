#pragma once
#include "Asset.h"
#include "ASprite.h"
class AFlipbook :
    public Asset
{
private:
    vector<Ptr<ASprite>> m_vecSprite;
    vector<float> m_vecDuration;
    float   m_FPS;
    bool    m_Loop;
public:
    void AddSprite(Ptr<ASprite> _Sprite) { m_vecSprite.push_back(_Sprite); }
    void SetSprite(int _Idx, Ptr<ASprite> _Sprite)
    {
        if (m_vecSprite.size() <= _Idx)
        {
            m_vecSprite.resize(_Idx + 1);
        }
        m_vecSprite[_Idx] = _Sprite;
    }
    Ptr<ASprite> GetSprite(int _Idx) {
        if (_Idx < 0 || _Idx >= (int)m_vecSprite.size())
            return nullptr;
        return m_vecSprite[_Idx];
    }

    UINT GetSpriteCount() { return (UINT)m_vecSprite.size(); }

    int GetFrameIndex(float time) const
    {
        int frame = (int)(time * m_FPS);

        if (m_Loop)
            frame %= m_vecSprite.size();
        else if (frame >= (int)m_vecSprite.size())
            frame = (int)m_vecSprite.size() - 1;

        return frame;
    }
    void AddFrame(Ptr<ASprite> sprite, float duration){ m_vecSprite.push_back(sprite); m_vecDuration.push_back(duration); }

    float GetDuration(int idx) const { return m_vecDuration[idx]; }

public:
    float GetFPS() const { return m_FPS; }
    void SetFPS(float _FPS) { m_FPS = _FPS; }

    bool IsLoop() const { return m_Loop; }
    void SetLoop(bool _Loop) { m_Loop = _Loop; }

    virtual int Save(const wstring& _FilePath) override;
    virtual int Load(const wstring& _FilePath) override;
public:
    AFlipbook();
    virtual ~AFlipbook();
};

