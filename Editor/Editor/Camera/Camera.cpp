#include "../../pch.h"
#include "Camera.h"
#include "../../Managers/InputManager/InputManager.h"

Camera::Camera() : m_X(0.0f), m_Y(0.0f), m_Zoom(1.0f), m_Rotation(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y)
{
    m_X = x;
    m_Y = y;
}

void Camera::Update()
{
    const float CAMERA_SPEED = 200.0f; // 픽셀/초
    const float ZOOM_SPEED = 1.0f;

    InputManager* inputManager = InputManager::GetInstance();

    // 방향키로 카메라 이동
    if (inputManager->IsKeyPressed(VK_LEFT))
    {
        m_X -= CAMERA_SPEED * 0.016f; // 대략 60FPS 기준
    }
    if (inputManager->IsKeyPressed(VK_RIGHT))
    {
        m_X += CAMERA_SPEED * 0.016f;
    }
    if (inputManager->IsKeyPressed(VK_UP))
    {
        m_Y -= CAMERA_SPEED * 0.016f;
    }
    if (inputManager->IsKeyPressed(VK_DOWN))
    {
        m_Y += CAMERA_SPEED * 0.016f;
    }

    // 줌 기능 (Page Up/Down)
    if (inputManager->IsKeyDown(VK_PRIOR)) // Page Up
    {
        m_Zoom += ZOOM_SPEED * 0.1f;
        if (m_Zoom > 3.0f) m_Zoom = 3.0f;
    }
    if (inputManager->IsKeyDown(VK_NEXT)) // Page Down
    {
        m_Zoom -= ZOOM_SPEED * 0.1f;
        if (m_Zoom < 0.2f) m_Zoom = 0.2f;
    }

    // 카메라 리셋 (Home 키)
    if (inputManager->IsKeyDown(VK_HOME))
    {
        m_X = 0.0f;
        m_Y = 0.0f;
        m_Zoom = 1.0f;
        printf("Camera reset to origin\n");
    }
}

float Camera::GetX() const
{
    return m_X;
}

float Camera::GetY() const
{
    return m_Y;
}

float Camera::GetZoom() const
{
    return m_Zoom;
}

// 월드 좌표를 스크린 좌표로 변환
POINT Camera::WorldToScreen(float worldX, float worldY) const
{
    POINT screenPos;
    screenPos.x = static_cast<LONG>((worldX - m_X) * m_Zoom + WINCX / 2);
    screenPos.y = static_cast<LONG>((worldY - m_Y) * m_Zoom + WINCY / 2);
    return screenPos;
}

// 스크린 좌표를 월드 좌표로 변환
void Camera::ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const
{
    worldX = (screenX - WINCX / 2) / m_Zoom + m_X;
    worldY = (screenY - WINCY / 2) / m_Zoom + m_Y;
}