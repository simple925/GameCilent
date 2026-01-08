#pragma once
//
//// 프로그램 관리자 자료형
//// 자료형(클래스)으로 생성시킬 수 있는 객체의 개수를 1개로 제한
//
//// 디자인 패턴
//// iterator pattern - 자료구조 컨테이너를 설계할때, 내부동작 원리는 감추고,
////                   저장된 데이터를 쉽게 접근할 수 있게 하는 iterator 클래스를 같이 제공한다.
//// singleton pattern - 자료구조 컨테이너를 설계할때, 내부동작 우너리는 감추고,
////                    저장된 데이터를 쉽게 접근할 수 있게 하는 iterator 클래스를 같이 제공한다.
//
//class Engine
//{
//private:
//	// 정적 맴버변수
//	// 선언된 클래스 전용 전역변수, 데이터 영역 메모리에 1개만 저장
//	// 선언된 클래스의 객체가 여러개 만들어진다고 해서, 그 객체안에 들어있는 개념이 아니다.
//	static int		m_Engine; // 정적 맴버 변수 public인 경우 객체 없이 접근 가능
//
//public:
//	// 맴버변수
//	// 선언된 클래스 객체마다 안에 들어있다, 클래스 자료형이 어떻게 구성되어있는지를 나타냄
//	int				m_i;
//
//	// 맴버함수
//	void SetData(int _Data) {
//		m_Engine = _Data;
//		this->m_i = _Data;
//	}
//
//	// 정적 맴버함수
//	// 객체 없이 호출이 가능한 맴버함수
//	// 객체 없이 호출이 가능하다? ==> this 포인터가 없다.
//	// 클래스 전용 전역함수 + 클래스 private 에 접근이 가능하다.
//	static void SetData_Static(int _Data) {
//		// 자료형 전용 유일 전역변수인 정적 맴버는 접근 가능
//		m_Engine = _Data;
//		// 맴버 함수를 호출한 객체의 맴버를 뜻하는 일반 맴버변수는 접근이 불가능(this 에 주소를 받아오지 않기 때문에)
//		//this->m_i = _Data;
//	}
//};
//
//// global 변수도 데이터 영역에 생성 - 다른영역에서 접근이 됨
//// static 데이터 영역에 생성 - 다른 영역에선 접근이 안됨
//
//// static global = 0; 전역변수가 전언된 경우 파일 내에서만 사용가능한 변수


//class Engine
//{
//public:
//	static Engine* m_This; // 전방선언
//public:
//	// Engine 객체 반환함수
//	static Engine* GetEngine();
//	// Engine 객체 지우는 함수
//	static void Destroy();
//private:
//	// 외부에서 생성자를 호출하지 못하도록 함
//	Engine() {
//
//	}
//};
class Engine
{
public:
	// Engine 객체 반환함수
	static Engine* GetEngine();
private:
	// 외부에서 생성자를 호출하지 못하도록 함
	Engine() {

	}
};