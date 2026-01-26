#pragma once
class Entity
{
private:
	static UINT g_NextID;
private:
	const UINT	m_InstID;		// 객체마다 가지는 고유 ID 값
	wstring		m_Name;			// 객체마다 이름을 지정할 수 있다.
	int			m_RefCount;		// 참조 카운팅
private:
	void AddRef() { ++m_RefCount; }
	void Release() { --m_RefCount; if (m_RefCount <= 0) delete this; }
public:
	UINT GetId() { return m_InstID; }
	void SetName(const wstring& _Name) { m_Name = _Name; }
	const wstring& GetName() { return m_Name; }

public:
	Entity();
	Entity(const Entity& _Other);

	virtual ~Entity();

	/*
		어떤 타입이던 상관없이 선언
		Ptr<T> 모든 타입 friend 선언
	*/
	template<typename T>
	friend class Ptr;
};

