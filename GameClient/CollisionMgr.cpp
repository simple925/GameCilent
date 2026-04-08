#include "pch.h"
#include "CollisionMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"
#include "GameObject.h"
CollisionMgr::CollisionMgr()
{
}

CollisionMgr::~CollisionMgr()
{
}

void CollisionMgr::Progress(Ptr<ALevel> _Level)
{
	UINT* pMatrix = _Level->GetCollisionMatrix();

	for (UINT Row = 0; Row < MAX_LAYER; ++Row)
	{
		for (UINT Col = Row; Col < MAX_LAYER; ++Col)
		{
			// 비트 들어있는지 체크 &
			if (false == (pMatrix[Row] & (1 << Col))) continue;

			CollisionBtwLayer(_Level->GetLayer(Row), _Level->GetLayer(Col));
		}

	}
}

void CollisionMgr::CollisionBtwLayer(Layer* _Left, Layer* _Right)
{
	// & 가 붙어야 객체 복사가 안일어나고 원본 그대로 가져옴!!!!!
	// & 가 없으면 없기 때문에 지역변수로 취급함
	const vector<Ptr<GameObject>>& vecLeft = _Left->GetAllObjects();
	const vector<Ptr<GameObject>>& vecRight = _Right->GetAllObjects();

	for (UINT i = 0; i < (UINT)vecLeft.size(); ++i)
	{
		if (nullptr == vecLeft[i]->Collider2D()) continue;

		for (UINT j = 0; j < (UINT)vecRight.size(); ++j)
		{
			if (nullptr == vecRight[j]->Collider2D()) continue;

			// 두 충돌체의 고유 ID로 조합을 한 키값 생성
			COL_ID colid;
			colid.LeftID = vecLeft[i]->Collider2D()->GetId();
			colid.RightID = vecRight[j]->Collider2D()->GetId();
			
			map<ULONGLONG, bool>::iterator iter = m_mapColID.find(colid.ID);

			if (iter == m_mapColID.end())
			{
				m_mapColID.insert(make_pair(colid.ID, false));
				iter = m_mapColID.find(colid.ID);
			}
			
			// 충돌 검사를 진행하는 두 오브젝트중에서 하나라도 Dead 상태가 존재하는지
			bool IsDead = vecLeft[i]->IsDead() || vecRight[j]->IsDead();

			// 지금 충돌
			if (IsCollision(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D()))
			{
				// 둘중 하나가 곧 삭제 예정
				if (IsDead)
				{
					// 클로드가 추천한 개선 코드
					//if (iter->second)
					//{
						vecLeft[i]->Collider2D()->EndOverlap(vecRight[j]->Collider2D());
						vecRight[j]->Collider2D()->EndOverlap(vecLeft[i]->Collider2D());
					//}
					//m_mapColID.erase(iter);
					//continue;
				}
				// 이전 충돌 함
				else if (iter->second)
				{
					vecLeft[i]->Collider2D()->Overlap(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->Overlap(vecLeft[i]->Collider2D());
				}
				// 이전에는 충돌하지 않음
				else
				{
					vecLeft[i]->Collider2D()->BeginOverlap(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->BeginOverlap(vecLeft[i]->Collider2D());
				}
				iter->second = true;
			}
			// 현제 충돌 중이 아님
			else
			{
				// 이전 프레임에는 충돌 중
				if (iter->second)
				{
					vecLeft[i]->Collider2D()->EndOverlap(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->EndOverlap(vecLeft[i]->Collider2D());
				}
				iter->second = false;
			}
		}
	}
}

bool CollisionMgr::IsCollision(Ptr<CCollider2D> _LeftCol, Ptr<CCollider2D> _RightCol)
{
	Ptr<AMesh> pRectMesh = FIND(AMesh,L"q");
	const vector<Vtx> pVtx = pRectMesh->GetVtxSysMem();
	const Matrix& matWorldLeft = _LeftCol->GetWorldMatrix();
	const Matrix& matWorldRight = _RightCol->GetWorldMatrix();

	// 월드 공간상에서 충돌을 검사하기 위해서, RectMesh 모델을 각 충돌체의 월드행렬을 곱해서 정점을 충돌체 꼭지점에 배치시키낟.
	// 각 꼭지점끼리 빼서 두 충돌체의 표면 방향벡터를 각 충돌체로부터 2개씩 구한다.

	Vec3 Axis[4] = {};
	Axis[0] = XMVector3TransformCoord(pVtx[1].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
	Axis[1] = XMVector3TransformCoord(pVtx[3].vPos, matWorldLeft) - XMVector3TransformCoord(pVtx[0].vPos, matWorldLeft);
	Axis[2] = XMVector3TransformCoord(pVtx[1].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);
	Axis[3] = XMVector3TransformCoord(pVtx[3].vPos, matWorldRight) - XMVector3TransformCoord(pVtx[0].vPos, matWorldRight);

	// 중심 벡터를 구함
	Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldRight) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), matWorldLeft);
	for (int i = 0; i < 4; ++i)
	{
		// 4 개의 축 중에서, 하나를 투영 목적지로 정함
		// 원본값을 훼손하면 나중에 투영할때 문제가 생기기 때문에, 정규화한 벡터를 따로 지역변수로 둠
		Vec3 vProjAxis = Axis[i];
		vProjAxis.Normalize(); // 투영축 정규화

		// 투영축으로 4개의 벡터를 투영시켜서 얻은 면적의 절반 길이를 구함
		float Dot = 0.f;
		for (int j = 0; j < 4; ++j)
		{
			// 내적하는 경우 음수로 나올 수 있음 그래서 절대값을 취해줌 왜냐 코사인세타는 90가 넘어가면 음수가 됨
			Dot += fabs(vProjAxis.Dot(Axis[j])); // 내적 한 후 값을 다 더함 
		}
		Dot /= 2.f;

		// 두 충돌체의 중심끼리 이은 벡터도 투영시킴
		float fCenter = fabs(vCenter.Dot(vProjAxis)); 
		// 중심끼리 이은 벡터의 면적이 더크다면, 두 충돌체를 나눌 수 있는 분리축이 존재함
		if (fCenter > Dot) return false;
	}
	return true;
}
/*
Ptr<GameObject> CollisionMgr::Pick(Vec3 worldPos)
{
	Ptr<ALevel> level = LevelMgr::GetInst()->GetLevel();
	Ptr<GameObject> pickedObj = nullptr;
	float maxDepth = -FLT_MAX;

	Vec3 camPos = RenderMgr::GetInst()->GetPOVCamera()->Transform()->GetWorldPos();
	Vec3 camForward = RenderMgr::GetInst()->GetPOVCamera()->Transform()->GetDir(DIR::FRONT);

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		Layer* layer = level->GetLayer(i);

		for (auto obj : layer->GetAllObjects())
		{
			if (obj->IsDead())
				continue;

			CCollider2D* col = obj->Collider2D().Get();
			if (!col)
				continue;

			Vec3 pickPos = worldPos;
			pickPos.z = obj->Transform()->GetWorldPos().z;

			if (!col->Contains(pickPos))
				continue;

			Vec3 objPos = obj->Transform()->GetWorldPos();
			float depth = Dot(objPos - camPos, camForward);

			if (depth > maxDepth)
			{
				maxDepth = depth;
				pickedObj = obj;
			}
		}
	}

	return pickedObj;
}

Ptr<GameObject> CollisionMgr::PickRay3D(Vec2 mouse)
{
	Vec3 rayOrigin, rayDir;
	MakeRay(mouse, rayOrigin, rayDir);
	Ptr<ALevel> level = LevelMgr::GetInst()->GetLevel();
	Ptr<GameObject> picked = nullptr;
	float closestT = FLT_MAX;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		Layer* layer = level->GetLayer(i);

		for (auto obj : layer->GetAllObjects())
		{
			CCollider2D* col = obj->Collider2D().Get();
			if (!col) continue;

			Vec3 pos = obj->Transform()->GetWorldPos();
			Vec2 size = col->GetScale();

			Vec3 min = pos - Vec3(size.x * 0.5f, size.y * 0.5f, 0.1f);
			Vec3 max = pos + Vec3(size.x * 0.5f, size.y * 0.5f, 0.1f);

			float t;
			if (RayAABB(rayOrigin, rayDir, min, max, t))
			{
				if (t < closestT)
				{
					closestT = t;
					picked = obj;
				}
			}
		}
	}

	return picked;
}
*/