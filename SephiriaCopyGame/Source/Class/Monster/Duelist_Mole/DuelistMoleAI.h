#pragma once
#include <Engine/Object/Component/Component.h>

class Object;

enum class DuelistMoleState
{
    IDLE,       // ��� ����
    TRACKING,   // �÷��̾� ����
    PREPARING,  // ���� �غ� (1�� ���)
    CHARGING    // ���� ����
};

class DuelistMoleAI : public Component
{
public:
    explicit DuelistMoleAI(Object* owner);
    DuelistMoleAI(const DuelistMoleAI& other);

public:
    Component* CloneImpl() const override;
    void Update(float deltaTime) override;

    // Ÿ�� ����
    void SetTarget(Object* target);
    Object* GetTarget() const { return m_pTarget; }

    // ���� ������ ����
    void SetDetectionRange(float range) { m_DetectionRange = range; }
    void SetAttackRange(float range) { m_AttackRange = range; }
    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    void SetChargeSpeed(float speed) { m_ChargeSpeed = speed; }
    void SetPrepareTime(float time) { m_PrepareTime = time; }

    // ���� ��ȸ
    DuelistMoleState GetCurrentState() const { return m_CurrentState; }

private:
    // ���� �Լ���
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
    // ����
    DuelistMoleState m_CurrentState;
    Object* m_pTarget;

    // ��������
    float m_DetectionRange;    // ���� ����
    float m_AttackRange;       // ���� ����
    float m_MoveSpeed;         // �Ϲ� �̵� �ӵ�
    float m_ChargeSpeed;       // ���� �ӵ�
    float m_PrepareTime;       // ���� �غ� �ð�

    // Ÿ�̸�
    float m_PrepareTimer;      // ���� �غ� Ÿ�̸�

    // ���� ����
    Vector2 m_ChargeDirection; // ���� ����
    bool m_IsCharging;         // ���� ������ Ȯ��
};