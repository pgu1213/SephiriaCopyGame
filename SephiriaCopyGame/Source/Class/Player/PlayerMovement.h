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

    // Getters/Setters
    float GetSpeed() const { return m_Speed; }
    void SetSpeed(float speed) { m_Speed = speed; }

    bool IsAttacking() const { return m_IsAttacking; }
    bool IsDashing() const { return m_IsDashing; }
    int GetAttackComboCount() const { return m_AttackComboCount; }

private:
    // 기존 함수들
    void UpdateAnimationBasedOnMouse(const Vector2& inputDir);
    void HandleMovement(const Vector2& inputDir, float deltaTime);
    void HandleDash(const Vector2& inputDir);

    // 새로 추가된 공격 관련 함수들
    void HandleAttack(const Vector2& inputDir);
    Vector2 CalculateAttackLungeDirection(const Vector2& inputDir);
    Vector2 GetMouseDirection();
    void PerformAttackLunge(const Vector2& direction);

private:
    // 기본 이동 관련
    float m_Speed;
    Vector2 m_Velocity;

    // 대쉬 관련
    float m_DashSpeed;
    float m_DashDuration;
    float m_DashCooldown;
    float m_DashTimer;
    float m_DashCooldownTimer;
    bool m_IsDashing;
    Vector2 m_DashDirection;

    // 공격 관련 새로 추가된 변수들
    int m_AttackComboCount;        // 현재 콤보 단계 (1, 2, 3)
    float m_AttackComboMaxTime;    // 콤보 유지 최대 시간
    float m_AttackComboTimer;      // 콤보 타이머
    bool m_IsAttacking;            // 공격 중 여부
    float m_AttackDuration;        // 공격 지속 시간
    float m_AttackTimer;           // 공격 타이머
    float m_AttackLungeSpeed;      // 공격 시 돌진 속도
    float m_AttackLungeDuration;   // 공격 시 돌진 지속 시간
    float m_AttackLungeTimer;      // 공격 돌진 타이머
    bool m_IsAttackLunging;        // 공격 돌진 중 여부
    Vector2 m_AttackLungeDirection; // 공격 돌진 방향
};