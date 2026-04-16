#ifndef _DISTORTION
#define _DISTORTION

#include "value.fx"

struct VS_IN
{
	float3 vPos : POSITION;
	float2 vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;
};

VS_OUT VS_Distortion(VS_IN _input)
{
	VS_OUT output = (VS_OUT) 0.f;
        
	float4 vWorld = mul(float4(_input.vPos, 1.f), g_matWorld);
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);
        
	output.vPosition = vProj;
	output.vUV = _input.vUV;
        
	return output;
}

//float4 PS_Distortion(VS_OUT _input) : SV_Target
//{
//    float2 vScreenUV = _input.vPosition.xy / Resolution;
    
//    float4 vColor = PostprocessTarget.Sample(g_sam_0, vScreenUV);
    
//    vColor.rgb = (vColor.r + vColor.g + vColor.b) / 3.f;
    
//    return vColor;
//}


//float4 PS_Distortion(VS_OUT _input) : SV_Target
//{
//    float3 vNoise = g_tex_0.Sample(g_sam_0, _input.vUV + float2(EngineTime * 0.05f, 0.f));
        
//    float2 vScreenUV = vNoise.xy * 0.02f + _input.vPosition.xy / Resolution;
//    float4 vColor = PostprocessTarget.Sample(g_sam_0, vScreenUV);
       
//    return vColor;
//}


static float RingRadius = 0.5f; // 보통 0.5f 사용함(원형 UV 반지름)
static float RingWidth = 0.1f; // 링 두께(예: 0.06f)
static float DistortionScale = 0.02f; // 굴절 세기(예: 0.01f ~ 0.03f)

float4 PS_Distortion(VS_OUT _input) : SV_Target
{
	float2 vScreenUV = _input.vPosition.xy / Resolution;

	float2 vCenterUV = float2(0.5f, 0.5f);
	float2 vDelta = _input.vUV - vCenterUV;
	float fDist = length(vDelta);

    // 원형 영역만 남기고 싶으면 사용하면 됨(원형 메쉬면 없어도 됨)
    // discard 랑 동일 기능, 0 보다 작은 값을 입력으로 넣으면 픽셀 쉐이더가 discard 됨, 입력값이 0 보다 크면 discard 하지 않음
	clip(0.5f - fDist);

	float fHalfWidth = RingWidth * 0.5f;

	float fInner = smoothstep(RingRadius - fHalfWidth, RingRadius, fDist);
	float fOuter = 1.f - smoothstep(RingRadius, RingRadius + fHalfWidth, fDist);
	float fRing = saturate(fInner * fOuter);

	float2 vDir = (fDist > 0.00001f) ? (vDelta / fDist) : float2(0.f, 0.f);
	float2 vOffsetUV = vDir * (fRing * DistortionScale);

	float4 vColor = PostprocessTarget.Sample(g_sam_0, vScreenUV + vOffsetUV);
	return vColor;
}


#endif