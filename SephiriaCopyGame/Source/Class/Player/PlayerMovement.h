#pragma once
#include <Engine/Object/Component/Component.h>

class PlayerMovement : public Component
{
public:
    PlayerMovement(Object* owner);
    PlayerMovement(const PlayerMovement& other); // ���� ������
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
    // ���� �Լ���
    void UpdateAnimationBasedOnMouse(const Vector2& inputDir);
    void HandleMovement(const Vector2& inputDir, float deltaTime);
    void HandleDash(const Vector2& inputDir);

    // ���� �߰��� ���� ���� �Լ���
    void HandleAttack(const Vector2& inputDir);
    Vector2 CalculateAttackLungeDirection(const Vector2& inputDir);
    Vector2 GetMouseDirection();
    void PerformAttackLunge(const Vector2& direction);

private:
    // �⺻ �̵� ����
    float m_Speed;
    Vector2 m_Velocity;

    // �뽬 ����
    float m_DashSpeed;
    float m_DashDuration;
    float m_DashCooldown;
    float m_DashTimer;
    float m_DashCooldownTimer;
    bool m_IsDashing;
    Vector2 m_DashDirection;

    // ���� ���� ���� �߰��� ������
    int m_AttackComboCount;        // ���� �޺� �ܰ� (1, 2, 3)
    float m_AttackComboMaxTime;    // �޺� ���� �ִ� �ð�
    float m_AttackComboTimer;      // �޺� Ÿ�̸�
    bool m_IsAttacking;            // ���� �� ����
    float m_AttackDuration;        // ���� ���� �ð�
    float m_AttackTimer;           // ���� Ÿ�̸�
    float m_AttackLungeSpeed;      // ���� �� ���� �ӵ�
    float m_AttackLungeDuration;   // ���� �� ���� ���� �ð�
    float m_AttackLungeTimer;      // ���� ���� Ÿ�̸�
    bool m_IsAttackLunging;        // ���� ���� �� ����
    Vector2 m_AttackLungeDirection; // ���� ���� ����
};