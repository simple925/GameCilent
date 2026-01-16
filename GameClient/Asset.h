#pragma once
#include "Entity.h"
// 쉐이더 사운드 애니메이션 매쉬 텍스쳐이미지
class Asset :
    public Entity
{
private:
    wstring             m_Key;          // 에셋을 찾을때 사용할 이름
    wstring             m_RelativePath; // 상대경로(실제 Content 폴더에 저장 되어있는 위치)

    // RTTI     : RunTime Type Identification
    // 런타임 도중에, 객체의 실제 타입을 알아내는 기술
    const ASSET_TYPE    m_Type;         // 에셋타입
public:
    ASSET_TYPE getType() { return m_Type; }
public:
    const wstring& getKey() { return m_Key; }
    const wstring& getRelativePath() { return m_RelativePath; }

public:
    // 기본생성자 제거, 자신이 어떤 타입의 Asset 인지 입력으로 받는 버전의 생성자만 남겨둠
    // 생성자가 하나라도 있으면 컴파일러는 자동으로 생성자를 안만든다!!
    Asset(ASSET_TYPE _Type);
    virtual ~Asset();
};

