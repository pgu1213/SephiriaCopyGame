#pragma once
#include <Engine/Object/Component/Component.h>

class Object;

enum class DuelistMoleState
{
    IDLE,       // 대기 상태
    TRACKING,   // 플레이어 추적
    PREPARING,  // 공격 준비 (1초 대기)
    CHARGING    // 돌진 공격
};

class DuelistMoleAI : public Component
{
public:
    explicit DuelistMoleAI(Object* owner);
    DuelistMoleAI(const DuelistMoleAI& other);

public:
    Component* CloneImpl() const override;
    void Update(float deltaTime) override;

    // 타겟 설정
    void SetTarget(Object* target);
    Object* GetTarget() const { return m_pTarget; }

    // 설정 가능한 값들
    void SetDetectionRange(float range) { m_DetectionRange = range; }
    void SetAttackRange(float range) { m_AttackRange = range; }
    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    void SetChargeSpeed(float speed) { m_ChargeSpeed = speed; }
    void SetPrepareTime(float time) { m_PrepareTime = time; }

    // 상태 조회
    DuelistMoleState GetCurrentState() const { return m_CurrentState; }

private:
    // 내부 함수들
    void UpdateIdle(float deltaTime);
    void UpdateTracking(float deltaTime);
    void UpdatePreparing(float deltaTime);
    void UpdateCharging(float deltaTime);

    bool IsPlayerInRange(float range);
    float GetDistanceToPlayer();
    Vector2 GetDirectionToPlayer();
    void MoveTowardsPlayer(float deltaTime, float speed);
    void ChangeState(DuelistMoleState newState);

private:
    // 상태
    DuelistMoleState m_CurrentState;
    Object* m_pTarget;

    // 설정값들
    float m_DetectionRange;    // 인지 범위
    float m_AttackRange;       // 공격 범위
    float m_MoveSpeed;         // 일반 이동 속도
    float m_ChargeSpeed;       // 돌진 속도
    float m_PrepareTime;       // 공격 준비 시간

    // 타이머
    float m_PrepareTimer;      // 공격 준비 타이머

    // 돌진 관련
    Vector2 m_ChargeDirection; // 돌진 방향
    bool m_IsCharging;         // 돌진 중인지 확인
};