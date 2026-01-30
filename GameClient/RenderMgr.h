#pragma once


#include "CCamera.h"

class RenderMgr
	: public singleton<RenderMgr>
{
	SINGLE(RenderMgr);
private:
	Ptr<CCamera>	m_MainCam;

public:
	void RegisterCamera(Ptr<CCamera> _Cam) { m_MainCam = _Cam; }
	Ptr<CCamera> GetPOVCamera() { return m_MainCam; }

public:
	void Init();
	void Progress();
};

