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
	private:
	float m_X;
	float m_Y;
	float m_Zoom;
	float m_Rotation;
};

