#include "pch.h"
#include "TransformUI.h"
#include "GameObject.h"
#include "PhysicsMgr.h"
TransformUI::TransformUI()
	: ComponentUI(COMPONENT_TYPE::TRANSFORM, "TransformUI")
{
}

TransformUI::~TransformUI()
{
}

void TransformUI::Tick_UI()
{
	OutputTitle("Transform");

	Vec3 vPos = GetTarget()->Transform()->GetRelativePos();
	Vec3 vScale = GetTarget()->Transform()->GetRelativeScale();
	Vec3 vRot = GetTarget()->Transform()->GetRelativeRot();

	float fsameLine = 100.f;



	ImGui::Text("POSITION");
	ImGui::SameLine(fsameLine);
	if (ImGui::DragFloat3("##POSITION", vPos))
	{
		GetTarget()->Transform()->SetRelativePos(vPos);
	}

	ImGui::Text("SCALE");
	ImGui::SameLine(fsameLine);
	if (ImGui::DragFloat3("##SCALE", vScale))
		GetTarget()->Transform()->SetRelativeScale(vScale);

	ImGui::Text("ROTATION");
	ImGui::SameLine(fsameLine);
	Vec3 vDegree = vRot * 180 / XM_PI;
	if (ImGui::DragFloat3("##ROTATION", vDegree))
	{
		vRot = vDegree * (XM_PI / 180.f);
		GetTarget()->Transform()->SetRelativeRot(vRot);
	}

	bool Independent = GetTarget()->Transform()->GetIndependentScale();

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Independent Scale");
	ImGui::SameLine();
	if (ImGui::Checkbox("##Independent", &Independent))
	{
		GetTarget()->Transform()->SetIndependentScale(Independent);
	}

	if(nullptr != GetTarget()->Rigidbody2D()) GetTarget()->Rigidbody2D()->SetPositionFromTransform();
}