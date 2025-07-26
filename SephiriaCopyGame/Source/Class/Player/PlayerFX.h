#pragma once
#include <Engine/Object/Component/Component.h>

class InputManager;
class Camera;

class PlayerFX : public Component
{
public:
    explicit PlayerFX(Object* owner);
    PlayerFX(const PlayerFX& other);

public:
    Component* CloneImpl() const override;
    void Update(float deltaTime) override;

    void SetRotation(float angle);
    float GetX() const { return m_Position.x; }
    float GetY() const { return m_Position.y; }
    float GetRotation() const { return m_Rotation; }

private:
    void UpdateRotationToMouse();
    float CalculateAngleToMouse();
    void UpdatePositionAroundPlayer(float playerX, float playerY);

private:
    Vector2 m_Position;
    float m_Rotation;
    Vector2 m_OffsetFromPlayer;
    bool m_IsFlipped;
};