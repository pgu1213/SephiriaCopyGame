#pragma once
#include <Engine/Object/Component/Component.h>

class UIImage : public Component
{
private:
    wstring m_SpriteName;
    COLORREF m_TintColor;
    bool m_PreserveAspect;
    Vector2 m_UVOffset;
    Vector2 m_UVScale;

public:
    explicit UIImage(UI* owner);
    UIImage(const UIImage& other);
protected:
    Component* CloneImpl() const override;
public:
    void CopyFrom(const IPrototypeable* source) override;
public:
    void Init() override;
    void Render(HDC hdc) override;
public:
    // 스프라이트 설정
    void SetSprite(const wstring& spriteName) { m_SpriteName = spriteName; }
    const wstring& GetSprite() const { return m_SpriteName; }

    void SetTintColor(COLORREF color) { m_TintColor = color; }
    COLORREF GetTintColor() const { return m_TintColor; }

    // UV 설정
    void SetUV(const Vector2& offset, const Vector2& scale);
    void SetPreserveAspect(bool preserve) { m_PreserveAspect = preserve; }
    bool GetPreserveAspect() const { return m_PreserveAspect; }
};