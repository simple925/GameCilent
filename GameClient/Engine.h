#pragma once
#include "single.h"
// 외부에서 생성자를 호출하지 못하도록 함
#define SINGLE(Type) private:\
						Type() {}\
						friend class singleton<Type>;
class Engine
	: public singleton<Engine>
{
private:
	SINGLE(Engine)
public:
	void Progres();
};