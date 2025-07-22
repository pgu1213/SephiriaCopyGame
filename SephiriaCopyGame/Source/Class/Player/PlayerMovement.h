#pragma once
#include <Engine/Object/Component/Component.h>

class PlayerMovement : public Component
{
public:
    PlayerMovement(Object* owner);
    PlayerMovement(const PlayerMovement& other); // 복사 생성자
protected:
    Component* CloneImpl() const override;
public:
    void Update(float deltaTime) override;

private:
    float m_Speed;             // 이동 속도
    Vector2 m_Velocity;      // 현재 속도

    // 대쉬 관련 변수들
    float m_DashSpeed;       // 대쉬 속도
    float m_DashDuration;    // 대쉬 지속 시간
    float m_DashCooldown;    // 대쉬 쿨다운 시간

    float m_DashTimer;       // 현재 대쉬 시간
    float m_DashCooldownTimer; // 현재 쿨다운 시간
    bool m_IsDashing;        // 대쉬 중인지 확인
    Vector2 m_DashDirection; // 대쉬 방향
};