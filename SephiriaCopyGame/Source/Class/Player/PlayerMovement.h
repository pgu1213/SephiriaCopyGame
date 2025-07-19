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
    void SetSpeed(float speed) { m_Speed = speed; }
    float GetSpeed() const { return m_Speed; }

private:
    float m_Speed = 200.0f;  // �ȼ�/��
    Vector2 m_Velocity = { 0.0f, 0.0f };
};