#ifndef _SETCOLOR
#define _SETCOLOR
#include "value.fx"

// unordered access register
// 양방향 레지스터
// 랜더링은 불가
// 읽고 쓰기 둘다 가능
RWTexture2D<float4> g_Target : register(u0);

// 쓰레드 개수 지정, 스레드 그룹, HLSL 5.0 기준 최대 1024개
// 문법 별로 지정 갯수가 다름
//[numthreads(8,8,8)]
//[numthreads(1024,1,1)]
[numthreads(32, 32, 1)]
void CS_SetColor(int3 _ID : SV_DispatchThreadID)
{
	//g_Target[_ID.xy] = float4(1.f, 0.f, 0.f, 1.f);
	g_Target[_ID.xy] = g_vec4_0;
}
#endif