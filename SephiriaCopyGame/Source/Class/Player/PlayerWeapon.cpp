#include "pch.h"
#include "PlayerWeapon.h"
#include "Engine/Managers/InputManager/InputManager.h"
#include "Engine/Object/Component/Camera/Camera.h"
#include "Engine/Object/Object/Object.h"
#include "Engine/Managers/SceneManager/SceneManager.h"

PlayerWeapon::PlayerWeapon(Object* owner)
    : Component(owner)
    , m_Position(0.0f, 0.0f)
    , m_Rotation(0.0f)
    , m_OffsetFromPlayer(30.0f, 0.0f) // 플레이어로부터 30픽셀 떨어진 위치
{
}

PlayerWeapon::PlayerWeapon(const PlayerWeapon& other)
    : Component(other.m_pObjOwner)
    , m_Position(other.m_Position)
    , m_Rotation(other.m_Rotation)
    , m_OffsetFromPlayer(other.m_OffsetFromPlayer)
{
}

Component* PlayerWeapon::CloneImpl() const
{
    return new PlayerWeapon(*this);
}

void PlayerWeapon::Update(float deltaTime)
{
    UpdateRotationToMouse();

    Scene* currentscene = SceneManager::GetInstance()->GetCurrentScene();
    Object* playerObject = currentscene->FindGameObject("Player");
    if (playerObject)
    {
        auto playerPos = playerObject->GetTransform().position;
        UpdatePositionAroundPlayer(static_cast<float>(playerPos.x), static_cast<float>(playerPos.y));
    }
    else
    {
        // 플레이어 오브젝트가 없으면 현재 위치 유지
        // SetPosition(m_Position.x, m_Position.y); // 필요시 사용
	}

    // 좌클릭 무기 휘두르기 (콤보)
    // 우클릭 방패 방어

}

void PlayerWeapon::SetRotation(float angle)
{
    m_Rotation = angle;
}

void PlayerWeapon::UpdateRotationToMouse()
{
    float angleToMouse = CalculateAngleToMouse();
    SetRotation(angleToMouse);
}

float PlayerWeapon::CalculateAngleToMouse()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();

    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    float deltaX = mouseX - m_Position.x;
    float deltaY = mouseY - m_Position.y;

    float angleInRadians = atan2f(deltaY, deltaX);

    float angleInDegrees = angleInRadians * 180.0f / PI;

    return angleInDegrees;
}

void PlayerWeapon::UpdatePositionAroundPlayer(float playerX, float playerY)
{
    // 회전 계산
    float rotationInRadians = m_Rotation * PI / 180.0f;

    float rotatedOffsetX = m_OffsetFromPlayer.x * cosf(rotationInRadians) - m_OffsetFromPlayer.y * sinf(rotationInRadians);
    float rotatedOffsetY = m_OffsetFromPlayer.x * sinf(rotationInRadians) + m_OffsetFromPlayer.y * cosf(rotationInRadians);

    m_pObjOwner->SetPosition(playerX + rotatedOffsetX, playerY + rotatedOffsetY);
}