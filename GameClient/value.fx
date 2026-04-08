#ifndef _VALUE
#define _VALUE
#include "struct.fx"
SamplerState g_sam_0 : register(s0); // texture 추출 도구
SamplerState g_sam_1 : register(s1); // texture 추출 도구
/*
cbuffer TRANSFORM : register(b1) // 바인딩 될 상수 레지스터 번호 (b0)
{
	float4 g_ObjectPos;
}
*/
// HLSL 5.0 : 4096
cbuffer TRANSFORM : register(b0)
{
	//float4 g_ObjectPos;
	//float4 g_ObjectScale;
	//float4 g_ObjectRot;
	
	// row_major 행열 읽는 방식이 cpu gpu 달라서 맞춰줌
	// matrix == float4x4 같은 자료형
	row_major matrix g_matWorld; // Local(Model) -> World Space
	row_major matrix g_matView; // World -> Camea(View) Space
	row_major matrix g_matProj; // Camera(View) -> Projection Space
}
// HLSL 5.0 : 4096
// 꼭 버퍼가 같아야함
cbuffer MATERIAL : register(b1) // 16byte의 배수로 구성 되어야함 꼭!!!
{
	//int iArr[4]; // 16으로 봐서 16 * 4 메모리 공간임
	int g_int_0;
	int g_int_1;
	int g_int_2;
	int g_int_3;
	
	float g_float_0;
	float g_float_1;
	float g_float_2;
	float g_float_3;
	
	float2 g_vec2_0;
	float2 g_vec2_1;
	float2 g_vec2_2;
	float2 g_vec2_3;
	
	float4 g_vec4_0;
	float4 g_vec4_1;
	float4 g_vec4_2;
	float4 g_vec4_3;
	
	row_major Matrix g_mat_0;
    row_major Matrix g_mat_1;
	
	int		g_btex_0;
	int		g_btex_1;
	int		g_btex_2;
	int		g_btex_3;
	int		g_btex_4;
	int		g_btex_5;
	int2	padding;
};

cbuffer GLOBAL : register(b2)
{
    float2      Resolution;		// 화면 해상도
	int		    Light2DCount;	// 2D 광원 개수
	int		    Light3DCount;	// 3D 광원 개수
	float	    DT;				// DeltaTime
	float	    Time;			// 누적 시간값
	float       EngineDT;		// DeltaTime
	float	    EngineTime;		// 누적 시간값
	float3      g_CameraPos;    // [추가] 3D 반사광 계산을 위해 필요
    float       padding0;
}


Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);

StructuredBuffer<Light2DInfo> g_Light2D : register(t12);
StructuredBuffer<Light3DInfo> g_Light3D : register(t13);
#define PI 3.1415926535f

#include "func.fx"
#endif
