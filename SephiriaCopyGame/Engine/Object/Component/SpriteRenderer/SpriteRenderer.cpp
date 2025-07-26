#include <pch.h>  
#include "SpriteRenderer.h"  
#include <Engine/Managers/ResourceManager/ResourceManager.h>

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
    , m_SortingOrder(0)
    , m_BlendMode(0)
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
    , m_SortingOrder(other.m_SortingOrder)
    , m_BlendMode(0)
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
		m_BlendMode = sourceSprite->m_BlendMode;
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

void SpriteRenderer::ApplyTransformations(Gdiplus::Graphics& graphics)
{
    if (!m_pObjOwner) return;

    // Object의 Transform 정보 가져오기
    const Transform& transform = m_pObjOwner->GetTransform();

    // 변환 행렬 생성
    Gdiplus::Matrix matrix;

    // 1. 이동 (Translation) - Object의 실제 위치 적용
    matrix.Translate(transform.position.x, transform.position.y);

    // 2. 회전 (Rotation) - 회전 중심점을 고려
    if (transform.rotation != 0.0f)
    {
        matrix.Rotate(transform.rotation);
    }

    // 3. 스케일 (Scale)
    if (transform.scale.x != 1.0f || transform.scale.y != 1.0f)
    {
        matrix.Scale(transform.scale.x, transform.scale.y);
    }

    // 4. FlipX 처리
    if (m_FlipX)
    {
        matrix.Scale(-1.0f, 1.0f);
    }

    // 5. FlipY 처리 (필요한 경우)
    if (m_FlipY)
    {
        matrix.Scale(1.0f, -1.0f);
    }

    // Graphics에 변환 행렬 적용
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