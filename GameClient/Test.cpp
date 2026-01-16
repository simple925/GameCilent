#include "pch.h"
#include "Device.h"
#include "PathMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "AMesh.h"

AMesh* g_RectMesh;

// 그래픽 파이프라인 문서
//https://learn.microsoft.com/ko-kr/windows/uwp/graphics-concepts/graphics-pipeline



// Contant Buffer(상수버퍼)
ComPtr<ID3D11Buffer>		g_CB;



// InputLayout - 하나의 정점이 어떻게 구성되어 있는지에 대한 정보
ComPtr<ID3D11InputLayout>	g_Layout;

// VertexShader
// HLSL 로 작성한 VS 함수를 컴파일한 어셈블리코드를 저장시킬 버퍼
ComPtr<ID3DBlob>			g_VSBlob;
ComPtr<ID3D11VertexShader>	g_VS;



// PixelShader
ComPtr<ID3DBlob>			g_PSBlob;
ComPtr<ID3D11PixelShader>	g_PS;

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

Vtx arrVtx[VTXCOUNT] = {};
UINT arrIdx[IDXCOUNT] = {};
Vec4 g_vTargetColor = Vec4(1.f, 1.f, 1.f, 1.f);
// 원의 중심 위치와 이동 속도 (NDC 좌표계 기준)
Vec3 g_vCenterPos = Vec3(0.f, 0.f, 0.f);
Vec2 g_vVelocity = Vec2(0.5f, 0.3f); // X축, Y축 이동 속도
float g_vZoom = 1.f;
float g_fRadius = 0.5f;             // 원의 반지름

int TestInit()
{

	// NDC 좌표계, 중심이 0,0, 위 아래 좌 우 로 +- 1 범위	
	// 0 -- 1
	// |  \ |
	// 3 -- 2
	/*
	arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	arrVtx[0].vUV = Vec2(0.f, 0.f);
	arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 0.f);

	arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	arrVtx[1].vUV = Vec2(0.f, 0.f);
	arrVtx[1].vColor = Vec4(0.f, 0.f, 1.f, 0.f);

	arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	arrVtx[2].vUV = Vec2(0.f, 0.f);
	arrVtx[2].vColor = Vec4(0.f, 1.f, 0.f, 0.f);

	arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	arrVtx[3].vUV = Vec2(0.f, 0.f);
	arrVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 0.f);
	*/
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

	wstring Path = PathMgr::GetInst()->GetContentPath(L"Shader\\test.fx");

	// 엔트리포인트
	if (FAILED(D3DCompileFromFile(Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_Test", "vs_5_0", D3D10_SHADER_DEBUG, 0, g_VSBlob.GetAddressOf(), nullptr))) {
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), nullptr, g_VS.GetAddressOf()))) {
		return E_FAIL;
	}

	if (FAILED(D3DCompileFromFile(Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_Test", "ps_5_0", D3D10_SHADER_DEBUG, 0, g_PSBlob.GetAddressOf(), nullptr))) {
		return E_FAIL;
	}

	// 컴파일한 Shader 코드로 PixelShader 만들기
	if (FAILED(DEVICE->CreatePixelShader(g_PSBlob->GetBufferPointer()
		, g_PSBlob->GetBufferSize(), nullptr
		, g_PS.GetAddressOf())))
	{
		return E_FAIL;
	}

	// ---12--- --8-- ---16---
	// Position   UV     Color
	// Input Layout 생성하기
	D3D11_INPUT_ELEMENT_DESC InputDesc[3] = {};

	InputDesc[0].SemanticName = "POSITION";
	InputDesc[0].SemanticIndex = 0;							// Semantic 이름이 중복되는 경우, 구별하기 위한 숫자
	InputDesc[0].AlignedByteOffset = 0;							// 메모리 시작 위치(offset)
	InputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// offset 으로부터 크기
	InputDesc[0].InputSlot = 0;							// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[0].InstanceDataStepRate = 0;

	InputDesc[1].SemanticName = "TEXCOORD";
	InputDesc[1].SemanticIndex = 0;							// Semantic 이름이 중복되는 경우, 구별하기 위한 숫자
	InputDesc[1].AlignedByteOffset = 12;							// 메모리 시작 위치(offset)
	//InputDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;	// offset 으로부터 크기
	InputDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;	// offset 으로부터 크기
	InputDesc[1].InputSlot = 0;							// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[1].InstanceDataStepRate = 0;

	InputDesc[2].SemanticName = "COLOR";
	InputDesc[2].SemanticIndex = 0;							// Semantic 이름이 중복되는 경우, 구별하기 위한 숫자
	InputDesc[2].AlignedByteOffset = 20;							// 메모리 시작 위치(offset)
	//InputDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// offset 으로부터 크기
	InputDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// offset 으로부터 크기
	InputDesc[2].InputSlot = 0;							// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// 설명하는 정점이 들어있는 buffer의 위치
	InputDesc[2].InstanceDataStepRate = 0;

	DEVICE->CreateInputLayout(InputDesc, 3, g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), g_Layout.GetAddressOf());

	return S_OK;


}



void TestTick()
{
	/*
	// 사각형을 움직인다.
	// SystemMemory(전역변수) 의 정점 좌표를 수정
	for (int i = 0; i < 4; ++i) {
		arrVtx[i].vPos.x += 0.1f * DT;
	}

	// 전역변수에 들어있는 정점 정보를 정점버퍼로 복사
	// ID3D11Resource gpu 메모리에서 사용됨
	D3D11_MAPPED_SUBRESOURCE tMapSub = {};
	CONTEXT->Map(g_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMapSub); // gpu 로 보내기위한 운송장 작성? 

	memcpy(tMapSub.pData, arrVtx, sizeof(Vtx) * 4);

	CONTEXT->Unmap(g_VB.Get(), 0); // gpu로 운송
	*/

	//if (GetAsyncKeyState('W') & 0x8000)
	// key event manege (Pressed, TAP, Released, None)
	// key event state manege
	if (KEY_PRESSED(KEY::RIGHT))
	{
		g_vCenterPos.x += 0.1f * DT;
	}

	if (KEY_PRESSED(KEY::LEFT))
	{
		g_vCenterPos.x -= 0.1f * DT;
	}

	if (KEY_PRESSED(KEY::UP))
	{
		g_vCenterPos.y += 0.1f * DT;
	}

	if (KEY_PRESSED(KEY::DOWN))
	{
		g_vCenterPos.y -= 0.1f * DT;
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
	
}

void TestRender()
{
	// 이전에 그려진 그림을 지운다.
	// 랜더타겟은
	Device::GetInst()->ClearTarget();

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

	// HLSL(High Level Shader Language) 5.0
	// Graphic Pipeline
	// 1. IA(Input Assembler)
	// 정점을 프로그래머가 설계하기 때문에, 전달 한 버텍스 버퍼안에서, 하나의 정점의 단위크기를 알려줘야 한다.
	

	// 상수버퍼설정
	CONTEXT->VSSetConstantBuffers(0/*상수버퍼를 바인딩할 레지스터 번호*/, 1, g_CB.GetAddressOf());

	// 정점으로 구성할 도형의 상태, 모양
	// TriangList 는 점 3개를 이어서 만든 삼각형(내부까지 색을 채움)*****************************
	CONTEXT->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CONTEXT->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	// 정점 하나 안에 들어있는 데이터를 구분해주는 정보
	CONTEXT->IASetInputLayout(g_Layout.Get());

	// 랜더링에 필요한 리소스 입력
	// 2. Vertex Shader(함수) - 정점 당 연산 수행 정점쉐이더
	// HLSL(High Level 
	CONTEXT->VSSetShader(g_VS.Get(), nullptr, 0);

	// 3. Rasterizer State - 정점 쉐이더에서 넘겨준 픽셀쉐이더를 호출할 함수를 연결
	// 정점쉬이더에서 반환한 정점의 취치를 NDC 좌표
	// 기준으로 들어오는 픽셀을 계산해서 픽셀 쉐이더로 연계
	// nullptr 입력: 기본 설정 옵션으로 설정
	// CULL_MODE : CULL_BACK
	// FILL_MODE : SOLID
	CONTEXT->RSSetState(nullptr);

	// 4. Pixel Shader
	// 레스터라이저를 거쳐서, 호출될 픽셀마다 실행되는 함수
	// 픽셀 쉐이더에서 리턴한 값이, 렌더타겟 내에서의 해당 픽셀 위체에 색상이 출력된다.
	CONTEXT->PSSetShader(g_PS.Get(), nullptr, 0);

	// OM (Output Merge State)
	// 픽셀 쉐이더에서 리턴한 값울, OM 단계에 연결되어있는 RenderTarget, DepthStencilTarget에 기록한다.
	// DepthStencilState - 깊이 비교	

	g_RectMesh->Render();
}

void TestRelease()
{
	if (nullptr != g_RectMesh) delete g_RectMesh;
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