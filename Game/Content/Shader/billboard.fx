#ifndef _BILLBOARD
#define _BILLBOARD
#include "value.fx"

#define BILLBOARD_SCALE g_vec2_0

// layout 정보
struct VS_IN
{
	// Semantic 은 꼭 맞을 필요 없음 필요한 것만 가져올 수 있음
	// Semantic 이름에는 0이 생략 되어 있음 POSITION
	float3 vPos : POSITION; // Semantic Layout에서 설명한 이름
	float2 vUV : TEXCOORD;
	float4 vColor : COLOR;
};

struct VS_OUT
{
	// SV_xxx 는 약속된 값
	float4 vPosition : SV_Position; // Rasterizer 로 보낼때, NDC 좌표
	float2 vUV : TEXCOORD;
	float4 vColor : COLOR;
};



VS_OUT VS_Billboard(VS_IN _input)
{
	
	VS_OUT output = (VS_OUT) 0.f;
	
	float4 vWorld = mul(float4(0.f, 0.f, 0.f, 1.f /*동차좌표*/), g_matWorld);
	float4 vView = mul(vWorld, g_matView);
	vView.xy += _input.vPos.xy * BILLBOARD_SCALE;
	
	float4 vProj = mul(vView, g_matProj);
	output.vPosition = vProj; // 월드행렬을 방향벡터에 곱할땐 이동정보를 무시해야되기 때문에 동차좌표가 0.f 들어가야함
	output.vUV = _input.vUV;
	output.vColor = _input.vColor;
    
	return output;
}

// 정점에서 반환한 값이 보간(Interpolation) 되어 픽셀이 쉐이더의 입력으로 돌아온다.
float4 PS_Billboard(VS_OUT _input) : SV_Target
{
	//return float4(1.f, 0.f, 0.f, 1.f);
	return g_tex_0.Sample(g_sam_1, _input.vUV);
}

#endif