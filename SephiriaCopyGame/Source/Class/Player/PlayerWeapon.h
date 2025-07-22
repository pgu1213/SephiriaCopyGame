#pragma once
#include <Engine/Object/Component/Component.h>

class InputManager;
class Camera;

class PlayerWeapon : public Component
{
public:
    explicit PlayerWeapon(Object* owner);
    PlayerWeapon(const PlayerWeapon& other);

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
    struct Vector2
    {
        float x, y;
        Vector2() : x(0), y(0) {}
        Vector2(float _x, float _y) : x(_x), y(_y) {}
    };

    Vector2 m_Position;
    float m_Rotation;

    Vector2 m_OffsetFromPlayer;
};