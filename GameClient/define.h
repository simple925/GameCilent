#pragma once
#define SINGLE(Type) private: Type(); Type(const Type* _Other) = delete; ~Type(); friend class singleton<Type>;

#define MAX_LAYER 32