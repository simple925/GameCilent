#pragma once
#include "single.h"

#include <FontEngine/FW1FontWrapper.h>
#include <FontEngine/FW1CompileSettings.h>

#ifdef _DEBUG
#pragma comment(lib, "FontEngine/FW1FontWrapper_debug.lib")
#else
#pragma comment(lib, "FontEngine/FW1FontWrapper.lib"))
#endif

#define FONT_RGBA(r, g, b, a) (((((BYTE)a << 24 ) | (BYTE)b << 16) | (BYTE)g << 8) | (BYTE)r)

#include <dwrite_3.h>
#include <filesystem>
#include <algorithm>
#pragma comment(lib, "Dwrite.lib")

class FontMgr :
    public singleton<FontMgr>
{
    SINGLE(FontMgr);
private:
    IFW1Factory* m_FW1Factory;
    IFW1FontWrapper* m_FontWrapper;

    wstring                     m_FontPath;
    wstring                     m_FontFamily;
public:
    void Init(const wstring& _FontPath = L"", const wstring& _FontFamily = L"");

    // _Color : FONT_RGBA 매크로
    void DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);

// FontLayout 생성
HRESULT CreateTextLayoutFromFontFile(const std::wstring& InFontFilePath,
    const std::wstring& InFamilyName,
    const std::wstring& InText,
    float InFontSize,
    float InMaxWidth,
    float InMaxHeight,
    IDWriteTextLayout** OutTextLayout);

private:
    void CreateFontWrapper();
    int FontCheck();
};

