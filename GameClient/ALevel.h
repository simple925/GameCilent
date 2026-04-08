#pragma once
#include "Asset.h"
#include "Layer.h"

class ALevel :
	public Asset
{
private:
	Layer       m_arrLayer[MAX_LAYER];  // 하나의 레벨안에 총 32개의 레이어가 존재
	UINT        m_Matrix[MAX_LAYER];    // 레이어끼리 충돌검사를 진행할 마킹 데이터
	bool        m_Changed;				// 레벨 안에 오브젝트들의 상태가 변경(오브젝트 추가, 삭제, 계층 변경) 됐는지 확인하는 변수
public:
	void AddObject(int _LayerIdx, Ptr<GameObject> _Object);
	Layer* GetLayer(int _Idx) { assert(0 <= _Idx && _Idx < MAX_LAYER); return &m_arrLayer[_Idx]; }
	void Deregister();
	void CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2);
	void CheckCollisionLayer(const wstring& _LayerName1, UINT _LayerName2);
	UINT* GetCollisionMatrix() { return m_Matrix; }
	Ptr<GameObject> FindObjectByName(const wstring& _name);

	bool IsChanged()
	{
		bool Changed = m_Changed;
		m_Changed = false; // 변경점을 알려주고 false 초기화함
		return Changed;
	}

	void SetChanged() { m_Changed = true; }

	virtual int Save(const wstring& _FilePath) override;
	virtual int Load(const wstring& _FilePath) override;
public:
	void Begin();
	void Tick();
	void FinalTick();

	CLONE(ALevel);
public:
	ALevel();
	virtual ~ALevel();
};

