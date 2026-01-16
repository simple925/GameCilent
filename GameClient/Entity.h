#pragma once
class Entity
{
private:
	static UINT g_NextID;
private:
	const UINT	m_InstID;		// 객체마다 가지는 고유 ID 값
	wstring		m_Name;			// 객체마다 이름을 지정할 수 있다.
public:
	UINT getId() { return m_InstID; }
	void setName(const wstring& _Name) { m_Name = _Name; }
	const wstring& getName() { return m_Name; }

public:
	Entity();
	Entity(const Entity& _Other);

	virtual ~Entity();
};

