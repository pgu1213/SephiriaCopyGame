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
    , m_IsAnimationPlaying(false)
    , m_IsAnimationPaused(false)
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
    : Component(other.m_pOwner)  
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
    , m_IsAnimationPlaying(false)
    , m_IsAnimationPaused(false)
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
        m_IsAnimationPlaying = false;
		m_IsAnimationPaused = sourceSprite->m_IsAnimationPaused;
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

    if (m_IsAnimationPlaying && !m_IsAnimationPaused)
    {
        UpdateAnimation(deltaTime);
        UpdateFrameAnimation(deltaTime);
    }

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

void SpriteRenderer::UpdateAnimation(float deltaTime)
{
    // Custom animation logic can be added here
    // This is where you'd integrate with an Animation component or system
}

void SpriteRenderer::UpdateFrameAnimation(float deltaTime)
{
    if (m_FrameCount <= 1)
        return;

    m_AnimationTimer += deltaTime;

    if (m_AnimationTimer >= m_FrameTime)
    {
        m_AnimationTimer = 0.0f;
        m_CurrentFrame++;

        if (m_CurrentFrame >= m_FrameCount)
        {
            if (m_IsLooping)
            {
                m_CurrentFrame = 0;
            }
            else
            {
                m_CurrentFrame = m_FrameCount - 1;
                m_IsAnimationPlaying = false;
            }
        }
    }
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

    // Apply transformations
    ApplyTransformations(graphics);

    // Set opacity
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
    graphics.SetTransform(&m_TransformMatrix);
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

void SpriteRenderer::PlayAnimation(const wstring& animationName)
{
    m_CurrentAnimationName = animationName;
    m_IsAnimationPlaying = true;
    m_IsAnimationPaused = false;
    m_AnimationTimer = 0.0f;
    m_CurrentFrame = 0;
}

void SpriteRenderer::StopAnimation()
{
    m_IsAnimationPlaying = false;
    m_IsAnimationPaused = false;
    m_AnimationTimer = 0.0f;
    m_CurrentFrame = 0;
}

void SpriteRenderer::PauseAnimation()
{
    m_IsAnimationPaused = true;
}

void SpriteRenderer::ResumeAnimation()
{
    m_IsAnimationPaused = false;
}

Vector2 SpriteRenderer::GetWorldPosition() const
{
    // If attached to an object, get its world position
    if (GetOwner())
    {
        // Assuming Object has GetPosition method
        // return GetOwner()->GetPosition() + m_Position;
    }
    return m_Position;
}

Vector2 SpriteRenderer::GetScreenPosition() const
{
    // Convert world position to screen position using camera
    // This would need camera integration
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