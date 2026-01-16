#pragma once
class PathMgr
	:public singleton<PathMgr>
{
	SINGLE(PathMgr)
private:
	wchar_t		m_ContentPath[255];
private:
public:
	void Init();
	const wchar_t* GetContentPath() { return m_ContentPath; }
	const wstring GetContentPath(const wstring& _relativeAddr) { return m_ContentPath + _relativeAddr; }
};

#define CONTENT_PATH PathMgr::GetInst()->GetContentPath()