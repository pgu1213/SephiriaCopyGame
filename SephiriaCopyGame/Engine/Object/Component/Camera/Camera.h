#pragma once
#include <Engine/Object/Component/Component.h>

class Camera : public Component
{
public:
    Camera(Object* owner);
    Camera(const Camera& other); // 복사 생성자
    virtual ~Camera();

protected:
    Component* CloneImpl() const override;

public:
    void CopyFrom(const IPrototypeable* source) override;
    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

public:
    // 카메라 위치 설정
    void SetPosition(float x, float y);
    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    // 카메라 회전 설정
    void SetRotation(float rotation);
    float GetRotation() const;

    // 카메라 줌 설정
    void SetZoom(float zoom);
    float GetZoom() const;

    // 카메라 이동
    void Move(float deltaX, float deltaY);
    void Move(const Vector2& delta);

    // 좌표 변환
    Vector2 WorldToScreen(const Vector2& worldPos) const;
    Vector2 ScreenToWorld(const Vector2& screenPos) const;
    POINT WorldToScreen(float worldX, float worldY) const;
    void ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;

    // 카메라 뷰포트
    void SetViewportSize(float width, float height);
    Vector2 GetViewportSize() const;

    // 카메라 경계 설정 (월드 좌표)
    void SetBounds(float left, float top, float right, float bottom);
    void ClearBounds();
    bool HasBounds() const;

    // 타겟 따라가기
    void SetTarget(Object* target);
    void SetFollowSpeed(float speed);
    void SetFollowOffset(const Vector2& offset);

    // 카메라 흔들림 효과
    void Shake(float intensity, float duration);
    void StopShake();

    // 현재 활성 카메라 관리
    static void SetMainCamera(Camera* camera);
    static Camera* GetMainCamera();

private:
    void UpdateTargetFollow(float deltaTime);
    void UpdateShake(float deltaTime);
    void ApplyBounds();

private:
    Vector2 m_Position;          // 카메라 위치
    float m_Rotation;            // 카메라 회전 (라디안)
    float m_Zoom;                // 카메라 줌 (1.0f = 기본)
    Vector2 m_ViewportSize;      // 뷰포트 크기

    // 카메라 경계
    bool m_HasBounds;
    float m_BoundsLeft, m_BoundsTop, m_BoundsRight, m_BoundsBottom;

    // 타겟 따라가기
    Object* m_Target;
    float m_FollowSpeed;
    Vector2 m_FollowOffset;

    // 카메라 흔들림
    bool m_IsShaking;
    float m_ShakeIntensity;
    float m_ShakeDuration;
    float m_ShakeTimer;
    Vector2 m_ShakeOffset;

    // 메인 카메라
    static Camera* s_MainCamera;
};