#pragma once
#include <Engine/Object/Component/Component.h>

class Camera : public Component
{
public:
    Camera(Object* owner);
    Camera(const Camera& other); // ���� ������
    virtual ~Camera();

protected:
    Component* CloneImpl() const override;

public:
    void CopyFrom(const IPrototypeable* source) override;
    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

public:
    // ī�޶� ��ġ ����
    void SetPosition(float x, float y);
    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    // ī�޶� ȸ�� ����
    void SetRotation(float rotation);
    float GetRotation() const;

    // ī�޶� �� ����
    void SetZoom(float zoom);
    float GetZoom() const;

    // ī�޶� �̵�
    void Move(float deltaX, float deltaY);
    void Move(const Vector2& delta);

    // ��ǥ ��ȯ
    Vector2 WorldToScreen(const Vector2& worldPos) const;
    Vector2 ScreenToWorld(const Vector2& screenPos) const;
    POINT WorldToScreen(float worldX, float worldY) const;
    void ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;

    // ī�޶� ����Ʈ
    void SetViewportSize(float width, float height);
    Vector2 GetViewportSize() const;

    // ī�޶� ��� ���� (���� ��ǥ)
    void SetBounds(float left, float top, float right, float bottom);
    void ClearBounds();
    bool HasBounds() const;

    // Ÿ�� ���󰡱�
    void SetTarget(Object* target);
    void SetFollowSpeed(float speed);
    void SetFollowOffset(const Vector2& offset);

    // ī�޶� ��鸲 ȿ��
    void Shake(float intensity, float duration);
    void StopShake();

    // ���� Ȱ�� ī�޶� ����
    static void SetMainCamera(Camera* camera);
    static Camera* GetMainCamera();

private:
    void UpdateTargetFollow(float deltaTime);
    void UpdateShake(float deltaTime);
    void ApplyBounds();

private:
    Vector2 m_Position;          // ī�޶� ��ġ
    float m_Rotation;            // ī�޶� ȸ�� (����)
    float m_Zoom;                // ī�޶� �� (1.0f = �⺻)
    Vector2 m_ViewportSize;      // ����Ʈ ũ��

    // ī�޶� ���
    bool m_HasBounds;
    float m_BoundsLeft, m_BoundsTop, m_BoundsRight, m_BoundsBottom;

    // Ÿ�� ���󰡱�
    Object* m_Target;
    float m_FollowSpeed;
    Vector2 m_FollowOffset;

    // ī�޶� ��鸲
    bool m_IsShaking;
    float m_ShakeIntensity;
    float m_ShakeDuration;
    float m_ShakeTimer;
    Vector2 m_ShakeOffset;

    // ���� ī�޶�
    static Camera* s_MainCamera;
};