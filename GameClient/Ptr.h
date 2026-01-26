#pragma once
// & 주소 내놔
// * 주소로 접근한다
template<typename T>
class Ptr
{
private:
	T* m_Ptr;

public:
	T* Get() const { return m_Ptr; }
	T** GetAdressOf() { return &m_Ptr; }

public:
	Ptr& operator =(T* _Ptr)
	{
		if (nullptr != m_Ptr)
			m_Ptr->Release();

		m_Ptr = _Ptr;

		if (nullptr != m_Ptr)
			m_Ptr->AddRef();

		return *this;
	}
	Ptr& operator =(const Ptr<T>& _Ptr) // 스마트 포인터 자체로 대입 하는 경우
	{
		if (nullptr != m_Ptr)
			m_Ptr->Release();

		m_Ptr = _Ptr.m_Ptr;

		if (nullptr != m_Ptr)
			m_Ptr->AddRef();

		return *this;
	}

	T* operator->() const { return m_Ptr; }

	bool operator == (T* _Ptr) const { return m_Ptr == _Ptr; }
	bool operator == (const Ptr<T>& _Ptr) const { return m_Ptr == _Ptr.m_Ptr; }

	bool operator != (T* _Ptr) const { return m_Ptr != _Ptr; }
	bool operator != (const Ptr<T>& _Ptr) const { return m_Ptr != _Ptr.m_Ptr; }
public:
	Ptr()
		: m_Ptr(nullptr)
	{}
	Ptr(T* _Ptr) // 복사 생성자를 이용해서 카피를 진행함 컴파일러가
		: m_Ptr(_Ptr)
	{
		if (nullptr != m_Ptr)
			m_Ptr->AddRef();
	}
	Ptr(const Ptr<T>& _Ptr) // 스마트 포인터 자체로 생성하는 경우
		: m_Ptr(_Ptr.m_Ptr)
	{
		if (nullptr != m_Ptr)
			m_Ptr->AddRef();
	}
	~Ptr()
	{
		if (nullptr != m_Ptr)
			m_Ptr->Release();
	}
};


// p == ptr객체 를 비교하기 위해 생성
template<typename T>
bool operator == (void* _Pointer, const Ptr<T>& _Ptr)
{
	return _Pointer == _Ptr.Get();
}
template<typename T>
bool operator != (void* _Pointer, const Ptr<T>& _Ptr)
{
	return _Pointer != _Ptr.Get();
}