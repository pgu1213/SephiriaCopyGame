#pragma once
#include <Engine/Object/Component/Component.h>

class SpriteRenderer : public Component
{
public:
    SpriteRenderer(Object* owner);
    SpriteRenderer(const SpriteRenderer& other); // 복사 생성자

protected:
    Component* CloneImpl() const override;

public:
    void CopyFrom(const IPrototypeable* source) override;
private:
    Bitmap* m_Texture;
    Vector2 m_Size;
    Vector2 m_Pivot;
    Color m_tint;
    int m_SortingOrder;
    bool m_FlipX;
    bool m_FlipY;
};