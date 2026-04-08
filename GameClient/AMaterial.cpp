#include "pch.h"
#include "AMaterial.h"
#include "Device.h"
#include "AssetMgr.h"
AMaterial::AMaterial()
	: Asset(ASSET_TYPE::MATERIAL)
	, m_Const{}
	, m_Domain(RENDER_DOMAIN::DOMAIN_NONE)
{
}

/*
	수동 구현 시 장점 디버깅 ㄱㄴ
	*** 주의점 *** 
		상속받는 클래스라면 상위 클래스의 복사 생성자 생성 지 확인을 필요로 하는 맴버가 있는지 주의해야함
*/
AMaterial::AMaterial(const AMaterial& _Other)
	: Asset(_Other)
	, m_Shader(_Other.m_Shader)
	, m_Tex{}
	, m_Const(_Other.m_Const)
	, m_Domain(_Other.m_Domain)
{
	for (int i = 0; i < TEX_END; ++i) m_Tex[i] = _Other.m_Tex[i];
}

AMaterial::~AMaterial()
{
}

void AMaterial::Binding()
{
	m_Shader->Binding();
	for (int i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_Tex[i])
		{
			m_Const.IsTex[i] = 0;
			continue;
		}
		m_Tex[i]->Binding(i);
		m_Const.IsTex[0] = 1;
	}

	Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&m_Const);
	Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();
}

void AMaterial::Clear()
{
	for (int i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_Tex[i]) continue;
		m_Tex[i]->Clear();
	}
}

AMaterial* AMaterial::Clone()
{
	return new AMaterial(*this);
}

int AMaterial::Save(const wstring& _FilePath)
{
	// 파일 스트림 커널
	FILE* pFile = nullptr;
	// 이중포인터가 필요한 이유 함수 안에서 File* 주소를 알려주기 위해서 포인터 변수를 요구함
	_wfopen_s(&pFile, _FilePath.c_str(), L"wb"); // w 쓰기 b 바이너리 0 과 1 이라고 알려줌

	// 재실이 사용하는 쉐이더 정보
	SaveAssetRef(pFile, m_Shader.Get());

	// 파이프라인 동작 시 , 어떤 텍스쳐를 전달하기로 했었는지
	for (const auto& tex : m_Tex) SaveAssetRef(pFile, tex.Get());

	// 파이프라인 동작 시, 전달할 상수 데이터
	fwrite(&m_Const, sizeof(MtrlConst), 1, pFile);

	// 렌더링 시점, 도메인
	fwrite(&m_Domain, sizeof(RENDER_DOMAIN), 1, pFile);

	fclose(pFile);
	return 0;
}

int AMaterial::Load(const wstring& _FilePath)
{
	// 파일 스트림 커널
	FILE* pFile = nullptr;

	// 이중포인터가 필요한 이유 함수 안에서 File* 주소를 알려주기 위해서 포인터 변수를 요구함
	_wfopen_s(&pFile, _FilePath.c_str(), L"rb"); // w 쓰기 r 읽기 b 바이너리 0 과 1 이라고 알려줌

	// 재질이 사용하는 쉐이더 파이프라인 정보
	m_Shader = LoadAssetRef<AGraphicShader>(pFile);

	// 파이프라인 동작 시 , 어떤 텍스쳐를 전달하기로 했었는지
	for (auto& tex : m_Tex) tex = LoadAssetRef<ATexture>(pFile);

	// 파이프라인 동작 시, 전달할 상수 데이터
	fread(&m_Const, sizeof(MtrlConst), 1, pFile);

	// 렌더링 시점, 도메인
	fread(&m_Domain, sizeof(RENDER_DOMAIN), 1, pFile);

	fclose(pFile);

	return 0;
}



