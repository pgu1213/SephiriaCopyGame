#include "pch.h"
#include "PlayerFX.h"
#include "Engine/Managers/InputManager/InputManager.h"
#include "Engine/Object/Component/Camera/Camera.h"
#include "Engine/Object/Object/Object.h"
#include "Engine/Managers/SceneManager/SceneManager.h"
#include "Engine/Object/Component/SpriteRenderer/SpriteRenderer.h"

PlayerFX::PlayerFX(Object* owner)
    : Component(owner)
    , m_Position(0.0f, 0.0f)
    , m_Rotation(0.0f)
    , m_OffsetFromPlayer(30.0f, 0.0f)
    , m_IsFlipped(false)
{
}

PlayerFX::PlayerFX(const PlayerFX& other)
    : Component(other.m_pObjOwner)
    , m_Position(other.m_Position)
    , m_Rotation(other.m_Rotation)
    , m_OffsetFromPlayer(other.m_OffsetFromPlayer)
    , m_IsFlipped(other.m_IsFlipped)
{
}

Component* PlayerFX::CloneImpl() const
{
    return new PlayerFX(*this);
}

void PlayerFX::Update(float deltaTime)
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
        // �÷��̾� ������Ʈ�� ������ ���� ��ġ ����
        // SetPosition(m_Position.x, m_Position.y); // �ʿ�� ���
    }

    // ��Ŭ�� ���� �ֵθ��� (�޺�)
    // ��Ŭ�� ���� ���

}

void PlayerFX::SetRotation(float angle)
{
    float adjustedAngle = angle + 90.0f;

    // -180�� ~ 180�� �ý��ۿ��� ���� ����
    // 7~11�� ����: (90�� < angle <= 180��) �Ǵ� (-180�� <= angle < -90��)
    bool shouldFlip = (angle > 90.0f) || (angle < -90.0f);

    if (shouldFlip != m_IsFlipped)
    {
        m_IsFlipped = shouldFlip;
        m_pObjOwner->GetComponent<SpriteRenderer>()->SetFlipX(m_IsFlipped);
    }

    // ��������Ʈ�� �����Ǿ��� �� ȸ���� ���ݴ�� ����
    //if (m_IsFlipped)
    //{
    //    adjustedAngle = -angle;  // �ܼ��� ������ ������ ����
    //}

    float angleInRadians = DegreeToRadian(adjustedAngle);
    m_pObjOwner->SetRotation(angleInRadians);
}

void PlayerFX::UpdateRotationToMouse()
{
    float angleToMouse = CalculateAngleToMouse();
    float anglechange = angleToMouse - m_Rotation;

    // ���� ���̸� -180�� ~ 180�� ������ ����ȭ
    while (anglechange < -180.0f)
    {
        anglechange += 360.0f;
    }
    while (anglechange > 180.0f)
    {
        anglechange -= 360.0f;
    }

    float lerpFactor = 0.2f;
    m_Rotation += anglechange * lerpFactor;

    // m_Rotation�� -180�� ~ 180�� ������ ����
    while (m_Rotation >= 180.0f) m_Rotation -= 360.0f;
    while (m_Rotation < -180.0f) m_Rotation += 360.0f;

    SetRotation(m_Rotation);
}

float PlayerFX::CalculateAngleToMouse()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* cameraObject = currentScene->FindGameObject("MainCamera");
    Camera* camera = cameraObject->GetComponent<Camera>();

    if (camera)
    {
        float worldMouseX, worldMouseY;
        camera->ScreenToWorld(mouseX, mouseY, worldMouseX, worldMouseY);
        mouseX = worldMouseX;
        mouseY = worldMouseY;
    }

    Scene* currentscene = SceneManager::GetInstance()->GetCurrentScene();
    Object* playerObject = currentscene->FindGameObject("Player");

    if (!playerObject)
    {
        return m_Rotation;
    }

    auto playerPos = playerObject->GetTransform().position;
    float playerX = static_cast<float>(playerPos.x);
    float playerY = static_cast<float>(playerPos.y);

    float deltaX = mouseX - playerX;
    float deltaY = mouseY - playerY;

    float distanceSquared = deltaX * deltaX + deltaY * deltaY;
    const float MIN_DISTANCE_SQUARED = 25.0f;

    if (distanceSquared < MIN_DISTANCE_SQUARED)
    {
        return m_Rotation;
    }

    float angleInRadians = atan2f(deltaY, deltaX);
    float angleInDegrees = RadianToDegree(angleInRadians);

    // -180�� ~ 180�� ������ ����ȭ (3��=0��, 9��=180��/-180��)
    while (angleInDegrees > 180.0f) angleInDegrees -= 360.0f;
    while (angleInDegrees < -180.0f) angleInDegrees += 360.0f;

    return angleInDegrees;
}

void PlayerFX::UpdatePositionAroundPlayer(float playerX, float playerY)
{
    float rotationInRadians = DegreeToRadian(m_Rotation);

    float rotatedOffsetX = m_OffsetFromPlayer.x * cosf(rotationInRadians) - m_OffsetFromPlayer.y * sinf(rotationInRadians);
    float rotatedOffsetY = m_OffsetFromPlayer.x * sinf(rotationInRadians) + m_OffsetFromPlayer.y * sinf(rotationInRadians);

    m_pObjOwner->SetPosition(playerX + rotatedOffsetX, playerY + rotatedOffsetY);
}