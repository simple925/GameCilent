#pragma once

struct Vertex
{
	Vec3	vPos;	// 3D 공간상에서 정점의 위치
	Vec2	vUV;	// 정점위에 이미지를 띄울 경우, 이미지의 어느지점을 참고하는지 좌표 Texture Coordinate UV
	Vec4	vColor;	// 정점 자체의 색상
}; typedef Vertex Vtx;

struct TransformMatrix
{
	Matrix matWorld;		// 물체가 채움
	Matrix matView;			// 뷰행렬
	Matrix matProj;			// NDC 좌표계로 투영(압축)시킴
};
// extern, 전방선언, 유일한 전역변수, 모든파일(cpp) 에서 접근 가능
extern TransformMatrix g_Trans;


struct MtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	mat[2];
};