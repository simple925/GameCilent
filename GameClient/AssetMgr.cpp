#include "pch.h"
#include "AssetMgr.h"
#include "PathMgr.h"

AssetMgr::AssetMgr()
{

}
AssetMgr::~AssetMgr()
{

}

Ptr<Asset> AssetMgr::FindAsset(ASSET_TYPE _Type, const wstring& _Key)
{
	map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[(UINT)_Type].find(_Key);
	if (iter == m_mapAsset->end()) {
		return nullptr;
	}
	return iter->second;
}

void AssetMgr::AddAsset(const wstring& _Key, Ptr<Asset> _Asset)
{
	assert(m_mapAsset[(UINT)_Asset->getType()].find(_Key) == m_mapAsset[(UINT)_Asset->getType()].end());
	m_mapAsset[(UINT)_Asset->getType()].insert(make_pair(_Key, _Asset));
}

void AssetMgr::Init()
{
	Ptr<AMesh> pMesh = nullptr;
	Ptr<AGraphicShader> pShader = nullptr;

	// 네모
	Vtx arrVtx[4] = {};
	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 0.f);

	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vUV = Vec2(1.f, 0.f);
	arrVtx[1].vColor = Vec4(0.f, 0.f, 1.f, 0.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vUV = Vec2(1.f, 1.f);
	arrVtx[2].vColor = Vec4(0.f, 1.f, 0.f, 0.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vUV = Vec2(0.f, 1.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 0.f);
	UINT arrIdx[6] = { 0, 2, 3, 0, 1, 2 };
	// 사각형 메쉬 생성
	pMesh = new AMesh;
	pMesh->Create(arrVtx, 4, arrIdx, 6);
	//m_mapAsset.insert(make_pair(L"q", pMesh.Get()));
	AddAsset(L"q", pMesh.Get());

	// 동구람위
	const int TRICOUNT = 100;
	const int VTXCOUNT = TRICOUNT + 1;
	const int IDXCOUNT = TRICOUNT * 3;
	Vtx oVtx[VTXCOUNT] = {};
	UINT oIdx[IDXCOUNT] = {};

	float fRadius = 1.0f; // Scale로 조절할 것이므로 기본 크기는 1로 설정
	oVtx[0].vPos = Vec3(0.f, 0.f, 0.f);
	oVtx[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	for (int i = 0; i < TRICOUNT; ++i) {
		float fAngle = (XM_2PI / (float)TRICOUNT) * (float)i;
		oVtx[i + 1].vPos = Vec3(cosf(fAngle) * fRadius, -sinf(fAngle) * fRadius, 0.f);
		oVtx[i + 1].vColor = Vec4(
			(float)rand() / (float)RAND_MAX,
			(float)rand() / (float)RAND_MAX,
			(float)rand() / (float)RAND_MAX,
			1.f
		);
	}
	for (int i = 0; i < TRICOUNT; ++i) {
		oIdx[i * 3 + 0] = 0;
		oIdx[i * 3 + 1] = i + 1;
		oIdx[i * 3 + 2] = (i == TRICOUNT - 1) ? 1 : i + 2;
	}
	pMesh = new AMesh;
	pMesh->Create(oVtx, VTXCOUNT, oIdx, IDXCOUNT);
	//m_mapAsset.insert(make_pair(L"c", pMesh.Get()));
	AddAsset(L"c", pMesh.Get());

	// 셈오
	Vtx arr[3] = {};
	arr[0].vPos = Vec3(0.f, 1.f, 0.f);
	arr[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arr[0].vUV = Vec2(0.5f, 0.0f);

	arr[1].vPos = Vec3(1.f, -1.f, 0.f);
	arr[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arr[1].vUV = Vec2(1.0f, 1.0f);

	arr[2].vPos = Vec3(-1.f, -1.f, 0.f);
	arr[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arr[2].vUV = Vec2(0.0f, 1.0f);

	UINT idx[3] = { 0 , 1 , 2 };

	pMesh = new AMesh;
	pMesh->Create(arr, 3, idx, 3);
	//m_mapAsset.insert(make_pair(L"t", pMesh.Get()));
	AddAsset(L"t", pMesh.Get());

	// ===================
	// Cube (3D 정육면체)
	// ===================
	Vtx cubeVtx[8] = {};

	// 앞면 (Z = -0.5f)
	cubeVtx[0].vPos = Vec3(-0.5f, 0.5f, -0.5f); cubeVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f); cubeVtx[0].vUV = Vec2(0.f, 0.f);
	cubeVtx[1].vPos = Vec3(0.5f, 0.5f, -0.5f); cubeVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f); cubeVtx[1].vUV = Vec2(1.f, 0.f);
	cubeVtx[2].vPos = Vec3(0.5f, -0.5f, -0.5f); cubeVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f); cubeVtx[2].vUV = Vec2(1.f, 1.f);
	cubeVtx[3].vPos = Vec3(-0.5f, -0.5f, -0.5f); cubeVtx[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f); cubeVtx[3].vUV = Vec2(0.f, 1.f);

	// 뒷면 (Z = 0.5f)
	cubeVtx[4].vPos = Vec3(-0.5f, 0.5f, 0.5f); cubeVtx[4].vColor = Vec4(1.f, 0.f, 1.f, 1.f); cubeVtx[4].vUV = Vec2(0.f, 0.f);
	cubeVtx[5].vPos = Vec3(0.5f, 0.5f, 0.5f); cubeVtx[5].vColor = Vec4(1.f, 1.f, 0.f, 1.f); cubeVtx[5].vUV = Vec2(1.f, 0.f);
	cubeVtx[6].vPos = Vec3(0.5f, -0.5f, 0.5f); cubeVtx[6].vColor = Vec4(0.f, 1.f, 1.f, 1.f); cubeVtx[6].vUV = Vec2(1.f, 1.f);
	cubeVtx[7].vPos = Vec3(-0.5f, -0.5f, 0.5f); cubeVtx[7].vColor = Vec4(0.f, 0.f, 0.f, 1.f); cubeVtx[7].vUV = Vec2(0.f, 1.f);

	// 인덱스 (삼각형 12개, 시계 방향)
	UINT cubeIdx[36] = {
		0, 1, 2, 0, 2, 3, // 앞
		4, 6, 5, 4, 7, 6, // 뒤
		4, 5, 1, 4, 1, 0, // 위
		3, 2, 6, 3, 6, 7, // 아래
		4, 0, 3, 4, 3, 7, // 왼쪽
		1, 5, 6, 1, 6, 2  // 오른쪽
	};

	pMesh = new AMesh;
	pMesh->Create(cubeVtx, 8, cubeIdx, 36);
	AddAsset(L"CubeMesh", pMesh.Get());


	pShader = new AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\test.fx", "VS_Test");
	pShader->CreatePixelShader(L"Shader\\test.fx", "PS_Test");
	//m_mapAsset.insert(make_pair(L"shader1", pShader.Get()));
	AddAsset(L"shader1", pShader.Get());

	pShader = new AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\test.fx", "VS_Test");
	pShader->CreatePixelShader(L"Shader\\test.fx", "PS_Test2");
	//m_mapAsset.insert(make_pair(L"shader2", pShader.Get()));
	AddAsset(L"shader2", pShader.Get());

	Ptr<ATexture> pTex = nullptr;
	pTex = new ATexture;
	wstring FilePath = CONTENT_PATH;
	FilePath += L"Texture\\Character.png";
	pTex->Load(FilePath);
	AddAsset(L"PlayerImage", pTex.Get());

	pTex = nullptr;
	pTex = new ATexture;
	FilePath = CONTENT_PATH;
	FilePath += L"Texture\\Fighter.bmp";
	pTex->Load(FilePath);
	AddAsset(L"Fighter", pTex.Get());

	pTex = nullptr;
	pTex = new ATexture;
	FilePath = CONTENT_PATH;
	FilePath += L"Texture\\1945\\back1.png";
	pTex->Load(FilePath);
	AddAsset(L"back_1", pTex.Get());

	pTex = nullptr;
	pTex = new ATexture;
	FilePath = CONTENT_PATH;
	FilePath += L"Texture\\1945\\back2.png";
	pTex->Load(FilePath);
	AddAsset(L"back_2", pTex.Get());

	pTex = nullptr;
	pTex = new ATexture;
	FilePath = CONTENT_PATH;
	FilePath += L"Texture\\1945\\back3.png";
	pTex->Load(FilePath);
	AddAsset(L"back_3", pTex.Get());

	pTex = nullptr;
	pTex = new ATexture;
	FilePath = CONTENT_PATH;
	FilePath += L"Texture\\1945\\p.png";
	pTex->Load(FilePath);
	AddAsset(L"p", pTex.Get());
	pTex = nullptr;
	pTex = new ATexture;
	FilePath = CONTENT_PATH;
	FilePath += L"Texture\\1945\\m.bmp";
	pTex->Load(FilePath);
	AddAsset(L"m", pTex.Get());
}
