#include "pch.h"
#include "AssetMgr.h"
#include "GLBExtractUtil.h"
#include "PathMgr.h"
#include "fezlvlLoder.h"
#include "LevelMgr.h"
namespace fs = filesystem;


void AssetMgr::Init()
{
	CreateEngineMesh();
	CreateEngineShader();
	CreateEngineTexture();
	CreateEngineMaterial();
	CreateEngineSprite();
	CreateEnginePrefab();
	CreatePrefabFromGLB();
	//LOAD(APrefab, L"Prefab\\untitled.fezts.pref");

}

void AssetMgr::CreateEngineMesh()
{
	Ptr<AMesh> pMesh = nullptr;
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
	pMesh->SetName(L"1_네모");
	pMesh->Create(arrVtx, 4, arrIdx, 6);
	AddAsset(L"q", pMesh.Get());


	arrIdx[0] = 0;
	arrIdx[1] = 1;
	arrIdx[2] = 2;
	arrIdx[3] = 3;
	arrIdx[4] = 0;
	pMesh = new AMesh;
	pMesh->SetName(L"2_네모_debug");
	pMesh->Create(arrVtx, 4, arrIdx, 5);
	AddAsset(L"q_debug", pMesh.Get());

	// 동구람위
	const int TRICOUNT = 40;
	const int VTXCOUNT = TRICOUNT + 1;
	const int IDXCOUNT = TRICOUNT * 3;
	Vtx oVtx[VTXCOUNT] = {};
	UINT oIdx[IDXCOUNT] = {};

	float fRadius = 0.5f; // Scale로 조절할 것이므로 기본 크기는 1로 설정
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
	pMesh->SetName(L"3_동구람위");
	pMesh->Create(oVtx, VTXCOUNT, oIdx, IDXCOUNT);
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
	pMesh->SetName(L"4_셈오1");
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
	pMesh->SetName(L"5_큐브");
	pMesh->Create(cubeVtx, 8, cubeIdx, 36);
	AddAsset(L"CubeMesh", pMesh.Get());

	// 원
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;

	Vtx v;
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.5f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float Theta = 0.f;
	float Radius = 0.5f;
	float Slice = 40.f;

	// 원의 테두리 정점 추가
	for (int i = 0; i < (int)Slice + 1; ++i)
	{
		v.vPos = Vec3(Radius * cosf(Theta), Radius * sinf(Theta), 0.f);
		//v.vUV = Vec2(0.5f, 0.5f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);

		// XM_2PI 360도
		Theta += XM_2PI / Slice;
	}

	// 인덱스
	for (int i = 0; i < (int)Slice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new AMesh;
	pMesh->SetName(L"6_원");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh", pMesh.Get());

	vecIdx.clear();
	for (int i = 0; i < (int)Slice + 1; ++i) {
		vecIdx.push_back(i + 1);
	}

	pMesh = new AMesh;
	pMesh->SetName(L"7_원_debug");
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset(L"CircleMesh_LineStrip", pMesh.Get());

	vector<fs::path> mesh_files;
	for (const auto& entry : fs::directory_iterator(CONTENT_PATH + L"Mesh\\"))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".mesh")
			mesh_files.push_back(entry.path());
	}

	for (const auto& mesh : mesh_files)
	{
		wstring fileName = mesh.stem().wstring();
		LOAD(AMesh, L"Mesh\\" + fileName + L".mesh");
	}
}

void AssetMgr::CreateEngineShader()
{
	Ptr<AGraphicShader> pShader = nullptr;

	pShader = new AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"Shader\\std2d.fx", "PS_Std2D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);

	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");


	AddAsset(L"Std2DShader", pShader.Get());

	/*
	* 
	*/
	pShader = new AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Std3D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);

	pShader->AddShaderParam(SHADER_PARAM::VEC4, 0, L"TintColor");
	pShader->AddShaderParam(SHADER_PARAM::TEX, 0, L"OutColor");


	AddAsset(L"Std3DShader", pShader.Get());

	// ===============
	// BillboardShader
	// ===============
	pShader = new AGraphicShader;
	pShader->SetName(L"BillboardShader");
	pShader->CreateVertexShader(L"Shader\\billboard.fx", "VS_Billboard");
	pShader->CreatePixelShader(L"Shader\\billboard.fx", "PS_Billboard");
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());


	// ============
	// SpriteShader
	// ============
	pShader = new AGraphicShader;
	pShader->SetName(L"SpriteShader");
	pShader->CreateVertexShader(L"Shader\\sprite.fx", "VS_Sprite");
	pShader->CreatePixelShader(L"Shader\\sprite.fx", "PS_Sprite");
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);

	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	// ==============
	// FlipbookShader
	// ==============
	pShader = new AGraphicShader;
	pShader->SetName(L"FlipbookShader");
	pShader->CreateVertexShader(L"Shader\\flipbook.fx", "VS_Flipbook");
	pShader->CreatePixelShader(L"Shader\\flipbook.fx", "PS_Flipbook");
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	// =============
	// TileMapShader
	// =============
	// 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해둔다

	pShader = new AGraphicShader;
	pShader->SetName(L"TileShader");
	pShader->CreateVertexShader(L"Shader\\tile.fx", "VS_Tile");
	pShader->CreatePixelShader(L"Shader\\tile.fx", "PS_Tile");
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	AssetMgr::GetInst()->AddAsset(pShader->GetName(), pShader.Get());

	pShader = new AGraphicShader;
	pShader->CreateVertexShader(L"Shader\\dbg.fx", "VS_Debug");
	pShader->CreatePixelShader(L"Shader\\dbg.fx", "PS_Debug");
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	AddAsset(L"DbgShader", pShader.Get());


}

void AssetMgr::CreateEngineTexture()
{
	Load<ATexture>(L"PlayerImage",L"Texture\\Character.png");
	
	Load<ATexture>(L"Fighter", L"Texture\\Fighter.bmp");

	Load<ATexture>(L"back_1", L"Texture\\1945\\back1.png");
	Load<ATexture>(L"p", L"Texture\\1945\\p.png");
	Load<ATexture>(L"m", L"Texture\\1945\\m.bmp");
	/*
	Load<ATexture>(
		pTex = nullptr;
		pTex = new ATexture;
		pTex->SetName(L"태양");
		FilePath = CONTENT_PATH;
		FilePath += L"Texture\\sola.png";
		pTex->Load(FilePath);
		AddAsset(L"sola", pTex.Get());
	);

	Load<ATexture>(
		pTex = nullptr;
		pTex = new ATexture;
		pTex->SetName(L"지구");
		FilePath = CONTENT_PATH;
		FilePath += L"Texture\\earth.png";
		pTex->Load(FilePath);
		AddAsset(L"earth", pTex.Get());
	);
	Load<ATexture>(
		pTex = nullptr;
		pTex = new ATexture;
		pTex->SetName(L"수성");
		FilePath = CONTENT_PATH;
		FilePath += L"Texture\\mercury.png";
		pTex->Load(FilePath);
		AddAsset(L"mercury", pTex.Get());
	);
	Load<ATexture>(
		pTex = nullptr;
		pTex = new ATexture;
		pTex->SetName(L"금성");
		FilePath = CONTENT_PATH;
		FilePath += L"Texture\\venus.png";
		pTex->Load(FilePath);
		AddAsset(L"venus", pTex.Get());
	);

	Load<ATexture>(
		pTex = nullptr;
		pTex = new ATexture;
		pTex->SetName(L"우주");
		FilePath = CONTENT_PATH;
		FilePath += L"Texture\\univers.jpeg";
		pTex->Load(FilePath);
		AddAsset(L"univers", pTex.Get());
	);

	Load<ATexture>(
		pTex = nullptr;
		pTex = new ATexture;
		pTex->SetName(L"슬라임");
		FilePath = CONTENT_PATH;
		FilePath += L"Texture\\slime_run.png";
		pTex->Load(FilePath);
		AddAsset(L"slime", pTex.Get());
	);
	*/
	Load<ATexture>(L"Link", L"Texture\\link.png");

	Load<ATexture>(L"TileAtlas", L"Texture\\TILE.bmp");
}

void AssetMgr::CreateEngineMaterial()
{
	// 재질 만들기
	Ptr<AMaterial> pMtrl = nullptr;

	pMtrl = new AMaterial;
	pMtrl->SetName(L"Std2DMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));

	// Parameter
	pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Fighter"));
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
	AddAsset(pMtrl->GetName(), pMtrl.Get());


	pMtrl = new AMaterial;
	pMtrl->SetName(L"MonsterMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));

	// Parameter
	pMtrl->SetScalar(VEC4_0, Vec4(1.f, 1.f, 1.f, 1.f));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"Fighter"));
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);
	AddAsset(pMtrl->GetName(), pMtrl.Get());

	/*
	pMtrl = new AMaterial;
	pMtrl->SetName(L"m_sola");
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"sola"));
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	pMtrl = new AMaterial;
	pMtrl->SetName(L"m_earth");
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"earth"));
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	pMtrl = new AMaterial;
	pMtrl->SetName(L"m_mercury");
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"mercury"));
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	pMtrl = new AMaterial;
	pMtrl->SetName(L"m_venus");
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"venus"));
	AddAsset(pMtrl->GetName(), pMtrl.Get());

	pMtrl = new AMaterial;
	pMtrl->SetName(L"m_univers");
	pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader"));
	pMtrl->SetTexture(TEX_0, Find<ATexture>(L"univers"));
	AddAsset(pMtrl->GetName(), pMtrl.Get());
	*/

	pMtrl = new AMaterial;
	pMtrl->SetName(L"DbgMtrl");
	pMtrl->SetShader(Find<AGraphicShader>(L"DbgShader"));
	pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_DEBUG);
	AddAsset(pMtrl->GetName(), pMtrl.Get());

	//Load<AMaterial>(L"Material\\Default Material_0.mtrl", L"Material\\Default Material_0.mtrl");
}

void AssetMgr::CreateEngineSprite()
{

	// =======
	// TileMap
	// =======
	Ptr<ATileMap> pTileMap = nullptr;

	pTileMap = new ATileMap;
	pTileMap->SetName(L"TestTileMap");
	pTileMap->SetRowCol(20, 20);
	pTileMap->SetTileSize(Vec2(64.f, 64.f));
	pTileMap->SetAtlas(FIND(ATexture, L"TileAtlas"));

	for (int i = 0; i < 20; ++i)
		for (int j = 0; j < 20; ++j)
			pTileMap->SetSprite(i, j, LOAD(ASprite, L"Sprite\\TileSprite_1.sprite"));

	AddAsset(pTileMap->GetName(), pTileMap.Get());

}

#include "Source\\Scripts\\CMissileScript.h"
void AssetMgr::CreateEnginePrefab()
{
	// Missile 역할 프리팹 제작
	//GameObject* pObject = new GameObject;
	//pObject->SetName(L"Missile");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CCollider2D);	
	//pObject->AddComponent(new CMissileScript);		

	//pObject->Transform()->SetRelativeScale(Vec3(10.f, 30.f, 1.f));

	//pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
	//pObject->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	//// 미사일 오브젝트를 프리팹으로 등록
	//Ptr<APrefab> pMissilePrefab = new APrefab;
	//pMissilePrefab->SetObject(pObject);
	//AddAsset(L"Prefab\\Missile.pref", pMissilePrefab.Get());

	//wstring FilePath = CONTENT_PATH + L"Prefab\\Missile.pref";
	//pMissilePrefab->Save(FilePath);
	//LOAD(APrefab, L"Prefab\\Missile.pref");
}

void AssetMgr::CreatePrefabFromGLB()
{
	//LOAD(APrefab, L"Prefab\\16_bit_doorao.fezao.pref");

	wstring path = L"GLB\\trileSets\\";
	for (const auto& entry : fs::directory_iterator(CONTENT_PATH + path))
	{

		wstring fileName = entry.path().stem().wstring();
		wstring _glbPath = CONTENT_PATH + path + fileName + L".glb";
		// 1. GLB 데이터 로드 (메쉬 + 재질 정보 포함)
		vector<GLB_MeshContainer> meshData;
		map<int, TrileMeta> trileMetaData;
		if (!GLBExtractUtil::Extract(string(_glbPath.begin(), _glbPath.end()), meshData, trileMetaData))
			continue;

		// 2. 루트 오브젝트 생성
		Ptr<GameObject> pRootObj = new GameObject;
		pRootObj->SetName(fileName);
		pRootObj->AddComponent(new CTransform);
		pRootObj->Transform()->SetRelativeScale(Vec3(1.f, 1.f, 1.f));
		//pRootObj->AddComponent(new CRigidbody2D);
		//pRootObj->Rigidbody2D()->SetType(BODY_TYPE::KINEMATIC);
		//pRootObj->AddComponent(new CCollider2D);

		for (size_t i = 0; i < meshData.size(); ++i)
		{
			int trileId = meshData[i].trileID;

			Ptr<GameObject> pChild = new GameObject;
			pChild->SetName(StrToWStr(meshData[i].meshName+"_tid"+to_string(trileId)));
			pChild->AddComponent(new CTransform);

			pChild->SetTrileMeta(trileMetaData.at(trileId));

			Vec3 scale;
			Quat rot;
			Vec3 pos;

			meshData[i].localMatrix.Decompose(scale, rot, pos);

			pChild->Transform()->SetRelativePos(pos);
			pChild->Transform()->SetRelativeScale(scale);
			// Quaternion → Euler 변환 필요
			Vec3 euler = ToEuler(rot); // ❗ 없으면 아래 방법
			pChild->Transform()->SetRelativeRot(euler);


			// 2. [Texture] 추출 및 물리 저장
			Ptr<ATexture> pTex = nullptr;
			if (meshData[i].hasTexture)
			{
				pTex = new ATexture;
				pTex->Create(meshData[i].width, meshData[i].height, DXGI_FORMAT_R8G8B8A8_UNORM, meshData[i].pixelData);

				// 엔진 전용 바이너리 포맷으로 저장
				wstring texRelPath = L"Texture\\GLB_Dump\\" + fileName + L"_" + to_wstring(i) + L".tex";
				pTex->Save(CONTENT_PATH + texRelPath);

				// 에셋 관리자에 등록
				pTex->SetName(texRelPath);
				pTex->SetRelativePath(texRelPath);
				AddAsset(pTex->GetName(), pTex.Get());
			}

			// 3. [Material] 생성 (본인의 CreateEngineMaterial 방식 활용)
			Ptr<AMaterial> pMtrl = new AMaterial;
			
			pMtrl->SetName(fileName + L"_Mtrl_" + to_wstring(i));
			pMtrl->SetShader(Find<AGraphicShader>(L"Std2DShader")); // 3D용 쉐이더 설정

			// 파라미터 세팅
			pMtrl->SetScalar(VEC4_0, meshData[i].baseColor);
			if (pTex != nullptr) pMtrl->SetTexture(TEX_0, pTex);

			pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_OPAQUE);

			// 중요: 에셋 매니저에 등록 후 .mtrl 파일로 저장해야 프리팹이 참조 가능함
			wstring mtrlRelPath = L"Material\\" + pMtrl->GetName() + L".mtrl";
			AddAsset(pMtrl->GetName(), pMtrl.Get());
			pMtrl->SetRelativePath(mtrlRelPath);
			pMtrl->Save(CONTENT_PATH + mtrlRelPath);

			// 4. [Mesh] 생성 및 등록
			Ptr<AMesh> pMesh = new AMesh;
			pMesh->SetName(fileName + L"_Mesh_" + to_wstring(i));
			pMesh->Create(meshData[i].vertices.data(), (UINT)meshData[i].vertices.size(),
				meshData[i].indices.data(), (UINT)meshData[i].indices.size());
			AddAsset(pMesh->GetName(), pMesh.Get());

			wstring meshRelPath = L"Mesh\\" + pMesh->GetName() + L".mesh";
			pMesh->SetRelativePath(meshRelPath);
			pMesh->Save(CONTENT_PATH + meshRelPath);

			// 5. [Render Component] 조립
			Ptr<CMeshRender> pMeshRender = new CMeshRender;
			pMeshRender->SetMesh(pMesh);
			pMeshRender->SetMaterial(pMtrl);
			pChild->AddComponent(pMeshRender.Get());

			pRootObj->AddChild(pChild);
		}

		// 6. 프리팹으로 포장
		Ptr<APrefab> pPrefab = new APrefab;
		pPrefab->SetObject(pRootObj);

		wstring prefKey = L"Prefab\\" + fileName + L".pref";
		AddAsset(prefKey, pPrefab.Get());
		pPrefab->Save(CONTENT_PATH + prefKey);
	}
}