#include "pch.h"
#include "RenderMgr.h"
#include "TaskMgr.h"
#include "FezlvlLoder.h"
void CreateObject(GameObject* _Object, int LayerIdx)
{
	TaskInfo info = {};
	info.Type = TASK_TYPE::CREATE_OBJECT;
	info.Param_0 = (DWORD_PTR)_Object;
	info.Param_1 = LayerIdx;
	TaskMgr::GetInst()->AddTask(info);
}
void DrawDebugRect(Vec3 _Pos, Vec3 _Scale, Vec3 _Rot, Vec4 _Color, float _Duration, bool _DepthTest)
{
	DbgInfo info = {};
	info.Shape = DBG_SHAPE::RECT;
	info.Pos = _Pos;
	info.Scale = _Scale;
	info.Rotation = _Rot;
	info.Color = _Color;
	info.Age = 0.f;
	info.Life = _Duration;
	info.DepthTest = _DepthTest;
	RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugRect(const Matrix& _matWorld, Vec4 _Color, float _Duration, bool _DepthTest)
{
	DbgInfo info = {};
	info.Shape = DBG_SHAPE::RECT;
	info.matWorld = _matWorld;
	info.Color = _Color;
	info.Age = 0.f;
	info.Life = _Duration;
	info.DepthTest = _DepthTest;
	RenderMgr::GetInst()->AddDebugInfo(info);
}

void DrawDebugCircle(Vec3 _Pos, float _Radius, Vec4 _Color, float _Duration, bool _DepthTest)
{
	DbgInfo info = {};
	info.Shape = DBG_SHAPE::CIRCLE;
	info.Pos = _Pos;
	info.Scale = Vec3(_Radius * 2.f, _Radius * 2.f, 0.f);
	info.Rotation = Vec3(0.f, 0.f, 0.f);
	info.Color = _Color;
	info.Age = 0.f;
	info.Life = _Duration;
	info.DepthTest = _DepthTest;
	RenderMgr::GetInst()->AddDebugInfo(info);
}

void DebugPrint(const wchar_t* format, ...)
{
	wchar_t buffer[512];

	va_list args;
	va_start(args, format);
	vswprintf_s(buffer, 512, format, args); // 🔥 크기 명시
	va_end(args);

	OutputDebugStringW(buffer);
}

void SaveWString(FILE* _File, const wstring& _String)
{
	int Len = _String.length();
	fwrite(&Len, sizeof(int), 1, _File);		// 크기 읽음
	fwrite(_String.data(), sizeof(wchar_t), Len, _File); // 실제 문자열 읽음
}

wstring LoadWString(FILE* _File)
{
	int Len = 0;
	fread(&Len, sizeof(int), 1, _File);		// 크기 읽음

	wchar_t buff[255] = {};
	fread(buff, sizeof(wchar_t), Len, _File); // 실제 문자열 읽음

	return buff;
}

void SaveAssetRef(FILE* _File, Asset* _Asset)
{
	// Asset 이 Null 인지 아닌지 저장
	bool IsNull = _Asset;
	fwrite(&IsNull, sizeof(bool), 1, _File);

	// Asset 의 Key, RelativePath 저장
	if (nullptr != _Asset)
	{
		SaveWString(_File, _Asset->GetKey());
		SaveWString(_File, _Asset->GetRelativePath());
	}
}

float Saturate(float _Data)
{
	if (1.f < _Data)
		return 1.f;
	else if (_Data < 0.f)
		return 0.f;
	else
		return _Data;
}

bool IsValid(Ptr<GameObject>& _Object)
{
	if (nullptr == _Object || _Object->IsDead())
	{
		_Object = nullptr;
		return false;
	}
	return true;
}

string WStrToStr(const wstring& _wstr)
{
	if (_wstr.empty()) return {};
	int size_needed = WideCharToMultiByte(
		CP_UTF8, 0,
		_wstr.c_str(), (int)_wstr.size(),
		nullptr, 0,
		nullptr, nullptr);

	string str(size_needed, 0);

	WideCharToMultiByte(
		CP_UTF8, 0,
		_wstr.c_str(), (int)_wstr.size(),
		&str[0], size_needed,
		nullptr, nullptr);

	return str;
}

wstring StrToWStr(const string& _str)
{
	if (_str.empty()) return {};

	// 1. 변환 후 필요한 wstring의 길이를 측정 (문자 개수 기준)
	int size_needed = MultiByteToWideChar(
		CP_UTF8, 0,
		_str.c_str(), (int)_str.size(),
		nullptr, 0);

	// 2. 출력용 wstring 준비
	wstring wstr(size_needed, 0);

	// 3. 실제 변환 수행
	MultiByteToWideChar(
		CP_UTF8, 0,
		_str.c_str(), (int)_str.size(),
		&wstr[0], size_needed);

	return wstr;
}

wchar_t Buff[255] = {};
void ChangeLevel(const wstring& _NextLevelName)
{
	TaskInfo info = {};
	wcscpy_s(Buff, 255, _NextLevelName.c_str());

	info.Type = TASK_TYPE::CHANGE_LEVEL;
	info.Param_0 = (DWORD_PTR)Buff;

	TaskMgr::GetInst()->AddTask(info);
}

void ChangeLevelState(LEVEL_STATE _NextState)
{
	TaskInfo info = {};

	info.Type = TASK_TYPE::CHANGE_LEVEL_STATE;
	info.Param_0 = (DWORD_PTR)_NextState;

	TaskMgr::GetInst()->AddTask(info);
}

float Dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 ToEuler(const Quat& q)
{
	Vec3 euler;

	// yaw (Y)
	euler.y = atan2(2.0f * (q.w * q.y + q.x * q.z),
		1.0f - 2.0f * (q.y * q.y + q.x * q.x));

	// pitch (X)
	float sinp = 2.0f * (q.w * q.x - q.z * q.y);
	if (abs(sinp) >= 1)
		euler.x = copysign(XM_PIDIV2, sinp);
	else
		euler.x = asin(sinp);

	// roll (Z)
	euler.z = atan2(2.0f * (q.w * q.z + q.y * q.x),
		1.0f - 2.0f * (q.z * q.z + q.x * q.x));

	return euler;
}

#include "ALevel.h"
#include "GameObject.h"

#include "AssetMgr.h"
#include "Device.h"
#include "CollisionMgr.h"

#include "Source\\Scripts\\CPlayerScript.h"
#include "Source\\Scripts\\CCamMoveScript.h"
#include "Source\\Scripts\\CMonsterScript.h"
#include "Source/Scripts/CShockWaveScript.h"

void CreateTestLevel()
{
	/*
	{
		Ptr<ALevel> pLevel = LOAD(ALevel, L"Level\\TestLevel.lv");
		ChangeLevel(L"Level\\TestLevel.lv");
	}
	return;
	*/

	// 카메라 역할 Ojbect
	Ptr<GameObject> pCameraObj = new GameObject;


	// Player Object
	Ptr<GameObject> pObject = new GameObject;

	// Tile Object
	Ptr<GameObject> pTileObj = new GameObject;

	// Level Create
	Ptr<ALevel> pLevel = new ALevel;
	pLevel->SetName(L"Current Level");

	pLevel->GetLayer(0)->SetName(L"Default");
	pLevel->GetLayer(1)->SetName(L"Background");
	pLevel->GetLayer(2)->SetName(L"Tile");
	pLevel->GetLayer(3)->SetName(L"Player");
	pLevel->GetLayer(4)->SetName(L"PlayerProjectile");
	pLevel->GetLayer(5)->SetName(L"Enermy");
	pLevel->GetLayer(6)->SetName(L"EnermyProjectile");

	/*
	* Camera Start ======================================
	*/
	pCameraObj->SetName(L"MainCamera");
	pCameraObj->AddComponent(new CTransform);
	pCameraObj->AddComponent(new CCamera);

	Ptr<CCamMoveScript> pCCamMoveScript = new CCamMoveScript;
	//pCCamMoveScript->SetTarget(pObject);
	pCameraObj->AddComponent(pCCamMoveScript.Get());
	//pCameraObj->Camera()->LayerCheck(0); // 0번을 그려라
	pCameraObj->Camera()->LayerCheckAll();
	//pCameraObj->Camera()->LayerCheck(31); // 31 -> UI 레이어
	//pCameraObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pCameraObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pCameraObj->Camera()->SetFar(6000.f);
	pCameraObj->Camera()->SetFOV(90.f);
	pCameraObj->Camera()->SetOrthoScale(1.f);

	Vec2 vResolution = Device::GetInst()->GetRenderResolution();
	pCameraObj->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
	pCameraObj->Camera()->SetWidth(vResolution.x);

	pLevel->AddObject(0, pCameraObj);
	/*
	* Camera End ======================================
	*/

	/*
	* Light Start ======================================
	*/
	// 광원 오브젝트
	Ptr<GameObject> pLightObj = new GameObject;
	pLightObj->SetName(L"Light_1");
	pLightObj->AddComponent(new CTransform);
	pLightObj->AddComponent(new CLight2D);
	pLightObj->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	//pLightObj->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	pLightObj->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	//pLightObj->Light2D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pLightObj->Light2D()->SetRadius(300.f);
	pLightObj->Transform()->SetRelativePos(Vec3(-150.f, 0.f, 0.f));
	pLevel->AddObject(0, pLightObj);
	/*
	pLightObj = new GameObject;
	pLightObj->SetName(L"Light_2");
	pLightObj->AddComponent(new CTransform);
	pLightObj->AddComponent(new CLight2D);
	//pLightObj->Light2D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObj->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	pLightObj->Light2D()->SetLightColor(Vec3(0.3f, 0.3f, 1.f));
	//pLightObj->Light2D()->SetAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pLightObj->Light2D()->SetRadius(300.f);
	pLightObj->Transform()->SetRelativePos(Vec3(150.f, 0.f, 0.f));
	pLevel->AddObject(0, pLightObj);
	*/
	// 광원 추가

	/*
	* Light End ======================================
	*/

	/*
	* Monster Start ===============================================
	for (int i = 0; i < 5; ++i)
	{
		Ptr<GameObject> pMonster = new GameObject;

		pMonster->SetName(L"Monster");
		pMonster->AddComponent(new CTransform);
		//pMonster->AddComponent(new CSpriteRender);
		pMonster->AddComponent(new CMeshRender);
		pMonster->AddComponent(new CCollider2D);
		pMonster->AddComponent(new CMonsterScript);

		pMonster->Transform()->SetRelativePos(Vec3(300.f * (float)i, 0.f, 100.f));
		pMonster->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 0.f));
		//pMonster->SpriteRender()->SetSprite(FIND(ASprite, L"TileSprite_46"));
		pMonster->MeshRender()->SetMesh(FIND(AMesh, L"q"));
		pMonster->MeshRender()->SetMaterial(FIND(AMaterial, L"MonsterMtrl"));
		pLevel->AddObject(5, pMonster);
	}
	*/
	/*
	* Monster End ===============================================
	*/

	/*
	* Player Start ===============================================
	*/
	pObject->SetName(L"Player");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CFlipbookRender);
	pObject->AddComponent(new CClickable);
	pObject->AddComponent(new CRigidbody2D);
	pObject->Rigidbody2D()->SetType(BODY_TYPE::DYNAMIC);
	pObject->AddComponent(new CCollider2D);
	pObject->Collider2D()->SetActive(true);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
	pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));

	pObject->Collider2D()->SetOffset(Vec2(0.f, -0.1f));
	pObject->Collider2D()->SetScale(Vec2(0.25f, 0.5f));

	pObject->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveDown.flip"));
	pObject->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveLeft.flip"));
	pObject->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveUp.flip"));
	pObject->FlipbookRender()->AddFlipbook(LOAD(AFlipbook, L"Flipbook\\Link_MoveRight.flip"));
	pObject->FlipbookRender()->Play(1, 15.f, -1);

	//Ptr<GameObject> pCube = new GameObject;
	//pCube->SetName(L"Cube");
	//pCube->AddComponent(new CTransform);
	//pCube->AddComponent(new CMeshRender);
	//pCube->Transform()->SetRelativePos(Vec3(0.f, 0.f, 300.f));
	//pCube->Transform()->SetRelativeScale(Vec3(100.f, 300.f, 300.f));
	//pCube->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"CubeMesh"));
	//pCube->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));
	//pLevel->AddObject(3, pCube);
	/*
	Ptr<GameObject> pChild = new GameObject;
	pChild->SetName(L"Child");
	pChild->AddComponent(new CTransform);
	pChild->AddComponent(new CMeshRender);
	pChild->AddComponent(new CCollider2D);

	pChild->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
	pChild->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	pChild->Transform()->SetIndependentScale(true);

	pChild->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
	pChild->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	Ptr<GameObject> pChild2 = new GameObject;
	pChild2->SetName(L"Child2");
	pChild2->AddComponent(new CTransform);
	pChild2->AddComponent(new CMeshRender);
	pChild2->AddComponent(new CCollider2D);

	pChild2->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
	pChild2->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	pChild2->Transform()->SetIndependentScale(true);

	pChild2->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
	pChild2->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	Ptr<GameObject> pChild3 = new GameObject;
	pChild3->SetName(L"Child3");
	pChild3->AddComponent(new CTransform);
	pChild3->AddComponent(new CMeshRender);
	pChild3->AddComponent(new CCollider2D);

	pChild3->Transform()->SetRelativePos(Vec3(-200.f, 0.f, 0.f));
	pChild3->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));
	pChild3->Transform()->SetIndependentScale(true);

	pChild3->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q"));
	pChild3->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"Std2DMtrl"));

	pChild->AddChild(pChild2);
	pChild2->AddChild(pChild3);

	pLightObj = new GameObject;
	pLightObj->SetName(L"Light 3");
	pLightObj->AddComponent(new CTransform);
	pLightObj->AddComponent(new CLight2D);

	pLightObj->Light2D()->SetLightType(LIGHT_TYPE::SPOT);
	pLightObj->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pLightObj->Light2D()->SetRadius(400.f);
	pLightObj->Light2D()->SetAngle(XM_PI / 4.f);

	pLightObj->Transform()->SetRelativePos(pObject->Transform()->GetRelativePos());
	*/

	Ptr<CPlayerScript> playerScript = new CPlayerScript;
	//playerScript->SetTarget(pMonster);
	//playerScript->SetLight(pLightObj);
	pObject->AddComponent(playerScript.Get());

	// Player 와 Child 부모자식 연결
	//pObject->AddChild(pChild);
	//pObject->AddChild(pLightObj);
	pLevel->AddObject(3, pObject);
	/*
	* Player End ===============================================
	*/


	/*
	// 오브젝트 생성
	pObject = new GameObject;
	//pObject->SetName(L"Player");
	pObject->SetName(L"sola");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlayerScript);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(500.f, 500.f));
	pObject->BillboardRender()->SetTexture(AssetMgr::GetInst()->Find<ATexture>(L"sola"));
	pLevel->AddObject(0, pObject);
	pObject = new GameObject;
	pObject->SetName(L"mercury");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlanetControllerScript);
	pObject->Transform()->SetRelativePos(Vec3(100.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(200.f, 200.f));
	pLevel->AddObject(0, pObject);

	pObject = new GameObject;
	pObject->SetName(L"earth");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlanetControllerScript);
	pObject->Transform()->SetRelativePos(Vec3(200.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(200.f, 200.f));
	pLevel->AddObject(0, pObject);

	pObject = new GameObject;
	pObject->SetName(L"venus");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CBillboardRender);
	pObject->AddComponent(new CPlanetControllerScript);
	pObject->Transform()->SetRelativePos(Vec3(300.f, 0.f, 300.f));
	pObject->BillboardRender()->SetBillboardScale(Vec2(200.f, 200.f));
	pLevel->AddObject(0, pObject);
	*/

	/*
* Ground Start ===============================================
*/
	Ptr<GameObject> pGround = new GameObject;
	pGround->SetName(L"그라운드");

	pGround->AddComponent(new CTransform);
	pGround->AddComponent(new CMeshRender);

	pGround->AddComponent(new CRigidbody2D);   // 🔥 중요
	pGround->Rigidbody2D()->SetType(BODY_TYPE::STATIC);

	pGround->AddComponent(new CCollider2D);
	pGround->Collider2D()->SetActive(true);
	// 위치
	pGround->Transform()->SetRelativePos(Vec3(0.f, -300.f, 100.f));
	pGround->Transform()->SetRelativeScale(Vec3(1000.f, 50.f, 1.f));

	// 메쉬
	pGround->MeshRender()->SetMesh(FIND(AMesh, L"q"));
	//pGround->MeshRender()->SetMaterial(FIND(AMaterial, L"Std2DMtrl"));

	// 콜라이더 크기
	pGround->Collider2D()->SetScale(Vec2(1.f, 1.f));

	// 🔥 Static 설정
	//pGround->Rigidbody2D()->SetStatic(true);

	pLevel->AddObject(2, pGround);
	/*
	* Ground End ===============================================
	*/

	/*
	* Tile Start ===============================================
	pTileObj->SetName(L"TileObj");
	pTileObj->AddComponent(new CTransform);
	pTileObj->AddComponent(new CCollider2D);
	pTileObj->AddComponent(new CTileRender);

	pTileObj->Transform()->SetRelativePos(Vec3(-640.f, 640.f, 500.f));
	pTileObj->TileRender()->SetTileMap(FIND(ATileMap, L"TestTileMap"));

	pLevel->AddObject(2, pTileObj);
	*/
	/*
	* Tile End ===============================================
	*/

	/*
	pObject = new GameObject;
	pObject->SetName(L"배경");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 1000.f));
	pObject->Transform()->SetRelativeScale(Vec3(1600.0f, 900.f, 100.f));

	pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"q").Get());
	pObject->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"m_univers"));

	pLevel->AddObject(1, pObject);
	*/

	/*
	// 3. GameObject 생성 (3개)
	g_Object3 = new GameObject;
	g_Object3->AddComponent(new CTransform);
	g_Object3->AddComponent(new CMeshRender);
	//g_Object3->AddComponent(new CPlayerScript);

	g_Object3->Transform()->SetRelativePos(Vec3(-0.5f, 0.f, 0.f));
	g_Object3->Transform()->SetRelativeScale(Vec3(0.2f, 0.2f, 1.f)); // 반지름 역할

	g_Object3->MeshRender()->SetMesh((AMesh*)AssetMgr::GetInst()->Find<AMesh>(L"c").Get());
	g_Object3->MeshRender()->SetShader((AGraphicShader*)AssetMgr::GetInst()->Find<AGraphicShader>(L"shader2").Get());
	pLevel->AddObject(0, pObject);
	*/
	//pLevel->Init();

	/*
	0 Default
	1 Background
	2 Tile
	3 Player
	4 PlayerProjectile
	5 Enermy
	6 EnermyProjectile
	*/
	pLevel->CheckCollisionLayer(3, 5); // 나 == 적
	pLevel->CheckCollisionLayer(3, 6); // 나 == m탄
	pLevel->CheckCollisionLayer(4, 5); // p탄 == 적
	pLevel->CheckCollisionLayer(5, 6); // 적 == m탄

	pLevel->SetChanged();

	AssetMgr::GetInst()->AddAsset(L"TestLevel", pLevel.Get());

	// 레벨을 파일로 변경
	//wstring ContentPath = CONTENT_PATH;
	//pLevel->Save(ContentPath + L"Level\\TestLevel.lv");

	FezlvlLoder::Load(L"Prefab\\untitled.fezts.pref", L"levels\\trial\\arch.fezlvl.json", pLevel.Get());
	//FezlvlLoder::Load(L"Prefab\\untitled.fezts.pref", L"levels\\trial\\big_tower.fezlvl.json", pLevel.Get());
	//FezlvlLoder::Load(L"Prefab\\untitled.fezts.pref", L"levels\\trial\\memory_core.fezlvl.json", pLevel.Get());
	//FezlvlLoder::Load(L"Prefab\\untitled.fezts.pref", L"levels\\trial\\nature_hub.fezlvl.json", pLevel.Get());
	//FezlvlLoder::Load(L"Prefab\\industrial.fezts.pref", L"levels\\trial\\pivot_watertower.fezlvl.json", pLevel.Get());
	//FezlvlLoder::Load(L"Prefab\\industrial.fezts.pref", L"levels\\trial\\pivot_watertower.fezlvl.json", pLevel.Get());

	// ==================
	// PostProcess 테스트
	// ==================
	Ptr<GameObject> PostProcessObj = new GameObject;
	PostProcessObj->SetName(L"Postproces");

	PostProcessObj->AddComponent(new CTransform);
	PostProcessObj->AddComponent(new CMeshRender);
	PostProcessObj->AddComponent(new CShockWaveScript);

	PostProcessObj->Transform()->SetRelativePos(Vec3(-500.f, 0.f, -100.f));
	PostProcessObj->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

	PostProcessObj->MeshRender()->SetMesh(FIND(AMesh, L"q"));
	PostProcessObj->MeshRender()->SetMaterial(FIND(AMaterial, L"DistortionMtrl"));

	pLevel->AddObject(0, PostProcessObj);

	// 레벨을 변경
	ChangeLevel(L"TestLevel");


	//pLevel = new ALevel;

	//AssetMgr::GetInst()->AddAsset(L"JsonLevel", pLevel.Get());

	//ChangeLevel(L"JsonLevel");
}