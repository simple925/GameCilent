#pragma once
#include "Component.h"
enum class PROJ_TYPE
{
	ORTHOGRAPHIC, // 직교투영
	PERSPECTIVE,  // 원근투영
};
class CCamera :
	public Component
{

private:
	UINT        m_LayerCheck;   // 어떤 레이어만 화면에 렌더링 할 것인지 비트체크
	PROJ_TYPE   m_ProjType;     // 투영 방식
	float       m_Far;          // 카메라 시야 최대거리
	float		m_Near;			// 케마라 바로 앞 절단면(이 값보다 가까우면 나오지 않음)
	float       m_Width;        // 투영 가로길이
	float       m_AspectRatio;  // 종횡비(가로 / 세로), 세로대비 가로의 길이 비율
	float       m_FOV;          // 원근투영 시야각
	float       m_OrthoScale;   // 직교투영 배율
	Matrix      m_matView;      // View 행렬;
	Matrix      m_matProj;      // Proj 행렬;

	float		m_fOrbitDist;	// 카메라와 공전 중심점 사이의 거리

	vector<GameObject*>		m_vecOpaque;
	vector<GameObject*>		m_vecMasked;
	vector<GameObject*>		m_vecParticle;
	vector<GameObject*>		m_vecTransparent;
	vector<GameObject*>		m_vecPostProcess;

public:

	GET_SET(PROJ_TYPE, ProjType);
	GET_SET(float, Far);
	GET_SET(float, Near);
	GET_SET(float, Width);
	GET_SET(float, AspectRatio);
	GET_SET(float, OrthoScale);
	GET_SET(float, fOrbitDist);
	Matrix GetViewMat() { return m_matView; }

	float GetFOV() { return m_FOV * (180 / XM_PI); }
	// _Degree 60분법
	// 라디안으로 보정
	void SetFOV(float _Degree) { m_FOV = _Degree* (XM_PI / 180.f); }

public:
	virtual void Begin() override;
	virtual void FinalTick() override;
public:
	void LayerCheckAll() { m_LayerCheck = 0xffffffff; } // 32비트 전부가 1로 꽉 채워진 상태
	void LayerCheckClear() { m_LayerCheck = 0; }
	void LayerCheck(int _Idx);
	// 특정 레이어가 켜져 있는지 확인하는 함수
	bool IsLayerChecked(int _Idx) { return m_LayerCheck & (1 << _Idx); }
	// UI에서 직접 접근하기 위해 현재 비트값 전체를 가져오는 함수
	UINT GetLayerCheck() { return m_LayerCheck; }

public:
	void SortObject();
	void Render();

	virtual void SaveToLevelFile(FILE* _File);
	virtual void LoadFromLevelFile(FILE* _File);

	CLONE(CCamera);
public:
	CCamera();
	virtual ~CCamera();
};

