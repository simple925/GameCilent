#ifndef _FUNC
#define _FUNC

#include "value.fx"

float3 CalcLight2D(int _LightIdx, float3 _PixelPos)
{
	float3 LightColor = float3(0.f, 0.f, 0.f);
	
	// 광원과 물체의 거리에 따른 빛의 세기 비율
	float DistRatio = 1.f;
	
	// DirectionalLight
	if (0 == g_Light2D[_LightIdx].Type)
	{
		LightColor = g_Light2D[_LightIdx].Color + g_Light2D[_LightIdx].Ambient;

	}
	// Point
	else if (1 == g_Light2D[_LightIdx].Type)
	{
		// LightColor
		LightColor = g_Light2D[_LightIdx].Color;
		
		// 광원에서 픽셀을 향하는 방향벡터(월드공간 기준)
		float3 LightToPixel = _PixelPos - g_Light2D[_LightIdx].WorldPos;
		
		// 2차원으로만 거리를 따질 것이기 때문에, z 성분 제거
		LightToPixel.z = 0.f;
		
		// 벡터의 길이 측정(광원과 픽셀 사이의 거리)
		float Dist = length(LightToPixel);
		
		// 반전을 통해서 비율을 적용 받음 밝을 경우는 1에 가까워야 하기때문에
		// saturate() 0 ~ 1 을 벗어나지 않게 해줌
		// 물체와 광원의 거리가 멀어질수록 빛의 세기가 0에 가까워진다.
		// DistRatio = saturate(1.f - (Dist / g_Light2D[_LightIdx].Radius)); // 직선으로 감소하는 그래프
		DistRatio = saturate(cos(saturate(Dist / g_Light2D[_LightIdx].Radius) * (PI / 2.f))); // 곡선으로 감소하는 그래프
	}
	// Spot
	else
	{
		
		// LightColor
		LightColor = g_Light2D[_LightIdx].Color;

		// 광원 → 픽셀 벡터 (월드공간)
		float3 LightToPixel = _PixelPos - g_Light2D[_LightIdx].WorldPos;
		LightToPixel.z = 0.f;

		// 거리 계산
		float Dist = length(LightToPixel);

		// 기본값
		DistRatio = 0.f;

		// 반경 체크 먼저 (빠른 컷)
		if (Dist < g_Light2D[_LightIdx].Radius)
		{
			float3 L = normalize(LightToPixel);
			float3 D = normalize(g_Light2D[_LightIdx].LightDir);

			// cos 값 직접 비교 (acos 제거)
			float cosTheta = dot(L, D);
			float cosCutoff = cos(g_Light2D[_LightIdx].Angle * 0.5f);
			
			if (cosTheta > cosCutoff)
			{
				/* 여기가 진짜 스포트라이트 느낌의 빛
				// 거리 감쇠
				float distRatio = saturate(Dist / g_Light2D[_LightIdx].Radius);
				float distAtt = cos(distRatio * (PI * 0.5f));

				DistRatio = saturate(distAtt);
				*/
				 // ⭐ 거리 감쇄 (부드럽게)
				float distAtt = 1.f - (Dist / g_Light2D[_LightIdx].Radius);
				distAtt = saturate(distAtt);
				distAtt *= distAtt; // 제곱으로 부드럽게

				// ⭐ 각도 감쇄 (핵심)
				float angleAtt = saturate((cosTheta - cosCutoff) / (1.0f - cosCutoff));

				// ⭐ 중심 집중 효과
				angleAtt = pow(angleAtt, 4.f); // 값 키우면 더 손전등 느낌

				DistRatio = distAtt * angleAtt;
			}
		}
	}
	return LightColor * DistRatio;
}
#endif