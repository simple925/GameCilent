#pragma once
#include "EditorUI.h"
#include "ComponentUI.h"
#include "ScriptUI.h"
#include "AssetUI.h"
class Inspector
	: public EditorUI
{
private:
	Ptr<GameObject>		m_TargetObject;
	Ptr<ComponentUI>	m_arrComUI[(UINT)COMPONENT_TYPE::END];
	vector<Ptr<ScriptUI>>   m_vecScriptUI;

	Ptr<Asset>			m_TargetAsset;
	Ptr<AssetUI>		m_arrAssetUI[(UINT)ASSET_TYPE::END];

public:
	void SetTargetObject(Ptr<GameObject> _Object);
	void SetTargetAsset(Ptr<Asset> _Asset);
public:
	virtual void Tick_UI()override;
	void CreateChildUI();
public:
	Inspector();
	virtual ~Inspector();
};

