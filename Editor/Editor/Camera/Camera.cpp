#include "../../pch.h"
#include "Camera.h"

extern HWND g_hWnd;

Camera::Camera()
    : m_X(0.0f)
    , m_Y(0.0f)
    , m_Zoom(1.0f)
    , m_Rotation(0.0f)
    , m_ScreenWidth(1280)
    , m_ScreenHeight(960)
{
    // 화면 크기 가져오기
    RECT clientRect;
    if (GetClientRect(g_hWnd, &clientRect))
    {
        m_ScreenWidth = clientRect.right;
        m_ScreenHeight = clientRect.bottom;
    }
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y)
{
    m_X = x;
    m_Y = y;
}

void Camera::Move(float deltaX, float deltaY)
{
    m_X += deltaX;
    m_Y += deltaY;
}

void Camera::Update()
{
    // 카메라 업데이트 로직 (필요시 확장)
}

void Camera::SetZoom(float zoom)
{
    m_Zoom = max(0.1f, min(5.0f, zoom)); // 줌 범위 제한
}

POINT Camera::WorldToScreen(float worldX, float worldY) const
{
    POINT screenPos;
    screenPos.x = (int)((worldX - m_X) * m_Zoom + m_ScreenWidth / 2);
    screenPos.y = (int)((worldY - m_Y) * m_Zoom + m_ScreenHeight / 2);
    return screenPos;
}

void Camera::ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const
{
    worldX = (screenX - m_ScreenWidth / 2) / m_Zoom + m_X;
    worldY = (screenY - m_ScreenHeight / 2) / m_Zoom + m_Y;
}