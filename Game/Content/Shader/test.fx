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
	
	Matrix mat_1;
	Matrix mat_2;
};
cbuffer TRANSFORM : register(b0)
{
	//float4 g_ObjectPos;
	//float4 g_ObjectScale;
	//float4 g_ObjectRot;
	
	// row_major 행열 읽는 방식이 cpu gpu 달라서 맞춰줌
	// matrix == float4x4 같은 자료형
	row_major matrix g_matWorld;		// Local(Model) -> World Space
	row_major matrix g_matView;			// World -> Camera(View) Space
	row_major matrix g_matProj;			// Camera(View) Space ->
}

VS_OUT VS_Test(VS_IN _input)
{
		/*
	// z 회전
	float3x3 rotMat =
	{
		cos(g_ObjectRot.z), sin(g_ObjectRot.z), 0.f,
		-sin(g_ObjectRot.z), cos(g_ObjectRot.z), 0.f,
		0.f, 0.f, 1.f,
	};
	// y 회전
	float3x3 rotMat =
	{
		cos(g_ObjectRot.z), 0.f, sin(g_ObjectRot.z),
		0.f, 1.f, 0.f,
		-sin(g_ObjectRot.z), 0.f, cos(g_ObjectRot.z),
	};
	// x 회전
	float3x3 rotMat =
	{
		1.f, 0.f, 0.f,
		0.f, cos(g_ObjectRot.z), sin(g_ObjectRot.z),
		0.f, -sin(g_ObjectRot.z), cos(g_ObjectRot.z),
	};
	*/

	/* 
		(x, y, z, 동차좌표)  X   ( scale.x		0		 0		 0	 )
								(	0		  scale.y	 0		 0	 )
								(	0			0	   scale.z	 0	 )
								(  pos.x	   pos.y    pos.z	 0	 )
	*/
	VS_OUT output = (VS_OUT) 0.f;
	
	// Local -> World
	float4 vWorld = mul(float4(_input.vPos, 1.f /*동차좌표*/), g_matWorld);
	// World -> View
	float4 vView = mul(vWorld, g_matView);
	// View -> Proj
	float4 vProj = mul(vView, g_matProj);
	
	// Viewz 값으로 나눈다.
	// 투영행렬 특성상, 정확한 NDC 좌표를 얻기 위해서
	// 투영행렬을 곱할 View 좌표의 z 값으로 나누는 작업이 행렬안에 있어야하는데 이것이 불가능하기 때문에
	// 연산 결과의 w(4열) 자리에 View 좌표의 z가 출력 되도록 한다.
	
	// 투영행렬을 곱하고 얻은 결과값의 x,y,z 를 w 로 나눠야 최종 NDC 좌표를 얻을 수 있다.
	// (VX, VY, VZ, 1.f) * ProjMat == (PX*VZ, PY*VZ, PZ*VZ, VZ:나눌값) 레스터라이져가 해줌
	// vProj.xyz /= vProj.w; 이 작업을 해야 NDC 좌표계로 값이 옮겨짐
	
	
	// 동차좌표 0으로 설정하면 이동 정보를 무시함
	// x,y,z, 를 w 로 나누는 작업을 Rasterizer 에서 진행하기 때문에,
	// 수동으로 나누는 코드를 작성할 필요는 없다.
	output.vPosition = vProj; // 월드행렬을 방향벡터에 곱할땐 이동정보를 무시해야되기 때문에 동차좌표가 0.f 들어가야함
	output.vUV = _input.vUV;
	output.vColor = _input.vColor;
    
	return output;
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
float4 PS_Test(VS_OUT _input) : SV_Target
{
	
	float4 vColor = g_tex_0.Sample(g_sam_1, _input.vUV);
	if (vColor.a == 0.f ||vColor.r > 0.70f && vColor.b > 0.70f && vColor.g < 0.3f) // 마젠타 색인 경우
	{
		discard; // 픽셀쉐이더는 키워드를 만나면 픽셀쉐이더가 종료됨
	}
	if (g_int_0 == 1)
	{
		vColor.r *= 2.f;
	}
	
	// 알파블렌딩의 핵심 기본 배경을 출력함!!!!!! 블렌드 쉐이프
	// 깊이 텍스쳐에 흔적이 남는다
	
	// texture 추출 도구 필요
	// 입력 UV는 정점에서 반환한 값을 보간 받아서 픽셀쉐이더에 입력됨
		return vColor;
}
#endif