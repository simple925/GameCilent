#pragma once
#include "CRenderComponent.h"
#include "AFlipbook.h"
class CFlipbookRender :
	public CRenderComponent
{
private:
	vector<Ptr<AFlipbook>>		m_vecFlipbook;
	int							m_CurFlipbook;
	int							m_CurSprite;
	int							m_RepeatCount; // -1 : 반복재생, 1 이상이면 재생 횟수
	bool						m_Finish;
	float						m_FPS;
	float						m_AccTime;
public:
	void SetFlipbook(int _Idx, Ptr<AFlipbook> _Flipbook)
	{
		if (m_vecFlipbook.size() <= _Idx)
			m_vecFlipbook.resize(_Idx + 1);
		m_vecFlipbook[_Idx] = _Flipbook;
	}

	void AddFlipbook(Ptr<AFlipbook> _Flipbook) { m_vecFlipbook.push_back(_Flipbook); }
	void Play(int FlibookIdx, float _FPS, int _RepeatCount)
	{
		m_CurFlipbook = FlibookIdx;
		m_RepeatCount = _RepeatCount;
		m_FPS = _FPS;
	}
	bool CheckFinish();

public:
	virtual void FinalTick() override;
	virtual void Render() override;
	virtual void CreateMaterial() override;
public:
	virtual void SaveToLevelFile(FILE* _File);
	virtual void LoadFromLevelFile(FILE* _File);
	CLONE(CFlipbookRender);
public:
	CFlipbookRender();
	virtual ~CFlipbookRender();
};

