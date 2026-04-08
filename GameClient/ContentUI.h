#pragma once
#include "EditorUI.h"
#include "TreeUI.h"
class ContentUI :
	public EditorUI
{
private:
	Ptr<TreeUI> m_Tree;
public:
	virtual void Tick_UI();
	void Renew();
	void SelectAsset(DWORD_PTR _Asset);
public:
	ContentUI();
	virtual ~ContentUI();
};

