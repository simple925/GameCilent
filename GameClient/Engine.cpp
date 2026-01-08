#include "Engine.h"

// 정적맴버변수 초기화
//Engine* Engine::m_This = nullptr;
//
// 장점
// - 런타임 중에 필요하지 않으면 Destroy 를 호출해서 객체를 삭제할 수 있다.
// 단점
// - 프로그램 종료전에 반드시 Destroy 함수를 호출 해야함
//Engine* Engine::GetEngine()
//{
//	// GetEngine 함수는 정적 '맴버' 함수이기 때문에
//	// Engine 클래스의 private 기능에 접근이 가능
//	if (nullptr == m_This) {
//		m_This = new Engine;
//	}
//	return m_This;
//}
//
//void Engine::Destroy()
//{
//	if (nullptr != m_This) {
//		delete m_This;
//		m_This = nullptr;
//	}
//}

// 장점
// - 객체 소멸 신경 쓸 필요 없음
// 단점
// - 런타임 시 객체 삭제 불가능
Engine* Engine::GetEngine()
{
	// singleton 으로 객체의 개수를 1개로 줄임
	// 데이터 영역에 정적변수로 Engine 객체를 생성
	static Engine engine;
	//engine.Engine::Engine(); // 컴파일 타임에 생기지롱
	// 그 주소를 준다.
	return &engine;
}
