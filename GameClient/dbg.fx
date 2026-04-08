#ifndef _DEBUG_RENDER
#define _DEBUG_RENDER

#include "value.fx"

#define COLOR g_vec4_0

// layout ����
struct VS_IN
{
	float3 vPos : POSITION; // Semantic Layout���� ������ �̸�
	float2 vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition : SV_Position; // Rasterizer �� ������, NDC ��ǥ
	float2 vUV : TEXCOORD;
};


VS_OUT VS_Debug(VS_IN _input)
{
	/*
	// z ȸ��
	float3x3 rotMat =
	{
		cos(g_ObjectRot.z), sin(g_ObjectRot.z), 0.f,
		-sin(g_ObjectRot.z), cos(g_ObjectRot.z), 0.f,
		0.f, 0.f, 1.f,
	};
	// y ȸ��
	float3x3 rotMat =
	{
		cos(g_ObjectRot.z), 0.f, sin(g_ObjectRot.z),
		0.f, 1.f, 0.f,
		-sin(g_ObjectRot.z), 0.f, cos(g_ObjectRot.z),
	};
	// x ȸ��
	float3x3 rotMat =
	{
		1.f, 0.f, 0.f,
		0.f, cos(g_ObjectRot.z), sin(g_ObjectRot.z),
		0.f, -sin(g_ObjectRot.z), cos(g_ObjectRot.z),
	};
	*/

	/* 
		(x, y, z, ������ǥ)  X   ( scale.x		0		 0		 0	 )
								(	0		  scale.y	 0		 0	 )
								(	0			0	   scale.z	 0	 )
								(  pos.x	   pos.y    pos.z	 0	 )
	*/
	VS_OUT output = (VS_OUT) 0.f;
	
	// Local -> World
	float4 vWorld = mul(float4(_input.vPos, 1.f /*������ǥ*/), g_matWorld);
	// World -> View
	float4 vView = mul(vWorld, g_matView);
	// View -> Proj
	float4 vProj = mul(vView, g_matProj);
	
	// Viewz ������ ������.
	// ������� Ư����, ��Ȯ�� NDC ��ǥ�� ��� ���ؼ�
	// ��������� ���� View ��ǥ�� z ������ ������ �۾��� ��ľȿ� �־���ϴµ� �̰��� �Ұ����ϱ� ������
	// ���� ����� w(4��) �ڸ��� View ��ǥ�� z�� ��� �ǵ��� �Ѵ�.
	
	// ��������� ���ϰ� ���� ������� x,y,z �� w �� ������ ���� NDC ��ǥ�� ���� �� �ִ�.
	// (VX, VY, VZ, 1.f) * ProjMat == (PX*VZ, PY*VZ, PZ*VZ, VZ:������) �����Ͷ������� ����
	// vProj.xyz /= vProj.w; �� �۾��� �ؾ� NDC ��ǥ��� ���� �Ű���
	
	
	// ������ǥ 0���� �����ϸ� �̵� ������ ������
	// x,y,z, �� w �� ������ �۾��� Rasterizer ���� �����ϱ� ������,
	// �������� ������ �ڵ带 �ۼ��� �ʿ�� ����.
	output.vPosition = vProj; // ��������� ���⺤�Ϳ� ���Ҷ� �̵������� �����ؾߵǱ� ������ ������ǥ�� 0.f ������
	output.vUV = _input.vUV;
    
	return output;
}


float4 PS_Debug(VS_OUT _input) : SV_Target
{
	

	return COLOR;
}

#endif
