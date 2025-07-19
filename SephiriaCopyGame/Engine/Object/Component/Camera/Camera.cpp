#include <pch.h>
#include "Camera.h"
#include <Engine/Object/Object/Object.h>

Camera* Camera::s_MainCamera = nullptr;

Camera::Camera(Object* owner)
    : Component(owner)
    , m_Position(0.0f, 0.0f)
    , m_Rotation(0.0f)
    , m_Zoom(1.0f)
    , m_ViewportSize(static_cast<float>(WINCX), static_cast<float>(WINCY))
    , m_HasBounds(false)
    , m_BoundsLeft(0.0f)
    , m_BoundsTop(0.0f)
    , m_BoundsRight(0.0f)
    , m_BoundsBottom(0.0f)
    , m_Target(nullptr)
    , m_FollowSpeed(5.0f)
    , m_FollowOffset(0.0f, 0.0f)
    , m_IsShaking(false)
    , m_ShakeIntensity(0.0f)
    , m_ShakeDuration(0.0f)
    , m_ShakeTimer(0.0f)
    , m_ShakeOffset(0.0f, 0.0f)
{
    // 첫 번째 카메라를 메인 카메라로 설정
    if (s_MainCamera == nullptr)
    {
        s_MainCamera = this;
    }
}

Camera::Camera(const Camera& other)
    : Component(other.m_pOwner)
    , m_Position(other.m_Position)
    , m_Rotation(other.m_Rotation)
    , m_Zoom(other.m_Zoom)
    , m_ViewportSize(other.m_ViewportSize)
    , m_HasBounds(other.m_HasBounds)
    , m_BoundsLeft(other.m_BoundsLeft)
    , m_BoundsTop(other.m_BoundsTop)
    , m_BoundsRight(other.m_BoundsRight)
    , m_BoundsBottom(other.m_BoundsBottom)
    , m_Target(other.m_Target)
    , m_FollowSpeed(other.m_FollowSpeed)
    , m_FollowOffset(other.m_FollowOffset)
    , m_IsShaking(false) // 흔들림은 복사하지 않음
    , m_ShakeIntensity(0.0f)
    , m_ShakeDuration(0.0f)
    , m_ShakeTimer(0.0f)
    , m_ShakeOffset(0.0f, 0.0f)
{
}

Camera::~Camera()
{
    if (s_MainCamera == this)
    {
        s_MainCamera = nullptr;
    }
}

Component* Camera::CloneImpl() const
{
    return new Camera(*this);
}

void Camera::CopyFrom(const IPrototypeable* source)
{
    const Camera* camera = dynamic_cast<const Camera*>(source);
    if (camera)
    {
        m_Position = camera->m_Position;
        m_Rotation = camera->m_Rotation;
        m_Zoom = camera->m_Zoom;
        m_ViewportSize = camera->m_ViewportSize;
        m_HasBounds = camera->m_HasBounds;
        m_BoundsLeft = camera->m_BoundsLeft;
        m_BoundsTop = camera->m_BoundsTop;
        m_BoundsRight = camera->m_BoundsRight;
        m_BoundsBottom = camera->m_BoundsBottom;
        m_Target = camera->m_Target;
        m_FollowSpeed = camera->m_FollowSpeed;
        m_FollowOffset = camera->m_FollowOffset;
        // 흔들림 정보는 복사하지 않음
    }
}

void Camera::Update(float deltaTime)
{
    UpdateTargetFollow(deltaTime);
    UpdateShake(deltaTime);
    ApplyBounds();
}

void Camera::Render(HDC hdc)
{
}

void Camera::SetPosition(float x, float y)
{
    m_Position.x = x;
    m_Position.y = y;
}

void Camera::SetPosition(const Vector2& position)
{
    m_Position = position;
}

Vector2 Camera::GetPosition() const
{
    return m_Position + m_ShakeOffset;
}

void Camera::SetRotation(float rotation)
{
    m_Rotation = rotation;
}

float Camera::GetRotation() const
{
    return m_Rotation;
}

void Camera::SetZoom(float zoom)
{
    m_Zoom = max(0.1f, zoom); // 최소 줌 제한
}

float Camera::GetZoom() const
{
    return m_Zoom;
}

void Camera::Move(float deltaX, float deltaY)
{
    m_Position.x += deltaX;
    m_Position.y += deltaY;
}

void Camera::Move(const Vector2& delta)
{
    m_Position.x += delta.x;
    m_Position.y += delta.y;
}

Vector2 Camera::WorldToScreen(const Vector2& worldPos) const
{
    Vector2 cameraPos = GetPosition();

    // 카메라 위치를 기준으로 상대 좌표 계산
    Vector2 relative = Vector2(worldPos.x - cameraPos.x, worldPos.y - cameraPos.y);

    // 줌 적용
    relative.x *= m_Zoom;
    relative.y *= m_Zoom;

    // 회전 적용 (필요시)
    if (m_Rotation != 0.0f)
    {
        float cos_r = cosf(m_Rotation);
        float sin_r = sinf(m_Rotation);
        float rotatedX = relative.x * cos_r - relative.y * sin_r;
        float rotatedY = relative.x * sin_r + relative.y * cos_r;
        relative.x = rotatedX;
        relative.y = rotatedY;
    }

    // 화면 중심으로 이동
    Vector2 screenPos;
    screenPos.x = relative.x + m_ViewportSize.x * 0.5f;
    screenPos.y = relative.y + m_ViewportSize.y * 0.5f;

    return screenPos;
}

Vector2 Camera::ScreenToWorld(const Vector2& screenPos) const
{
    Vector2 cameraPos = GetPosition();

    // 화면 중심 기준으로 상대 좌표 계산
    Vector2 relative;
    relative.x = screenPos.x - m_ViewportSize.x * 0.5f;
    relative.y = screenPos.y - m_ViewportSize.y * 0.5f;

    // 회전 역변환 (필요시)
    if (m_Rotation != 0.0f)
    {
        float cos_r = cosf(-m_Rotation);
        float sin_r = sinf(-m_Rotation);
        float rotatedX = relative.x * cos_r - relative.y * sin_r;
        float rotatedY = relative.x * sin_r + relative.y * cos_r;
        relative.x = rotatedX;
        relative.y = rotatedY;
    }

    // 줌 역변환
    relative.x /= m_Zoom;
    relative.y /= m_Zoom;

    // 월드 좌표로 변환
    Vector2 worldPos;
    worldPos.x = relative.x + cameraPos.x;
    worldPos.y = relative.y + cameraPos.y;

    return worldPos;
}

POINT Camera::WorldToScreen(float worldX, float worldY) const
{
    Vector2 screenPos = WorldToScreen(Vector2(worldX, worldY));
    POINT point;
    point.x = static_cast<int>(screenPos.x);
    point.y = static_cast<int>(screenPos.y);
    return point;
}

void Camera::ScreenToWorld(int screenX, int screenY, float& worldX, float& worldY) const
{
    Vector2 worldPos = ScreenToWorld(Vector2(static_cast<float>(screenX), static_cast<float>(screenY)));
    worldX = worldPos.x;
    worldY = worldPos.y;
}

void Camera::SetViewportSize(float width, float height)
{
    m_ViewportSize.x = width;
    m_ViewportSize.y = height;
}

Vector2 Camera::GetViewportSize() const
{
    return m_ViewportSize;
}

void Camera::SetBounds(float left, float top, float right, float bottom)
{
    m_HasBounds = true;
    m_BoundsLeft = left;
    m_BoundsTop = top;
    m_BoundsRight = right;
    m_BoundsBottom = bottom;
}

void Camera::ClearBounds()
{
    m_HasBounds = false;
}

bool Camera::HasBounds() const
{
    return m_HasBounds;
}

void Camera::SetTarget(Object* target)
{
    m_Target = target;
}

void Camera::SetFollowSpeed(float speed)
{
    m_FollowSpeed = speed;
}

void Camera::SetFollowOffset(const Vector2& offset)
{
    m_FollowOffset = offset;
}

void Camera::Shake(float intensity, float duration)
{
    m_IsShaking = true;
    m_ShakeIntensity = intensity;
    m_ShakeDuration = duration;
    m_ShakeTimer = 0.0f;
}

void Camera::StopShake()
{
    m_IsShaking = false;
    m_ShakeOffset = Vector2(0.0f, 0.0f);
}

void Camera::SetMainCamera(Camera* camera)
{
    s_MainCamera = camera;
}

Camera* Camera::GetMainCamera()
{
    return s_MainCamera;
}

void Camera::UpdateTargetFollow(float deltaTime)
{
    if (m_Target == nullptr)
        return;

    // 타겟의 Transform 컴포넌트에서 위치 가져오기 (Object 클래스에 GetTransform() 메서드가 있다고 가정)
    Vector2 targetPos = m_Target->GetTransform().position;
    targetPos.x += m_FollowOffset.x;
    targetPos.y += m_FollowOffset.y;

    // 부드럽게 따라가기
    Vector2 direction = Vector2(targetPos.x - m_Position.x, targetPos.y - m_Position.y);
    m_Position.x += direction.x * m_FollowSpeed * deltaTime;
    m_Position.y += direction.y * m_FollowSpeed * deltaTime;
}

void Camera::UpdateShake(float deltaTime)
{
    if (!m_IsShaking)
        return;

    m_ShakeTimer += deltaTime;

    if (m_ShakeTimer >= m_ShakeDuration)
    {
        StopShake();
        return;
    }

    // 랜덤한 흔들림 효과
    float intensity = m_ShakeIntensity * (1.0f - m_ShakeTimer / m_ShakeDuration);
    m_ShakeOffset.x = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * intensity;
    m_ShakeOffset.y = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * intensity;
}

void Camera::ApplyBounds()
{
    if (!m_HasBounds)
        return;

    // 뷰포트 크기의 절반
    float halfViewWidth = m_ViewportSize.x * 0.5f / m_Zoom;
    float halfViewHeight = m_ViewportSize.y * 0.5f / m_Zoom;

    // 경계 내에서 카메라 위치 제한
    if (m_Position.x - halfViewWidth < m_BoundsLeft)
        m_Position.x = m_BoundsLeft + halfViewWidth;
    if (m_Position.x + halfViewWidth > m_BoundsRight)
        m_Position.x = m_BoundsRight - halfViewWidth;
    if (m_Position.y - halfViewHeight < m_BoundsTop)
        m_Position.y = m_BoundsTop + halfViewHeight;
    if (m_Position.y + halfViewHeight > m_BoundsBottom)
        m_Position.y = m_BoundsBottom - halfViewHeight;
}