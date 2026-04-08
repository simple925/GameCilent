#include "pch.h"
#include "CCollider2D.h"

#include "RenderMgr.h"

#include "PhysicsUtil.h"
#include "PhysicsMgr.h"
CCollider2D::CCollider2D()
	: Component(COMPONENT_TYPE::COLLIDER2D)
	, m_Scale(Vec2(1.f, 1.f))
	, m_OverlapCount(0)
	, m_ShapeDirty(false)
	, m_Active(true)
	, m_OneWay(false)
{
}

CCollider2D::CCollider2D(const CCollider2D& _Origin)
	: Component(_Origin)
	, m_Offset(_Origin.m_Offset)
	, m_Scale(_Origin.m_Scale)
	, m_OverlapCount(0)
	, m_ShapeDirty(false)
	, m_Active(_Origin.m_Active)
	, m_OneWay(_Origin.m_OneWay)
{
	// 비긴 되면 델리게이트 함수들이 등록 됨 그래서 복사 할 필요 없음
	m_Body = b2_nullBodyId;
	m_Shape = b2_nullShapeId;
}

CCollider2D::~CCollider2D()
{
	if (!B2_IS_NULL(m_Shape) && !B2_IS_NULL(m_Body)) DestroyShape();
}

void CCollider2D::Begin()
{
	CreateShape();
}

void CCollider2D::FinalTick()
{
	// XMMatrixTranslation 이동행렬
	Matrix matTran = XMMatrixTranslation(m_Offset.x, m_Offset.y, 0.f);
	// XMMatrixScaling 크기행렬
	Matrix matScale = XMMatrixScaling(m_Scale.x, m_Scale.y, 0.f);

	m_matWorld = matScale * matTran;
	m_matWorld *= Transform()->GetWorldMat();

	if (0 < m_OverlapCount)
		DrawDebugRect(m_matWorld, Vec4(1.f, 0.f, 0.f, 1.f), 0.f);
	else if (0 == m_OverlapCount)
		DrawDebugRect(m_matWorld, Vec4(0.f, 1.f, 0.f, 1.f), 0.f);
	else
		//assert(nullptr);

	if (m_ShapeDirty)
	{
		DestroyShape();
		CreateShape();
		m_ShapeDirty = false;
	}

	if (m_ShapeDirty && !GetOwner()->IsDead())
	{
		PhysicsMgr::GetInst()->RequestShapeRecreate(this); // Step 이후로 위임
		m_ShapeDirty = false;
	}
}

void CCollider2D::BeginOverlap(Ptr<CCollider2D> _Other)
{
	for (auto& del : m_vecBeginDel)
	{
		(del.Inst->*del.MemFunc)(this, _Other.Get());
	}
	//for (size_t i = 0; i < m_vecBeginDel.size(); ++i) { (m_vecBeginDel[i].Inst->*m_vecBeginDel[i].MemFunc)(this, _Other.Get()); }
	++m_OverlapCount;
}

void CCollider2D::Overlap(Ptr<CCollider2D> _Other)
{ // 프레임단위 연산이 필요
	for (auto& del : m_vecOverDel)
	{
		(del.Inst->*del.MemFunc)(this, _Other.Get());
	}
	//for (size_t i = 0; i < m_vecOverDel.size(); ++i) { (m_vecOverDel[i].Inst->*m_vecOverDel[i].MemFunc)(this, _Other.Get()); }
}

void CCollider2D::EndOverlap(Ptr<CCollider2D> _Other)
{
	for (auto& del : m_vecEndDel)
	{
		(del.Inst->*del.MemFunc)(this, _Other.Get());
	}
	//for (size_t i = 0; i < m_vecEndDel.size(); ++i) { (m_vecEndDel[i].Inst->*m_vecEndDel[i].MemFunc)(this, _Other.Get()); }
	--m_OverlapCount;
}

void CCollider2D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Offset,sizeof(Vec2), 1, _File);
	fwrite(&m_Scale,sizeof(Vec2), 1, _File);
	fwrite(&m_Active, sizeof(bool), 1, _File);
}

void CCollider2D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Offset, sizeof(Vec2), 1, _File);
	fread(&m_Scale, sizeof(Vec2), 1, _File);
	fread(&m_Active, sizeof(bool), 1, _File);
}

void CCollider2D::CreateShape()
{
	if (!m_Active) return;

	Ptr<CRigidbody2D> rb = Rigidbody2D();
	if (!rb || B2_IS_NULL(rb->GetBody()))
		return;

	m_Body = rb->GetBody();
	
	Vec3 trScale = Transform()->GetWorldScale();

	float hx = Phys::ToPhys((m_Scale.x * trScale.x) * 0.5f);
	float hy = Phys::ToPhys((m_Scale.y * trScale.y) * 0.5f);

	// 🔥 Transform의 반전 정보를 Offset에 반영
	Vec3 worldPos = Transform()->GetWorldPos();
	float signX = (trScale.x < 0.f) ? -1.f : 1.f;
	float signY = (trScale.y < 0.f) ? -1.f : 1.f;

	b2Vec2 offset =
	{
		Phys::ToPhys(m_Offset.x * trScale.x * signX),
		Phys::ToPhys(m_Offset.y * trScale.y * signY)
	};

	hx = abs(hx);
	hy = abs(hy);

	b2Polygon box = b2MakeOffsetBox(hx, hy, offset, b2MakeRot(0.0f));

	b2ShapeDef def = b2DefaultShapeDef();

	def.userData = this;
	def.material.friction = 0.2f;
	def.material.restitution = 0.f;
	def.density = 1.f;
	def.enableContactEvents = true;
	def.enableHitEvents = true;
	def.enablePreSolveEvents = m_OneWay;
	def.isSensor = m_Trigger;	// Trigger면 센서로 설정(물리 반응 없이 겹침만 감지)
	m_Shape = b2CreatePolygonShape(m_Body, &def, &box);
}

void CCollider2D::DestroyShape()
{
	if (B2_IS_NULL(m_Shape))
		return;

	// Body 가 유효한지 확인
		if (B2_IS_NULL(m_Body))
		{
			m_Shape = b2_nullShapeId;
			return;
		}

	// World 가 유효한지 확인
	if (B2_IS_NULL(PhysicsMgr::GetInst()->GetWorld()))
	{
		m_Shape = b2_nullShapeId;
		return;
	}

	b2DestroyShape(m_Shape, false);
	m_Shape = b2_nullShapeId;
}