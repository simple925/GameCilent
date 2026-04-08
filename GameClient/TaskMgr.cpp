#include "pch.h"
#include "TaskMgr.h"
#include "LevelMgr.h"
#include "GameObject.h"
#include "AssetMgr.h"
#include "PhysicsMgr.h"
TaskMgr::TaskMgr()
{
}
TaskMgr::~TaskMgr()
{
}

void TaskMgr::Progress()
{
	// 쓰레기통 수거
	m_Garbage.clear();

	// Task 처리
	Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetLevel();

	for (UINT i = 0; i < (UINT)m_vecTask.size(); ++i)
	{
		switch (m_vecTask[i].Type)
		{
		case TASK_TYPE::CREATE_OBJECT:
		{
			Ptr<GameObject> pNewObj = (GameObject*)m_vecTask[i].Param_0;

			pCurLevel->AddObject((DWORD_PTR)m_vecTask[i].Param_1, pNewObj);

			pCurLevel->SetChanged(); // 레벨 변경점 저장

			// 레벨이 Play 상태 일때 추가된 오브젝트는, 레벨 시작시점때 Begin 을 호출받지 못했기 때문에
			// 레벨에 스폰될 때 Begin 을 호출받는다.
			if (LEVEL_STATE::PLAY == LevelMgr::GetInst()->GetLevelState())
			{
				pNewObj->Begin();
				pNewObj->FinalTick(); // ← 월드 행렬 갱신 후
				// Rigidbody 위치 재세팅
				if (pNewObj->Rigidbody2D())
					pNewObj->Rigidbody2D()->SetPositionFromTransform();
			}
		}
			break;
		case TASK_TYPE::DESTROY_OBJECT:
		{
			Ptr<GameObject> pObj = (GameObject*)m_vecTask[i].Param_0;
			if (false == pObj->m_Dead) {

				if (pObj->Rigidbody2D())
				{
					PhysicsMgr::GetInst()->UnregisterRigidbody(pObj->Rigidbody2D());
				}

				pObj->m_Dead = true;
				m_Garbage.push_back(pObj);
				pCurLevel->SetChanged(); // 레벨 변경점 저장
			}
		}
			break;
		case TASK_TYPE::CHANGE_LEVEL:
		{
			const wchar_t* pLevelName = (const wchar_t*)m_vecTask[i].Param_0;
			Ptr<ALevel> pLevel = AssetMgr::GetInst()->Find<ALevel>(pLevelName);
			LevelMgr::GetInst()->ChangeLevel(pLevel);
		}
			break;
		case TASK_TYPE::CHANGE_LEVEL_STATE:
		{
			LEVEL_STATE NextState = (LEVEL_STATE)m_vecTask[i].Param_0;
			LevelMgr::GetInst()->ChangeLevelState(NextState);
		}
			break;
		}
	}
	// 테스크를 처리하고 비움
	m_vecTask.clear();
}
