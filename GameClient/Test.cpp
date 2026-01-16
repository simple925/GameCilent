#include "pch.h"
#include "Device.h"
#include "PathMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "AMesh.h"
#include "AGraphicShader.h"
AMesh* g_RectMesh;

// 그래픽 파이프라인 문서
//https://learn.microsoft.com/ko-kr/windows/uwp/graphics-concepts/graphics-pipeline


// 파이프라인
AGraphicShader* g_Shader;

// Contant Buffer(상수버퍼)
ComPtr<ID3D11Buffer>		g_CB;




struct tTransform {
	Vec2 vOffset;   // 8byte 원의 중심 위치 (x, y)
	Vec2 vPadding;  // 8byte 16바이트 정렬을 위한 패딩
	Vec4 vColor;	// 16byte 추가
	float vZoom;
	float vDummy[3];// 12 (여기까지 48) -> 16의 배수 OK!
};

//Vtx arrVtx[6] = {};
const int TRICOUNT = 100;
const int VTXCOUNT = TRICOUNT + 1; // 중심점 1개 + 외곽 점들
const int IDXCOUNT = TRICOUNT * 3;

//Vtx arrVtx[VTXCOUNT] = {};
//UINT arrIdx[IDXCOUNT] = {};
//Vec4 g_vTargetColor = Vec4(1.f, 1.f, 1.f, 1.f);
// 원의 중심 위치와 이동 속도 (NDC 좌표계 기준)
//Vec3 g_vCenterPos = Vec3(0.f, 0.f, 0.f);
//Vec2 g_vVelocity = Vec2(0.5f, 0.3f); // X축, Y축 이동 속도
//float g_vZoom = 1.f;
//float g_fRadius = 0.5f;             // 원의 반지름

struct Circle
{
	Vec2 vCenter;   // 중심 (NDC)
	float fRadius;
	Vec4 vColor;
	float fZoom;
	bool bSelected;
};
vector<Circle> g_vecCircle;
int g_iSelectedCircle = -1;

int TestInit()
{
	const int TRICOUNT = 100;
	const int VTXCOUNT = TRICOUNT + 1;
	const int IDXCOUNT = TRICOUNT * 3;

	static Vtx arrVtx[VTXCOUNT];
	static UINT arrIdx[IDXCOUNT];
	// NDC 좌표계, 중심이 0,0, 위 아래 좌 우 로 +- 1 범위	
	// 1. 원점(0,0) 기준으로 정점 딱 한 번만 만들기
	float fRadius = 0.2f; // 원의 반지름

	// 0번 정점: 원의 중심
	arrVtx[0].vPos = Vec3(0.f, 0.f, 0.f); // 중심
	arrVtx[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f); // 하얀색
	arrVtx[0].vUV = Vec2(0.5f, 0.5f);

	// 외곽 정점들 계산
	for (int i = 0; i < TRICOUNT; ++i)
	{
		// 각도 계산 (360도를 삼각형 개수로 나눔)
		float fAngle = (XM_2PI / (float)TRICOUNT) * (float)i;

		// sin 앞에 '-'를 붙여서 시계 방향으로 정점을 생성합니다.
		arrVtx[i + 1].vPos = Vec3(cosf(fAngle) * fRadius, -sinf(fAngle) * fRadius, 0.f);
		arrVtx[i + 1].vColor = Vec4(1.f, 1.f, 1.f, 0.f);
		//arrVtx[i + 1].vUV = Vec2(0.f, 0.f);
	}
	// 인덱스 설정 (중심점 - 현재외곽 - 다음외곽)
	for (int i = 0; i < TRICOUNT; ++i)
	{
		arrIdx[i * 3 + 0] = 0;             // 항상 중심점
		arrIdx[i * 3 + 1] = i + 1;         // 현재 정점
		arrIdx[i * 3 + 2] = i + 2;         // 다음 정점

		// 마지막 삼각형은 다시 첫 번째 외곽 정점으로 연결
		if (i == TRICOUNT - 1)
			arrIdx[i * 3 + 2] = 1;
	}

	g_RectMesh = new AMesh;
	g_RectMesh->Create(arrVtx, sizeof(Vtx) * VTXCOUNT, arrIdx, sizeof(UINT) * IDXCOUNT);


	D3D11_BUFFER_DESC CBDesc = {};
	CBDesc.ByteWidth = sizeof(tTransform);

	// cpu를 통해서 버퍼의 내용을 쓰거나, 읽을 수 있는지
	// D3D11_USAGE_DEFAULT + 0
	// 버퍼를 생성한 이후에 수정할 수 없다.
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 버퍼용도
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// 처음 버퍼 생성할때 전달시킬 데이터의 시작주소를 Sub 구조체에 담아서 CreateBuffer 함수에 넣어준다.
	//tSub.pSysMem = arrIdx;
	if (FAILED(DEVICE->CreateBuffer(&CBDesc, nullptr, g_CB.GetAddressOf()))) {
		return E_FAIL;
	}

	// VertexShader
	// 컴파일할 VertexShader 함수가 작성 되어있는 파일의 절대 경로

	g_Shader = new AGraphicShader;
	g_Shader->CreateVertexShader(L"Shader\\test.fx", "VS_Test");
	g_Shader->CreatePixelShader(L"Shader\\test.fx", "PS_Test");

	g_vecCircle.clear();

	for (int i = 0; i < 3; ++i)
	{
		Circle c = {};
		c.vCenter = Vec2(-0.5f + i * 0.5f, 0.f);
		c.fRadius = 0.2f;
		c.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		c.fZoom = 1.f;
		c.bSelected = false;

		g_vecCircle.push_back(c);
	}

	return S_OK;


}

int PickCircle(const Vec2& mouseNDC)
{
	for (int i = 0; i < g_vecCircle.size(); ++i)
	{
		Vec2 diff = mouseNDC - g_vecCircle[i].vCenter;
		float distSq = diff.LengthSquared();

		float r = g_vecCircle[i].fRadius * g_vecCircle[i].fZoom;

		if (distSq <= r * r)
			return i;
	}
	return -1;
}
Vec2 GetMouseNDC()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(Device::GetInst()->GetHwnd(), &pt);

	float width = Device::GetInst()->GetRenderResol().x;
	float height = Device::GetInst()->GetRenderResol().y;

	float x = (pt.x / width) * 2.f - 1.f;
	float y = 1.f - (pt.y / height) * 2.f;

	return Vec2(x, y);
}

void TestTick()
{
	/*
	//if (GetAsyncKeyState('W') & 0x8000)
	// key event manege (Pressed, TAP, Released, None)
	// key event state manege
	if (KEY_PRESSED(KEY::RIGHT))
	{
		g_vCenterPos.x += 1.0f * DT;
	}

	if (KEY_PRESSED(KEY::LEFT))
	{
		g_vCenterPos.x -= 1.0f * DT;
	}

	if (KEY_PRESSED(KEY::UP))
	{
		g_vCenterPos.y += 1.0f * DT;
	}

	if (KEY_PRESSED(KEY::DOWN))
	{
		g_vCenterPos.y -= 1.0f * DT;
	}

	if (KEY_PRESSED(KEY::W))
	{
		g_vZoom += 0.1f * DT;
	}
	if (KEY_PRESSED(KEY::S))
	{
		g_vZoom -= 0.1f * DT;
	}
	// 1. 위치 업데이트 (기존 로직)
	//g_vCenterPos.x += g_vVelocity.x * DT;
	//g_vCenterPos.y += g_vVelocity.y * DT;

	// 2. 벽 체크 및 속도 반전 (기존 로직)
	if (abs(g_vCenterPos.x) + 0.2f > 1.f) g_vVelocity.x *= -1.f;
	if (abs(g_vCenterPos.y) + 0.2f > 1.f) g_vVelocity.y *= -1.f;

	// 2. 시간에 따른 부드러운 색상 계산
	static float fAccTime = 0.f;
	fAccTime += DT;

	// sin 함수 결과값(-1 ~ 1)을 0 ~ 1 범위로 변환하는 공식: (sin(x) * 0.5f) + 0.5f
	// 채널마다 속도를 다르게 주면 더 다채로운 색상이 나옵니다.
	float r = sinf(fAccTime * 1.5f) * 0.5f + 0.5f;
	float g = sinf(fAccTime * 2.1f) * 0.5f + 0.5f;
	float b = sinf(fAccTime * 0.7f) * 0.5f + 0.5f;

	g_vTargetColor = Vec4(r, g, b, 1.f);

	// ★ 핵심: 상수 버퍼에 현재 좌표(g_vCenterPos)를 써서 GPU로 보냄
	tTransform tr = {};
	tr.vOffset = Vec2(g_vCenterPos.x, g_vCenterPos.y);
	tr.vColor = g_vTargetColor;
	tr.vZoom = g_vZoom;
	D3D11_MAPPED_SUBRESOURCE tMapSub = {};
	// g_VB가 아니라 g_CB를 Map 합니다!
	if (SUCCEEDED(CONTEXT->Map(g_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMapSub))) {
		memcpy(tMapSub.pData, &tr, sizeof(tTransform));
		CONTEXT->Unmap(g_CB.Get(), 0);
	}
	*/
	static float fAccTime = 0.f;
	fAccTime += DT;
	float r = sinf(fAccTime * 1.5f) * 0.5f + 0.5f;
	float g = sinf(fAccTime * 2.1f) * 0.5f + 0.5f;
	float b = sinf(fAccTime * 0.7f) * 0.5f + 0.5f;
	//g_vTargetColor = Vec4(r, g, b, 1.f);
	if (KEY_TAP(KEY::LBUTTON))
	{
		Vec2 mouseNDC = GetMouseNDC();
		g_iSelectedCircle = PickCircle(mouseNDC);

		// 선택 상태 갱신
		for (int i = 0; i < g_vecCircle.size(); ++i)
			g_vecCircle[i].bSelected = (i == g_iSelectedCircle);
	}

	if (g_iSelectedCircle != -1)
	{
		Circle& c = g_vecCircle[g_iSelectedCircle];
		c.vColor = Vec4(r, g, b, 1.f);
		if (KEY_PRESSED(KEY::LEFT)) c.vCenter.x -= 1.f * DT;
		if (KEY_PRESSED(KEY::RIGHT)) c.vCenter.x += 1.f * DT;
		if (KEY_PRESSED(KEY::UP))    c.vCenter.y += 1.f * DT;
		if (KEY_PRESSED(KEY::DOWN))  c.vCenter.y -= 1.f * DT;

		if (KEY_PRESSED(KEY::W)) c.fZoom += 0.5f * DT;
		if (KEY_PRESSED(KEY::S)) c.fZoom -= 0.5f * DT;
	}
}



void TestRender()
{

	// 이전에 그려진 그림을 지운다.
	// 랜더타겟은
	Device::GetInst()->ClearTarget();
	

	// HLSL(High Level Shader Language) 5.0
	// Graphic Pipeline
	// 1. IA(Input Assembler)
	// 정점을 프로그래머가 설계하기 때문에, 전달 한 버텍스 버퍼안에서, 하나의 정점의 단위크기를 알려줘야 한다.
	

	// 상수버퍼설정
	CONTEXT->VSSetConstantBuffers(0/*상수버퍼를 바인딩할 레지스터 번호*/, 1, g_CB.GetAddressOf());

	g_Shader->Binding();

	for (Circle& c : g_vecCircle)
	{
		// 외곽선
		if (c.bSelected)
		{
			tTransform outline = {};
			outline.vOffset = c.vCenter;
			outline.vZoom = c.fZoom * 1.08f;   // 살짝 크게
			outline.vColor = Vec4(1, 0, 0, 1);  // 빨간 테두리

			D3D11_MAPPED_SUBRESOURCE sub;
			CONTEXT->Map(g_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
			memcpy(sub.pData, &outline, sizeof(outline));
			CONTEXT->Unmap(g_CB.Get(), 0);

			g_RectMesh->Render();
		}

		//  본체
		tTransform body = {};
		body.vOffset = c.vCenter;
		body.vZoom = c.fZoom;
		body.vColor = c.vColor;

		D3D11_MAPPED_SUBRESOURCE sub;
		CONTEXT->Map(g_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
		memcpy(sub.pData, &body, sizeof(body));
		CONTEXT->Unmap(g_CB.Get(), 0);

		g_RectMesh->Render();
	}
	//g_RectMesh->Render();
}

void TestRelease()
{
	if (nullptr != g_RectMesh) delete g_RectMesh;
	if (nullptr != g_Shader) delete g_Shader;
}

int TestFunc()
{
	static bool bInit = false;
	if (!bInit)
	{
		if (FAILED(TestInit()))
			return E_FAIL;

		bInit = true;
	}

	TestTick();

	TestRender();

	return S_OK;
}