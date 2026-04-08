#ifndef _STRUCT
#define _STRUCT

// Light2D 정보
struct Light2DInfo
{
    int     Type;
    float3  Color; // 빛의 색상
    float3  Ambient; // 환경광, 광원이 존재하면서 최소한으로 발생하는 빛의 세기
    float3  LightDir; // 광원의 빛이 향하는 방향
    float3  WorldPos; // 광원의 위치 (포인트, 스포트)
    float   Radius; // 빛의 영향 반경(포인트, 스포트)
    float   Angle; // SpotLight 범위 각
};

struct Light3DInfo
{
    float4 Color; // 빛의 색상
    float4 Ambient; // 환경광
    float4 Specular; // 반사광 (반짝임)
    float3 LightPos; // 광원 위치
    float Radius; // 영향 범위
    float3 LightDir; // 광원 방향
    int Type; // 0: Directional, 1: Point, 2: Spot
};

#endif