#ifndef _TEST // n = not ifndef [test] test 만약 test 정의 되어있지 않다면
#define _TEST

// layout 정보
struct VS_IN
{
	// Semantic 은 꼭 맞을 필요 없음 필요한 것만 가져올 수 있음
	// Semantic 이름에는 0이 생략 되어 있음 POSITION
	float3 vPos		: POSITION; // Semantic Layout에서 설명한 이름
	float2 vUV		: TEXCOORD;
	float4 vColor	: COLOR;
};

struct VS_OUT
{
	// SV_xxx 는 약속된 값
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
// 꼭 버퍼가 같아야함
cbuffer TRANSFORM_DATA : register(b1) // 16byte의 배수로 구성 되어야함 꼭!!!
{
	float2 g_vOffset;
	float2 g_vPadding;
	float4 g_vColor; // 원의 색상정보 16byte
	float g_vZoom; // 4
	float3 g_vDummy2; // 12 (C++의 vDummy[3]와 대응)
};
cbuffer TRANSFORM : register(b0)
{
	//float4 g_ObjectPos;
	//float4 g_ObjectScale;
	//float4 g_ObjectRot;
	
	// row_major 행열 읽는 방식이 cpu gpu 달라서 맞춰줌
	// matrix == float4x4 같은 자료형
	row_major matrix g_matWorld;		// Local(Model) -> World Space
}
VS_OUT VS_Test(VS_IN _input)
{
	VS_OUT output		= (VS_OUT) 0.f;
	
	// 원점 기준 정점 좌표(_input.vPos.xy)에 이동량(g_vOffset)을 더합니다.
	output.vPosition = float4(_input.vPos.xy * g_vZoom + g_vOffset, _input.vPos.z, 1.0f);
	output.vUV			= _input.vUV;
	output.vColor		= g_vColor;
	
	return output;
}
VS_OUT VS_Test2(VS_IN _input)
{
	VS_OUT output = (VS_OUT) 0.f;
	
	/* 
		(x, y, z, 동차좌표)  X   ( scale.x		0		 0		 0	 )
								(	0		  scale.y	 0		 0	 )
								(	0			0	   scale.z	 0	 )
								(  pos.x	   pos.y    pos.z	 0	 )
	*/
	// 동차좌표 0으로 설정하면 이동 정보를 무시함
	output.vPosition = mul(float4(_input.vPos, 1.f/*동차좌표*/), g_matWorld);
	output.vUV = _input.vUV;
	output.vColor = _input.vColor;
    
	return output;
}
// 정점에서 반환한 값이 보간(Interpolation) 되어 픽셀이 쉐이더의 입력으로 돌아온다.
float4 PS_Test(VS_OUT _input) : SV_Target
{
	//return _input.vColor;
	return float4(1.f, 0.f, 0.f, 1.f);
}

// 정점에서 반환한 값이 보간(Interpolation) 되어 픽셀이 쉐이더의 입력으로 돌아온다.
float4 PS_Test2(VS_OUT _input) : SV_Target
{
	//return float4(1.f, 0.f, 0.f, 1.f);
	return _input.vColor;
}

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0); // texture 추출 도구
SamplerState g_sam_1 : register(s1); // texture 추출 도구
// 입력된 텍스쳐를 사용해서 픽셀쉐이더의 출력 색상으로 지정한다.
// 텍스쳐 코디네이션, UV 좌표계
float4 PS_Test3(VS_OUT _input) : SV_Target
{
	// 알파블렌딩의 핵심 기본 배경을 출력함!!!!!! 블렌드 쉐이프
	// 깊이 텍스쳐에 흔적이 남는다
	
	// texture 추출 도구 필요
	// 입력 UV는 정점에서 반환한 값을 보간 받아서 픽셀쉐이더에 입력됨
	return g_tex_0.Sample(g_sam_0, _input.vUV);
}
#endif