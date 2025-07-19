#pragma once
class Camera
{
public:
	Camera();
	~Camera();
public:
	void SetPosition(float x, float y);
	void Update();
	float GetX() const;
	float GetY() const;
	float GetZoom() const;
	POINT WorldToScreen(float worldX, float worldY) const;
	void ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;
	private:
	float m_X;
	float m_Y;
	float m_Zoom;
	float m_Rotation;
};

