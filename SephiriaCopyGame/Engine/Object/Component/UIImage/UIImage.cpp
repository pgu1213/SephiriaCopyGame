#include "pch.h"
#include "UIImage.h"
#include <Engine/Object/UI/UI.h>
#include <Engine/Managers/ResourceManager/ResourceManager.h>

UIImage::UIImage(UI* owner)
    : Component(owner)
    , m_TintColor(RGB(255, 255, 255))
    , m_PreserveAspect(true)
    , m_UVOffset(Vector2::Zero)
    , m_UVScale(Vector2::One)
{
}

UIImage::UIImage(const UIImage& other)
    : Component(other)
    , m_SpriteName(other.m_SpriteName)
    , m_TintColor(other.m_TintColor)
    , m_PreserveAspect(other.m_PreserveAspect)
    , m_UVOffset(other.m_UVOffset)
    , m_UVScale(other.m_UVScale)
{
}

Component* UIImage::CloneImpl() const
{
    return new UIImage(*this);
}

void UIImage::CopyFrom(const IPrototypeable* source)
{
    Component::CopyFrom(source);

    const UIImage* sourceImage = dynamic_cast<const UIImage*>(source);
    if (sourceImage)
    {
        m_SpriteName = sourceImage->m_SpriteName;
        m_TintColor = sourceImage->m_TintColor;
        m_PreserveAspect = sourceImage->m_PreserveAspect;
        m_UVOffset = sourceImage->m_UVOffset;
		m_UVScale = sourceImage->m_UVScale;
    }
}

void UIImage::Init()
{
    Component::Init();
}

void UIImage::Render(HDC hdc)
{
    if (m_SpriteName.empty()) return;

    UI* ui = static_cast<UI*>(GetUIOwner());
    if (!ui) return;

    // 스프라이트 가져오기
    Bitmap* sprite = ResourceManager::GetInstance()->GetSprite(m_SpriteName);
    if (!sprite) return;

    Vector2 worldPos = ui->GetWorldPosition();
    Vector2 size = ui->GetSize();

    Graphics graphics(hdc);

    // 렌더링 위치 계산
    float destX = worldPos.x - size.x * 0.5f;
    float destY = worldPos.y - size.y * 0.5f;

    // UV 좌표 계산
    UINT spriteWidth = sprite->GetWidth();
    UINT spriteHeight = sprite->GetHeight();

    float srcX = m_UVOffset.x * spriteWidth;
    float srcY = m_UVOffset.y * spriteHeight;
    float srcWidth = m_UVScale.x * spriteWidth;
    float srcHeight = m_UVScale.y * spriteHeight;

    // 이미지 그리기
    graphics.DrawImage(
        sprite,
        RectF(destX, destY, size.x, size.y),
        srcX, srcY, srcWidth, srcHeight,
        UnitPixel
    );
}

void UIImage::SetUV(const Vector2& offset, const Vector2& scale)
{
    m_UVOffset = offset;
    m_UVScale = scale;
}