#pragma once
#include <Engine/Object/Component/Component.h>

class Camera;

class SpriteRenderer : public Component
{
public:
    SpriteRenderer(Object* owner);
    SpriteRenderer(const SpriteRenderer& other); // 복사 생성자
protected:
    Component* CloneImpl() const override;
public:
    void CopyFrom(const IPrototypeable* source) override;
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Render(HDC hdc) override;
    void OnDestroy() override;
public:
    // 모든 변수에 대해 수정할 수 있게 제공해야할듯?
    void SetSprite(const wstring& spriteName);
    void SetSprite(Gdiplus::Bitmap* bitmap);
    Gdiplus::Bitmap* GetSprite() const { return m_pSprite; }

    void SetPosition(const Vector2& position) { m_Position = position; }
    void SetPosition(float x, float y) { m_Position = Vector2(x, y); }
    Vector2 GetPosition() const { return m_Position; }

    void SetSize(const Vector2& size) { m_Size = size; }
    void SetSize(float width, float height) { m_Size = Vector2(width, height); }
    Vector2 GetSize() const { return m_Size; }

    void SetScale(const Vector2& scale) { m_Scale = scale; }
    void SetScale(float scaleX, float scaleY) { m_Scale = Vector2(scaleX, scaleY); }
    void SetScale(float uniformScale) { m_Scale = Vector2(uniformScale, uniformScale); }
    Vector2 GetScale() const { return m_Scale; }

    void SetRotation(float rotation) { m_Rotation = rotation; }
    float GetRotation() const { return m_Rotation; }

    void SetOpacity(float opacity) { m_Opacity = max(0.0f, min(1.0f, opacity)); }
    float GetOpacity() const { return m_Opacity; }

    void SetVisible(bool visible) { m_IsVisible = visible; }
    bool IsVisible() const { return m_IsVisible; }

    void SetFlipX(bool flipX) { m_FlipX = flipX; }
    void SetFlipY(bool flipY) { m_FlipY = flipY; }
    bool GetFlipX() const { return m_FlipX; }
    bool GetFlipY() const { return m_FlipY; }

    void SetAnchor(const Vector2& anchor) { m_Anchor = anchor; }
    void SetAnchor(float x, float y) { m_Anchor = Vector2(x, y); }
    Vector2 GetAnchor() const { return m_Anchor; }

    void SetSourceRect(const RECT& sourceRect) { m_SourceRect = sourceRect; m_UseSourceRect = true; }
    void SetSourceRect(int x, int y, int width, int height) {
        m_SourceRect = { x, y, x + width, y + height };
        m_UseSourceRect = true;
    }
    void ClearSourceRect() { m_UseSourceRect = false; }
    RECT GetSourceRect() const { return m_SourceRect; }
    bool IsUsingSourceRect() const { return m_UseSourceRect; }

    void SetRenderLayer(int layer) { m_RenderLayer = layer; }
    int GetRenderLayer() const { return m_RenderLayer; }

    void SetDepth(float depth) { m_Depth = depth; }
    float GetDepth() const { return m_Depth; }

    // 소스 영역 설정 (스프라이트 시트용)
    void SetUseSourceRect(bool use) { m_UseSourceRect = use; }

    // 스프라이트 이름으로 설정 (ResourceManager 사용)
    void SetSpriteByName(const wstring& spriteName);

    // Frame-based animation
    void SetFrameCount(int frameCount) { m_FrameCount = frameCount; }
    void SetCurrentFrame(int frame) { m_CurrentFrame = max(0, min(frame, m_FrameCount - 1)); }
    void SetFrameSize(int width, int height) { m_FrameWidth = width; m_FrameHeight = height; }
    void SetFrameTime(float frameTime) { m_FrameTime = frameTime; }
    void SetLooping(bool loop) { m_IsLooping = loop; }

    int GetCurrentFrame() const { return m_CurrentFrame; }
    int GetFrameCount() const { return m_FrameCount; }
    bool IsLooping() const { return m_IsLooping; }

    Vector2 GetWorldPosition() const;
    Vector2 GetScreenPosition() const;
    RECT GetBoundingRect() const;
    bool IsPointInside(const Vector2& point) const;

    void SetBlendMode(int blendMode) { m_BlendMode = blendMode; }
    int GetBlendMode() const { return m_BlendMode; }

    void SetSortingOrder(int order) { m_SortingOrder = order; }
    int GetSortingOrder() const { return m_SortingOrder; }

private:
    void CalculateTransform();
    void RenderSprite(HDC hdc);
    bool ShouldCull(Camera* camera) const;
    void ApplyTransformations(Gdiplus::Graphics& graphics);
    void ApplyTransformationsWithoutCamera(Gdiplus::Graphics& graphics); // 카메라 없을 시

    Vector2 CalculateAnchoredPosition() const;
    RECT CalculateCurrentFrameRect() const;
private:
    Gdiplus::Bitmap* m_pSprite;
    wstring m_SpriteName;

    Vector2 m_Position;
    Vector2 m_Size;
    Vector2 m_Scale;
    float m_Rotation;
    Vector2 m_Anchor;

    float m_Opacity;
    bool m_IsVisible;
    bool m_FlipX;
    bool m_FlipY;

    RECT m_SourceRect;
    bool m_UseSourceRect;

    int m_RenderLayer;
    float m_Depth;
    int m_SortingOrder;
    int m_BlendMode;

    int m_FrameCount;
    int m_CurrentFrame;
    int m_FrameWidth;
    int m_FrameHeight;
    float m_FrameTime;
    float m_AnimationTimer;
    bool m_IsLooping;

    bool m_TransformDirty;
    Gdiplus::Matrix m_TransformMatrix;
};