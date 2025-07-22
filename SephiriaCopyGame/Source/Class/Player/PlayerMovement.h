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

private:
    float m_Speed;             // �̵� �ӵ�
    Vector2 m_Velocity;      // ���� �ӵ�

    // �뽬 ���� ������
    float m_DashSpeed;       // �뽬 �ӵ�
    float m_DashDuration;    // �뽬 ���� �ð�
    float m_DashCooldown;    // �뽬 ��ٿ� �ð�

    float m_DashTimer;       // ���� �뽬 �ð�
    float m_DashCooldownTimer; // ���� ��ٿ� �ð�
    bool m_IsDashing;        // �뽬 ������ Ȯ��
    Vector2 m_DashDirection; // �뽬 ����
};