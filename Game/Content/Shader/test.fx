#ifndef _TEST // n = not ifndef [test] test 만약 test 정의 되어있지 않다면
#define _TEST

// layout 정보
struct VS_IN
{
	// Semantic 이름에는 0이 생략 되어 있음 POSITION
	float3 vPos		: POSITION; // Semantic Layout에서 설명한 이름
	float2 vUV		: TEXCOORD;
	float4 vColor	: COLOR;
};

struct VS_OUT
{
	float4 vPosition	: SV_Position; // Rasterizer 로 보낼때, NDC 좌표
	float2 vUV			: TEXCOORD;
	float4 vColor		: COLOR;
};
/*
cbuffer TRANSFORM : register(b1) // 바인딩 될 상수 레지스터 번호 (b0)
{
	float4 g_ObjectPos;
}
*/
cbuffer TRANSFORM_DATA : register(b0) // 16byte의 배수로 구성 되어야함 꼭!!!
{
	float2 g_vOffset;
	float2 g_vPadding;
	float4 g_vColor; // 원의 색상정보 16byte
	float g_vZoom; // 4
	float3 g_vDummy2; // 12 (C++의 vDummy[3]와 대응)
};
VS_OUT VS_Test(VS_IN _input)
{
	VS_OUT output		= (VS_OUT) 0.f;
	
	// 원점 기준 정점 좌표(_input.vPos.xy)에 이동량(g_vOffset)을 더합니다.
	float2 vFinalPos1 = _input.vPos.xy * g_vZoom;
	float2 vFinalPos = vFinalPos1 + g_vOffset;
    
	output.vPosition = float4(vFinalPos, _input.vPos.z, 1.0f);
	output.vUV			= _input.vUV;
	output.vColor		= g_vColor;
	
	return output;
}

// 정점에서 반환한 값이 보간(Interpolation) 되어 픽셀이 쉐이더의 입력으로 돌아온다.
float4 PS_Test(VS_OUT _input) : SV_Target
{
	return _input.vColor;
	//return float4(1.f, 0.f, 0.f, 1.f);
}

#endif