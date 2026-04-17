#pragma once
#include "AComputeShader.h"
#include "ATexture.h"

class ASetColorCS :
    public AComputeShader
{
private:
    Ptr<ATexture>   m_Target;
    Vec4            m_Color;

public:
    void SetTargetTex(Ptr<ATexture> _Tex) { m_Target = _Tex; }
    void SetColor(Vec4 _Color) { m_Color = _Color; }

public:
    virtual int Binding() override;
    virtual void CalcGroupNum() override;
    virtual void Clear() override;

public:
    ASetColorCS();
    virtual ~ASetColorCS();
};

