#pragma once

class Camera
{
public:
    Camera();
    ~Camera();

public:
    void SetPosition(float x, float y);
    void Move(float deltaX, float deltaY);
    void Update();
    void SetZoom(float zoom);

    float GetX() const { return m_X; }
    float GetY() const { return m_Y; }
    float GetZoom() const { return m_Zoom; }

    POINT WorldToScreen(float worldX, float worldY) const;
    void ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;

private:
    float m_X;
    float m_Y;
    float m_Zoom;
    float m_Rotation;

    // 화면 정보
    int m_ScreenWidth;
    int m_ScreenHeight;
};