#include <pch.h>  
#include "SpriteRenderer.h"  

SpriteRenderer::SpriteRenderer(Object* owner)  
    : Component(owner), m_Texture(nullptr), m_Size(1.0f, 1.0f),
    m_Pivot(0.5f, 0.5f), m_tint(255, 255, 255, 255),
    m_SortingOrder(0), m_FlipX(false), m_FlipY(false)
{  

}  

// 복사 생성자  
SpriteRenderer::SpriteRenderer(const SpriteRenderer& other)  
    : Component(other.m_pOwner)  
    , m_Texture(other.m_Texture)  
    , m_Size(other.m_Size)  
    , m_Pivot(other.m_Pivot)  
    , m_tint(other.m_tint)  
    , m_SortingOrder(other.m_SortingOrder)  
    , m_FlipX(other.m_FlipX)  
    , m_FlipY(other.m_FlipY)  
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
        m_Texture = sourceSprite->m_Texture;  
        m_Size = sourceSprite->m_Size;  
        m_Pivot = sourceSprite->m_Pivot;  
        m_tint = sourceSprite->m_tint;  
        m_SortingOrder = sourceSprite->m_SortingOrder;  
        m_FlipX = sourceSprite->m_FlipX;  
        m_FlipY = sourceSprite->m_FlipY;  
    }  
}