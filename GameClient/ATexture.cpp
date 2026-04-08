#include "pch.h"
#include "ATexture.h"
#include "Device.h"
ATexture::ATexture()
	: Asset(ASSET_TYPE::TEXTURE)
	, m_Desc{}
	, m_RecentNum(-1)
{
}

ATexture::~ATexture()
{
}

int ATexture::Create(UINT _width, UINT _height, DXGI_FORMAT _format, const vector<unsigned char>& _pixelData)
{
	// 🔥 반드시 추가
	m_Image.Initialize2D(_format, _width, _height, 1, 1);
	memcpy(m_Image.GetPixels(), _pixelData.data(), _pixelData.size());

	// GPU 생성
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = _width;
	desc.Height = _height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = _format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = _pixelData.data();
	initData.SysMemPitch = _width * 4;

	HRESULT hr = DEVICE->CreateTexture2D(&desc, &initData, m_Tex2D.GetAddressOf());
	if (FAILED(hr)) return E_FAIL;

	hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf());
	if (FAILED(hr)) return E_FAIL;

	m_Tex2D->GetDesc(&m_Desc);

	return S_OK;
}

int ATexture::Load(const wstring& _FilePath)
{
	wchar_t szExt[10] = {};
	_wsplitpath_s(_FilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 10);
	wstring strExt = szExt;

	HRESULT hr = S_OK;
	
	// --- 바이너리(엔진 전용) 포맷 체크 ---
	if (strExt == L".tex" || strExt == L"")
	{
		FILE* pFile = nullptr;
		if (0 != _wfopen_s(&pFile, _FilePath.c_str(), L"rb"))
			return E_FAIL;

		size_t width, height, imgCount, pixelsSize;
		DXGI_FORMAT format;

		fread(&width, sizeof(size_t), 1, pFile);
		fread(&height, sizeof(size_t), 1, pFile);
		fread(&format, sizeof(DXGI_FORMAT), 1, pFile);
		fread(&imgCount, sizeof(size_t), 1, pFile);
		fread(&pixelsSize, sizeof(size_t), 1, pFile);

		// ScratchImage 초기화 및 데이터 로드
		m_Image.Initialize2D(format, width, height, 1, 1);
		fread(m_Image.GetPixels(), 1, pixelsSize, pFile);

		fclose(pFile);
		hr = S_OK; // 바이너리 로딩 성공
	}
	// --- 기존 이미지 포맷 체크 ---
	else if (L".dds" == strExt)
	{
		// .dds
		//LoadFromDDSFile();
		hr = LoadFromDDSFile(_FilePath.c_str(), DDS_FLAGS_NONE, nullptr, m_Image);
	}
	else if (L".tga" == strExt)
	{
		// .tga
		//LoadFromTGAFile();
		hr = LoadFromTGAFile(_FilePath.c_str(), nullptr, m_Image);

	}
	else
	{
		// WIC(window image component) .png, .jpg, .jpeg, .bmp
		//LoadFromWICFile();
		hr = LoadFromWICFile(_FilePath.c_str(), WIC_FLAGS_NONE, nullptr, m_Image);
	}
	if (FAILED(hr)) {
		MessageBox(nullptr, L"텍스쳐 시스템메모리 로딩 실패", L"텍스쳐 로딩 실패", MB_OK);
		return E_FAIL;
	}

	// SysMem			->		GPU
	// ScratchImage		->		Texture2D
	// Texture2D 생성

	// Texture2D	-> RTV ->
	//				-> DSV ->
	//				-> SRV ->
	// View 생성

	// ScratcgImage에 로딩된 이미지 데이터를 기반으로 Texture2D 를 생성하고,
	// 다시 이걸로 ShaderResourceView 까지 만들어서 ShaderResourceView 주소를 알려줌
	if (FAILED(CreateShaderResourceView(DEVICE, m_Image.GetImages(), m_Image.GetImageCount(), m_Image.GetMetadata(), m_SRV.GetAddressOf()))) {
		MessageBox(nullptr, L"ShaderResource 생성 실패", L"텍스쳐 로딩 실패", MB_OK);
		return E_FAIL;
	}

	auto meta = m_Image.GetMetadata();

	// 생성된 SRV 를 이용해서, 먼저 만들어진 Texture2D 의 주소를 알아냄
	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
	// Texture2D 를 생성할때 세팅한 Desc 옵션정보를 알아냄
	m_Tex2D->GetDesc(&m_Desc);

	return S_OK;
}

int ATexture::Save(const wstring& _FilePath)
{
	FILE* pFile = nullptr;
	if (0 != _wfopen_s(&pFile, _FilePath.c_str(), L"wb"))
		return -1;

	// 1. 메타데이터 추출
	const TexMetadata& md = m_Image.GetMetadata();

	// 2. 필수 정보 저장 (가로, 세로, 포맷, 이미지 개수 등)
	size_t width = md.width;
	size_t height = md.height;
	DXGI_FORMAT format = md.format;
	size_t imgCount = m_Image.GetImageCount();

	fwrite(&width, sizeof(size_t), 1, pFile);
	fwrite(&height, sizeof(size_t), 1, pFile);
	fwrite(&format, sizeof(DXGI_FORMAT), 1, pFile);
	fwrite(&imgCount, sizeof(size_t), 1, pFile);

	// 3. 실제 픽셀 데이터 저장
	// m_Image.GetPixels()는 전체 이미지의 통합 버퍼 포인터를 반환합니다.
	size_t pixelsSize = m_Image.GetPixelsSize();
	fwrite(&pixelsSize, sizeof(size_t), 1, pFile); // 데이터 크기 기록
	fwrite(m_Image.GetPixels(), 1, pixelsSize, pFile);

	fclose(pFile);
	return 0;
}

void ATexture::Binding(UINT _RegisterNum)
{
	m_RecentNum = _RegisterNum;

	CONTEXT->VSSetShaderResources(m_RecentNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(m_RecentNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(m_RecentNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(m_RecentNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(m_RecentNum, 1, m_SRV.GetAddressOf());
}

void ATexture::Clear()
{
	if (-1 == m_RecentNum)
	{
		return;
	}
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_RecentNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_RecentNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_RecentNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_RecentNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_RecentNum, 1, &pSRV);

	m_RecentNum = -1;
}
