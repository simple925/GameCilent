#include "pch.h"
#include "FontMgr.h"
#include "Device.h"
#include "PathMgr.h"

FontMgr::FontMgr()
	: m_FW1Factory(nullptr)
	, m_FontWrapper(nullptr)
{
}

FontMgr::~FontMgr()
{
	if (nullptr != m_FW1Factory)
		m_FW1Factory->Release();

	if (nullptr != m_FontWrapper)
		m_FontWrapper->Release();
}

// ===============
// TestLayout 생성
// ===============
HRESULT FontMgr::CreateTextLayoutFromFontFile(
	const std::wstring& InFontFilePath,
	const std::wstring& InFamilyName,
	const std::wstring& InText,
	float InFontSize,
	float InMaxWidth,
	float InMaxHeight,
	IDWriteTextLayout** OutTextLayout)
{
	if (!OutTextLayout)
	{
		return E_INVALIDARG;
	}

	*OutTextLayout = nullptr;

	if (InFontFilePath.empty() || InFamilyName.empty() || InText.empty())
	{
		return E_INVALIDARG;
	}


	namespace fs = std::filesystem;

	std::error_code ErrorCode;
	const bool bExists = fs::exists(InFontFilePath, ErrorCode);
	const bool bIsFile = fs::is_regular_file(InFontFilePath, ErrorCode);

	if (!bExists || !bIsFile)
	{
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	ComPtr<IDWriteFactory5> DWriteFactory;
	HRESULT Hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory5),
		reinterpret_cast<IUnknown**>(DWriteFactory.GetAddressOf()));
	if (FAILED(Hr) || !DWriteFactory)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	ComPtr<IDWriteFontSetBuilder1> FontSetBuilder;
	Hr = DWriteFactory->CreateFontSetBuilder(&FontSetBuilder);
	if (FAILED(Hr) || !FontSetBuilder)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	ComPtr<IDWriteFontFile> FontFile;
	Hr = DWriteFactory->CreateFontFileReference(InFontFilePath.c_str(), nullptr, &FontFile);
	if (FAILED(Hr) || !FontFile)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	Hr = FontSetBuilder->AddFontFile(FontFile.Get());
	if (FAILED(Hr))
	{
		return Hr;
	}

	ComPtr<IDWriteFontSet> FontSet;
	Hr = FontSetBuilder->CreateFontSet(&FontSet);
	if (FAILED(Hr) || !FontSet)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	ComPtr<IDWriteFontCollection1> FontCollection;
	Hr = DWriteFactory->CreateFontCollectionFromFontSet(FontSet.Get(), &FontCollection);
	if (FAILED(Hr) || !FontCollection)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	UINT32 FamilyIndex = 0;
	BOOL bExistsFamily = FALSE;
	Hr = FontCollection->FindFamilyName(InFamilyName.c_str(), &FamilyIndex, &bExistsFamily);
	if (FAILED(Hr))
	{
		return Hr;
	}

	if (!bExistsFamily)
	{
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	}

	ComPtr<IDWriteTextFormat> TextFormat;
	Hr = DWriteFactory->CreateTextFormat(
		InFamilyName.c_str(),
		FontCollection.Get(),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		InFontSize,
		L"ko-KR",
		&TextFormat);
	if (FAILED(Hr) || !TextFormat)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	Hr = TextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	if (FAILED(Hr))
	{
		return Hr;
	}

	Hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if (FAILED(Hr))
	{
		return Hr;
	}

	Hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if (FAILED(Hr))
	{
		return Hr;
	}

	ComPtr<IDWriteTextLayout> TextLayout;
	Hr = DWriteFactory->CreateTextLayout(
		InText.c_str(),
		static_cast<UINT32>(InText.size()),
		TextFormat.Get(),
		InMaxWidth,
		InMaxHeight,
		&TextLayout);
	if (FAILED(Hr) || !TextLayout)
	{
		return FAILED(Hr) ? Hr : E_FAIL;
	}

	*OutTextLayout = TextLayout.Detach();
	return S_OK;
}

// ===============================================================================================

void FontMgr::Init(const wstring& _FontPath, const wstring& _FontFamily)
{
	if (!_FontPath.empty())
	{
		m_FontPath = CONTENT_PATH + _FontPath;
		m_FontFamily = _FontFamily;
	}

	CreateFontWrapper();
}

void FontMgr::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	if (m_FontFamily.empty())
	{
		m_FontWrapper->DrawString(
			CONTEXT,
			_pStr,
			_fFontSize,				// Font size
			_fPosX,					// Window X position
			_fPosY,					// Window Y position
			_Color,					// Text color, 0xAaBbGgRr
			FW1_RESTORESTATE		// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
	}
	else
	{
		// Font 정보를 이용해서 IDWriteTextLayout 객체 생성
		ComPtr<IDWriteTextLayout> pTextLayout = {};
		if (FAILED(CreateTextLayoutFromFontFile(
			m_FontPath
			, m_FontFamily
			, _pStr
			, _fFontSize
			, 1000.f
			, 200.f
			, pTextLayout.GetAddressOf())))
		{
			return;
		}

		m_FontWrapper->DrawTextLayout(
			CONTEXT,
			pTextLayout.Get(),
			_fPosX,					// Window X position
			_fPosY,					// Window Y position
			_Color,					// Text color, 0xAaBbGgRr);
			FW1_RESTORESTATE);
	}
}

void FontMgr::CreateFontWrapper()
{
	if (nullptr != m_FW1Factory)
	{
		m_FW1Factory->Release();
		m_FW1Factory = nullptr;
	}

	if (nullptr != m_FontWrapper)
	{
		m_FontWrapper->Release();
		m_FontWrapper = nullptr;
	}

	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_FW1Factory)))
	{
		assert(nullptr);
	}

	if (FAILED(m_FW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_FontWrapper)))
	{
		assert(nullptr);
	}
}

int FontMgr::FontCheck()
{
	IDWriteFactory* IDFactory = nullptr;
	m_FontWrapper->GetDWriteFactory(&IDFactory);

	if (!IDFactory)
	{
		assert(nullptr);
	}

	IDWriteFontCollection* FontCollection = nullptr;
	HRESULT Hr = IDFactory->GetSystemFontCollection(&FontCollection, TRUE);

	if (FAILED(Hr) || !FontCollection)
		assert(nullptr);

	UINT32 FamilyIndex = 0;
	BOOL bExists = FALSE;
	Hr = FontCollection->FindFamilyName(m_FontFamily.c_str(), &FamilyIndex, &bExists);

	FontCollection->Release();

	return bExists;
}