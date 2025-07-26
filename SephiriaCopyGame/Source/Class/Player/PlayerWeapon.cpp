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
    // 3시 기준이라 꺾어야 함
    float adjustedAngle = angle + 90.0f;

    float angleInRadians = DegreeToRadian(adjustedAngle);
    m_pObjOwner->SetRotation(angleInRadians);
}

void PlayerWeapon::UpdateRotationToMouse()
{
    float angleToMouse = CalculateAngleToMouse();
    float anglechange = angleToMouse - m_Rotation;

    while (anglechange < -180.0f)
    {
        anglechange += 360.0f;
    }
    while (anglechange > 180.0f)
    {
        anglechange -= 360.0f;
    }

    // 부드러운 회전을 위한 보간
    float lerpFactor = 0.2f;
    m_Rotation += anglechange * lerpFactor;

    // 각도를 0-360도 범위로 정규화
    while (m_Rotation >= 360.0f) m_Rotation -= 360.0f;
    while (m_Rotation < 0.0f) m_Rotation += 360.0f;

    SetRotation(m_Rotation);
}

float PlayerWeapon::CalculateAngleToMouse()
{
    POINT mousePos = InputManager::GetInstance()->GetMousePosition();
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // 카메라 가져오기
    Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    Object* cameraObject = currentScene->FindGameObject("MainCamera");
    Camera* camera = cameraObject->GetComponent<Camera>();

    if (camera)
    {
        // 화면 좌표를 월드 좌표로 변환
        float worldMouseX, worldMouseY;
        camera->ScreenToWorld(mouseX, mouseY, worldMouseX, worldMouseY);
        mouseX = worldMouseX;
        mouseY = worldMouseY;
    }

    // TODO - pOwner로 하면 떨림 현상 생겨서 임시조치
    // 플레이어의 현재 월드 위치 가져오기
    Scene* currentscene = SceneManager::GetInstance()->GetCurrentScene();
    Object* playerObject = currentscene->FindGameObject("Player");

    if(!playerObject)
    {
        // 플레이어 오브젝트가 없으면 현재 각도 유지
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
        return m_Rotation; // 현재 각도 유지
    }

    float angleInRadians = atan2f(deltaY, deltaX);
    float angleInDegrees = RadianToDegree(angleInRadians);

    // 0-360도 범위로 정규화
    while (angleInDegrees < 0.0f) angleInDegrees += 360.0f;
    while (angleInDegrees >= 360.0f) angleInDegrees -= 360.0f;

    return angleInDegrees;
}

void PlayerWeapon::UpdatePositionAroundPlayer(float playerX, float playerY)
{
    // 회전 계산
    float rotationInRadians = DegreeToRadian(m_Rotation);

    float rotatedOffsetX = m_OffsetFromPlayer.x * cosf(rotationInRadians) - m_OffsetFromPlayer.y * sinf(rotationInRadians);
    float rotatedOffsetY = m_OffsetFromPlayer.x * sinf(rotationInRadians) + m_OffsetFromPlayer.y * cosf(rotationInRadians);

    m_pObjOwner->SetPosition(playerX + rotatedOffsetX, playerY + rotatedOffsetY);
}