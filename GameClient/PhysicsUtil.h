#pragma once

// 주의: b2Vec2와 Vec3 타입을 인식할 수 있도록 
// 필요한 헤더(Box2D, Vector 관련)가 이 위에 포함되어 있어야 합니다.
class Phys
{
public:
    // 컴파일 타임 상수
    static constexpr float SCALE = 0.01f;
    static constexpr float INV_SCALE = 100.0f;

    // 1. 숫자를 물리 단위로 (World -> Phys)
    static inline float ToPhys(float v) { return v * SCALE; }

    // 2. 숫자를 픽셀 단위로 (Phys -> World)
    static inline float ToWorld(float v) { return v * INV_SCALE; }

    // 3. 벡터를 물리 단위로 (Vec3 -> b2Vec2)
    static inline b2Vec2 ToPhys(const Vec3& v)
    {
        return b2Vec2{ v.x * SCALE, v.y * SCALE };
    }

    // 4. 벡터를 픽셀 단위로 (b2Vec2 -> Vec3)
    static inline Vec3 ToWorld(const b2Vec2& v)
    {
        return Vec3{ v.x * INV_SCALE, v.y * INV_SCALE, 0.0f };
    }

private:
    // 유틸리티 클래스이므로 생성하지 못하도록 막음
    Phys() = delete;
};