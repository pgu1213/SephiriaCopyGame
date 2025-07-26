#include <pch.h>  
#include "SpriteRenderer.h"  
#include <Engine/Managers/ResourceManager/ResourceManager.h>
#include <Engine/Object/Component/Camera/Camera.h>

SpriteRenderer::SpriteRenderer(Object* owner)
    : Component(owner)
    , m_pSprite(nullptr)
    , m_Position(Vector2::Zero)
    , m_Size(Vector2::Zero)
    , m_Scale(Vector2::One)
    , m_Rotation(0.0f)
    , m_Anchor(Vector2(0.5f, 0.5f))
    , m_Opacity(1.0f)
    , m_IsVisible(true)
    , m_FlipX(false)
    , m_FlipY(false)
    , m_SourceRect({ 0, 0, 0, 0 })
    , m_UseSourceRect(false)
    , m_RenderLayer(0)
    , m_Depth(0.0f)
    , m_FrameCount(1)
    , m_CurrentFrame(0)
    , m_FrameWidth(0)
    , m_FrameHeight(0)
    , m_FrameTime(0.1f)
    , m_AnimationTimer(0.0f)
    , m_IsLooping(true)
    , m_TransformDirty(true)
{
}

// 복사 생성자  
SpriteRenderer::SpriteRenderer(const SpriteRenderer& other)  
    : Component(other.m_pObjOwner)  
    , m_pSprite(other.m_pSprite)
    , m_Position(Vector2::Zero)
    , m_Size(other.m_Size)
    , m_Scale(Vector2::One)
    , m_Rotation(0.0f)
    , m_Anchor(other.m_Anchor)
    , m_Opacity(1.0f)
    , m_IsVisible(true)
    , m_FlipX(other.m_FlipX)
    , m_FlipY(other.m_FlipY)
    , m_SourceRect({ 0, 0, 0, 0 })
    , m_UseSourceRect(false)
    , m_RenderLayer(0)
    , m_Depth(0.0f)
    , m_FrameCount(1)
    , m_CurrentFrame(0)
    , m_FrameWidth(0)
    , m_FrameHeight(0)
    , m_FrameTime(0.1f)
    , m_AnimationTimer(0.0f)
    , m_IsLooping(true)
    , m_TransformDirty(true)
{  
}  

Component* SpriteRenderer::CloneImpl() const  
{  
    return new SpriteRenderer(*this); 
}  

void SpriteRenderer::CopyFrom(const IPrototypeable* source)  
{  
    Component::CopyFrom(source);  

    const SpriteRenderer* sourceSprite = dynamic_cast<const SpriteRenderer*>(source);  
    if (sourceSprite)  
    {  
        m_pSprite = sourceSprite->m_pSprite;
        m_SpriteName = sourceSprite->m_SpriteName;
        m_Position = sourceSprite->m_Position;
        m_Size = sourceSprite->m_Size;
		m_Scale = sourceSprite->m_Scale;
		m_Rotation = sourceSprite->m_Rotation;
		m_Anchor = sourceSprite->m_Anchor;
		m_Opacity = sourceSprite->m_Opacity;
		m_IsVisible = sourceSprite->m_IsVisible;
		m_FlipX = sourceSprite->m_FlipX;
		m_FlipY = sourceSprite->m_FlipY;
		m_SourceRect = sourceSprite->m_SourceRect;
		m_UseSourceRect = sourceSprite->m_UseSourceRect;
		m_RenderLayer = sourceSprite->m_RenderLayer;
		m_Depth = sourceSprite->m_Depth;
		m_FrameCount = sourceSprite->m_FrameCount;
        m_CurrentFrame = 0;
		m_FrameWidth = sourceSprite->m_FrameWidth;
		m_FrameHeight = sourceSprite->m_FrameHeight;
		m_FrameTime = sourceSprite->m_FrameTime;
        m_AnimationTimer = 0.0f;
		m_IsLooping = sourceSprite->m_IsLooping;
		m_TransformDirty = sourceSprite->m_TransformDirty;
    }  
}

void SpriteRenderer::Init()
{
    Component::Init();
    m_TransformDirty = true;
}

void SpriteRenderer::Update(float deltaTime)
{
    Component::Update(deltaTime);

    if (m_TransformDirty)
    {
        CalculateTransform();
        m_TransformDirty = false;
    }
}


void SpriteRenderer::Render(HDC hdc)
{
    if (!m_IsVisible || !m_pSprite || m_Opacity <= 0.0f)
        return;

    // 카메라 기반 컬링 (성능 최적화)
    Camera* mainCamera = Camera::GetMainCamera();
    if (mainCamera && ShouldCull(mainCamera))
    {
        return; // 화면 밖에 있으므로 렌더링하지 않음
    }

    RenderSprite(hdc);
}

void SpriteRenderer::OnDestroy()
{
    m_pSprite = nullptr;
}

void SpriteRenderer::SetSprite(const wstring& spriteName)
{
    m_SpriteName = spriteName;
    m_pSprite = ResourceManager::GetInstance()->GetSprite(spriteName);

    if (m_pSprite && m_Size == Vector2::Zero)
    {
        // Auto-set size to sprite dimensions if not set
        m_Size.x = static_cast<float>(m_pSprite->GetWidth());
        m_Size.y = static_cast<float>(m_pSprite->GetHeight());
    }

    m_TransformDirty = true;
}

void SpriteRenderer::SetSprite(Gdiplus::Bitmap* bitmap)
{
    m_pSprite = bitmap;
    m_SpriteName.clear();

    if (m_pSprite && m_Size == Vector2::Zero)
    {
        m_Size.x = static_cast<float>(m_pSprite->GetWidth());
        m_Size.y = static_cast<float>(m_pSprite->GetHeight());
    }

    m_TransformDirty = true;
}

void SpriteRenderer::CalculateTransform()
{
    m_TransformMatrix.Reset();

    Vector2 anchoredPos = CalculateAnchoredPosition();

    // Apply transformations in order: Scale -> Rotate -> Translate
    m_TransformMatrix.Scale(m_Scale.x * (m_FlipX ? -1.0f : 1.0f),
        m_Scale.y * (m_FlipY ? -1.0f : 1.0f));

    if (m_Rotation != 0.0f)
    {
        m_TransformMatrix.Rotate(RadianToDegree(m_Rotation));
    }

    m_TransformMatrix.Translate(anchoredPos.x, anchoredPos.y);
}

void SpriteRenderer::RenderSprite(HDC hdc)
{
    Gdiplus::Graphics graphics(hdc);

    // Set rendering quality
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    ApplyTransformations(graphics);

    Gdiplus::ImageAttributes imageAttr;
    if (m_Opacity < 1.0f)
    {
        Gdiplus::ColorMatrix colorMatrix = {
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, m_Opacity, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f
        };
        imageAttr.SetColorMatrix(&colorMatrix);
    }

    // Calculate source and destination rectangles
    RECT srcRect = m_UseSourceRect ? m_SourceRect : CalculateCurrentFrameRect();

    Gdiplus::Rect destRect(
        static_cast<int>(-m_Size.x * m_Anchor.x),
        static_cast<int>(-m_Size.y * m_Anchor.y),
        static_cast<int>(m_Size.x),
        static_cast<int>(m_Size.y)
    );

    // Draw the sprite
    if (m_Opacity < 1.0f)
    {
        graphics.DrawImage(m_pSprite, destRect,
            srcRect.left, srcRect.top,
            srcRect.right - srcRect.left, srcRect.bottom - srcRect.top,
            Gdiplus::UnitPixel, &imageAttr);
    }
    else
    {
        graphics.DrawImage(m_pSprite, destRect,
            srcRect.left, srcRect.top,
            srcRect.right - srcRect.left, srcRect.bottom - srcRect.top,
            Gdiplus::UnitPixel);
    }
}

bool SpriteRenderer::ShouldCull(Camera* camera) const
{
    if (!m_pObjOwner || !camera) return false;

    // Object의 월드 위치 계산
    const Transform& transform = m_pObjOwner->GetTransform();
    Vector2 worldPos = Vector2(
        transform.position.x + m_Position.x,
        transform.position.y + m_Position.y
    );

    // 화면 좌표로 변환
    Vector2 screenPos = camera->WorldToScreen(worldPos);

    // 카메라 줌 적용된 크기 계산
    float zoom = camera->GetZoom();
    float scaledWidth = m_Size.x * m_Scale.x * zoom;
    float scaledHeight = m_Size.y * m_Scale.y * zoom;

    // 앵커 적용
    float left = screenPos.x - scaledWidth * m_Anchor.x;
    float right = left + scaledWidth;
    float top = screenPos.y - scaledHeight * m_Anchor.y;
    float bottom = top + scaledHeight;

    // 뷰포트 크기
    Vector2 viewport = camera->GetViewportSize();

    // 여백을 두고 컬링 (완전히 화면 밖에 있을 때만)
    float margin = 50.0f;
    return (right < -margin || left > viewport.x + margin ||
        bottom < -margin || top > viewport.y + margin);
}

void SpriteRenderer::ApplyTransformations(Gdiplus::Graphics& graphics)
{
    if (!m_pObjOwner) return;

    // 메인 카메라 가져오기
    Camera* mainCamera = Camera::GetMainCamera();
    if (!mainCamera)
    {
        // 카메라가 없으면 기본 변환만 적용 (디버그용)
        ApplyTransformationsWithoutCamera(graphics);
        return;
    }

    // Object의 Transform 정보 가져오기
    const Transform& transform = m_pObjOwner->GetTransform();

    // 1. 월드 좌표 계산 (Object Transform + SpriteRenderer Position)
    Vector2 worldPosition = Vector2(
        transform.position.x + m_Position.x,
        transform.position.y + m_Position.y
    );

    // 2. 카메라를 통해 월드 좌표를 화면 좌표로 변환
    Vector2 screenPosition = mainCamera->WorldToScreen(worldPosition);

    // 3. 변환 행렬 생성 및 적용
    Gdiplus::Matrix matrix;

    // 화면 좌표로 이동
    matrix.Translate(screenPosition.x, screenPosition.y);

    // Object 회전 적용
    if (transform.rotation != 0.0f)
    {
        matrix.Rotate(RadianToDegree(transform.rotation));
    }

    // SpriteRenderer 회전 적용
    if (m_Rotation != 0.0f)
    {
        matrix.Rotate(RadianToDegree(m_Rotation));
    }

    // 카메라 줌 + Object 스케일 + SpriteRenderer 스케일 적용
    float cameraZoom = mainCamera->GetZoom();
    float finalScaleX = transform.scale.x * m_Scale.x * cameraZoom;
    float finalScaleY = transform.scale.y * m_Scale.y * cameraZoom;

    // FlipX/Y 처리
    if (m_FlipX) finalScaleX *= -1.0f;
    if (m_FlipY) finalScaleY *= -1.0f;

    matrix.Scale(finalScaleX, finalScaleY);

    // Graphics에 변환 행렬 적용
    graphics.SetTransform(&matrix);
}

void SpriteRenderer::ApplyTransformationsWithoutCamera(Gdiplus::Graphics& graphics)
{
    if (!m_pObjOwner) return;

    const Transform& transform = m_pObjOwner->GetTransform();
    Gdiplus::Matrix matrix;

    // 기본 위치 적용 (Transform + SpriteRenderer Position)
    matrix.Translate(
        transform.position.x + m_Position.x,
        transform.position.y + m_Position.y
    );

    // 회전 적용
    if (transform.rotation != 0.0f)
    {
        matrix.Rotate(RadianToDegree(transform.rotation));
    }
    if (m_Rotation != 0.0f)
    {
        matrix.Rotate(RadianToDegree(m_Rotation));
    }

    // 스케일 적용
    float scaleX = transform.scale.x * m_Scale.x * (m_FlipX ? -1.0f : 1.0f);
    float scaleY = transform.scale.y * m_Scale.y * (m_FlipY ? -1.0f : 1.0f);
    matrix.Scale(scaleX, scaleY);

    graphics.SetTransform(&matrix);
}

Vector2 SpriteRenderer::CalculateAnchoredPosition() const
{
    return m_Position;
}

RECT SpriteRenderer::CalculateCurrentFrameRect() const
{
    if (!m_pSprite || m_FrameCount <= 1)
    {
        return { 0, 0, static_cast<int>(m_pSprite ? m_pSprite->GetWidth() : 0),
                      static_cast<int>(m_pSprite ? m_pSprite->GetHeight() : 0) };
    }

    int frameWidth = m_FrameWidth > 0 ? m_FrameWidth : static_cast<int>(m_pSprite->GetWidth() / m_FrameCount);
    int frameHeight = m_FrameHeight > 0 ? m_FrameHeight : static_cast<int>(m_pSprite->GetHeight());

    int frameX = m_CurrentFrame * frameWidth;
    int frameY = 0;

    return { frameX, frameY, frameX + frameWidth, frameY + frameHeight };
}

void SpriteRenderer::SetSpriteByName(const wstring& spriteName)
{
    m_pSprite = ResourceManager::GetInstance()->GetSprite(spriteName);
}

Vector2 SpriteRenderer::GetWorldPosition() const
{
    if (GetObjOwner())
    {

    }
    return m_Position;
}

Vector2 SpriteRenderer::GetScreenPosition() const
{
    return GetWorldPosition();
}

RECT SpriteRenderer::GetBoundingRect() const
{
    Vector2 worldPos = GetWorldPosition();
    Vector2 anchoredPos = Vector2(
        worldPos.x - m_Size.x * m_Anchor.x * m_Scale.x,
        worldPos.y - m_Size.y * m_Anchor.y * m_Scale.y
    );

    return {
        static_cast<int>(anchoredPos.x),
        static_cast<int>(anchoredPos.y),
        static_cast<int>(anchoredPos.x + m_Size.x * m_Scale.x),
        static_cast<int>(anchoredPos.y + m_Size.y * m_Scale.y)
    };
}

bool SpriteRenderer::IsPointInside(const Vector2& point) const
{
    RECT bounds = GetBoundingRect();
    return point.x >= bounds.left && point.x <= bounds.right &&
        point.y >= bounds.top && point.y <= bounds.bottom;
}