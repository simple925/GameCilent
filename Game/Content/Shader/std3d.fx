#ifndef _TEST // n = not ifndef [test] test 만약 test 정의 되어있지 않다면
#define _TEST
#include "value.fx"

#define TintColor g_vec4_0

// layout 정보
struct VS_IN
{
	// Semantic 은 꼭 맞을 필요 없음 필요한 것만 가져올 수 있음
	// Semantic 이름에는 0이 생략 되어 있음 POSITION
	float3 vPos : POSITION; // Semantic Layout에서 설명한 이름
	float2 vUV : TEXCOORD;
	float4 vColor : COLOR;
    float3 vNormal : NORMAL; // [필수] 3D를 위한 법선 벡터
};

struct VS_OUT
{
	// SV_xxx 는 약속된 값
	float4 vPosition : SV_Position; // Rasterizer 로 보낼때, NDC 좌표
	float2 vUV : TEXCOORD;
	float4 vColor : COLOR;
	float3 vWorldPos : POSITION;
    float3 vNormal : NORMAL; // [필수] 픽셀 쉐이더로 전달
};


VS_OUT VS_Std3D(VS_IN _input)
{
	
    VS_OUT output = (VS_OUT) 0.f;

    // 위치 계산
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vWorldPos = vWorldPos.xyz;
    output.vPosition = mul(mul(vWorldPos, g_matView), g_matProj);

    // 법선 계산: 물체가 회전하면 법선도 같이 회전해야 함 (w=0으로 하여 이동 제외)
    output.vNormal = normalize(mul(float4(_input.vNormal, 0.f), g_matWorld).xyz);

    output.vUV = _input.vUV;
    output.vColor = _input.vColor;
    return output;
}


float4 PS_Std3D(VS_OUT _input) : SV_Target
{
	/*
	float4 vColor = float4(1.f, 0.f, 1.f, 1.f);
	if(g_btex_0)
	{
		vColor = g_tex_0.Sample(g_sam_1, _input.vUV);
	}

	vColor *= TintColor;

	float3 LightColor = float3(0.f, 0.f, 0.f);
	
	for (int i = 0; i < Light2DCount; ++i)
	{
		LightColor += CalcLight2D(i, _input.vWorldPos);
	}
	
	vColor.rgb *= LightColor;
	
	return vColor;
	*/

    float4 vOutColor = g_tex_0.Sample(g_sam_0, _input.vUV);
    
    float3 finalLight = float3(0.f, 0.f, 0.f);
    float3 viewDir = normalize(g_CameraPos - _input.vWorldPos);
    float3 normal = normalize(_input.vNormal);

    for (int i = 0; i < Light3DCount; ++i)
    {
        // 간단한 Directional Light 예시 (Lambert 모델)
        float3 lightDir = normalize(-g_Light3D[i].LightDir);
        
        // 1. Diffuse: 법선과 빛의 각도에 따른 밝기
        float diffusePow = max(dot(normal, lightDir), 0.f);
        
        // 2. Specular: 카메라 위치에 따른 반짝임
        float3 reflectDir = reflect(-lightDir, normal);
        float specPow = pow(max(dot(viewDir, reflectDir), 0.f), 32.f);
        
        finalLight += (g_Light3D[i].Color.rgb * diffusePow);
        finalLight += (g_Light3D[i].Specular.rgb * specPow);
        finalLight += g_Light3D[i].Ambient.rgb;
    }

    vOutColor.rgb *= finalLight;
    return vOutColor;
}
#endif
