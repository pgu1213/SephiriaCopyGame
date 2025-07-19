#include <pch.h>
#include "UI.h"

Vector2 UI::s_ScreenSize = Vector2(static_cast<float>(WINCX), static_cast<float>(WINCY));

UI::UI()
    : m_Position(0.0f, 0.0f)
    , m_Size(100.0f, 50.0f)
    , m_AnchorOffset(0.0f, 0.0f)
    , m_Anchor(UIAnchor::TopLeft)
    , m_Visible(true)
    , m_Enabled(true)
    , m_Alpha(1.0f)
    , m_Color(RGB(255, 255, 255))
    , m_ZOrder(0)
    , m_IsHovered(false)
    , m_IsPressed(false)
    , m_Parent(nullptr)
{
}

UI::UI(const UI& other)
    : m_Position(other.m_Position)
    , m_Size(other.m_Size)
    , m_AnchorOffset(other.m_AnchorOffset)
    , m_Anchor(other.m_Anchor)
    , m_Visible(other.m_Visible)
    , m_Enabled(other.m_Enabled)
    , m_Alpha(other.m_Alpha)
    , m_Color(other.m_Color)
    , m_ZOrder(other.m_ZOrder)
    , m_IsHovered(false)
    , m_IsPressed(false)
    , m_ClickCallback(other.m_ClickCallback)
    , m_HoverCallback(other.m_HoverCallback)
    , m_Parent(nullptr)
{
    // 자식들은 복사하지 않음 (별도로 관리)
}

UI::~UI()
{
    ClearChildren();
    if (m_Parent)
    {
        m_Parent->RemoveChild(shared_ptr<UI>(this));
    }
}

unique_ptr<IPrototypeable> UI::Clone() const
{
    return make_unique<UI>(*this);
}

void UI::CopyFrom(const IPrototypeable* source)
{
    const UI* ui = dynamic_cast<const UI*>(source);
    if (ui)
    {
        m_Position = ui->m_Position;
        m_Size = ui->m_Size;
        m_AnchorOffset = ui->m_AnchorOffset;
        m_Anchor = ui->m_Anchor;
        m_Visible = ui->m_Visible;
        m_Enabled = ui->m_Enabled;
        m_Alpha = ui->m_Alpha;
        m_Color = ui->m_Color;
        m_ZOrder = ui->m_ZOrder;
        m_ClickCallback = ui->m_ClickCallback;
        m_HoverCallback = ui->m_HoverCallback;
    }
}

void UI::Update(float deltaTime)
{
    if (!m_Visible || !m_Enabled)
        return;

    // 자식 UI들 업데이트
    for (auto& child : m_Children)
    {
        if (child)
        {
            child->Update(deltaTime);
        }
    }
}

void UI::Render(HDC hdc)
{
    if (!m_Visible)
        return;

    // 기본 UI는 단순한 사각형 렌더링
    Vector2 screenPos = GetScreenPosition();
    Vector2 screenSize = GetActualSize();

    // 알파 블렌딩을 위한 브러시 생성
    HBRUSH brush = CreateSolidBrush(m_Color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    // 사각형 그리기
    Rectangle(hdc,
        static_cast<int>(screenPos.x),
        static_cast<int>(screenPos.y),
        static_cast<int>(screenPos.x + screenSize.x),
        static_cast<int>(screenPos.y + screenSize.y));

    SelectObject(hdc, oldBrush);
    DeleteObject(brush);

    // 자식 UI들 렌더링 (Z-Order 순으로 정렬해서 렌더링)
    vector<shared_ptr<UI>> sortedChildren = m_Children;
    sort(sortedChildren.begin(), sortedChildren.end(),
        [](const shared_ptr<UI>& a, const shared_ptr<UI>& b) {
            return a->GetZOrder() < b->GetZOrder();
        });

    for (auto& child : sortedChildren)
    {
        if (child)
        {
            child->Render(hdc);
        }
    }
}

bool UI::OnMouseMove(int x, int y)
{
    if (!m_Visible || !m_Enabled)
        return false;

    bool wasHovered = m_IsHovered;
    m_IsHovered = IsPointInside(x, y);

    // 호버 이벤트 처리
    if (m_IsHovered && !wasHovered)
    {
        OnHover();
    }

    // 자식 UI들에게 마우스 이벤트 전달
    for (auto& child : m_Children)
    {
        if (child && child->OnMouseMove(x, y))
        {
            return true; // 자식이 처리했으면 이벤트 소비
        }
    }

    return m_IsHovered;
}

bool UI::OnMouseDown(int x, int y)
{
    if (!m_Visible || !m_Enabled)
        return false;

    // 자식 UI들에게 먼저 이벤트 전달
    for (auto& child : m_Children)
    {
        if (child && child->OnMouseDown(x, y))
        {
            return true; // 자식이 처리했으면 이벤트 소비
        }
    }

    if (IsPointInside(x, y))
    {
        m_IsPressed = true;
        OnPress();
        return true;
    }

    return false;
}

bool UI::OnMouseUp(int x, int y)
{
    if (!m_Visible || !m_Enabled)
        return false;

    // 자식 UI들에게 먼저 이벤트 전달
    for (auto& child : m_Children)
    {
        if (child && child->OnMouseUp(x, y))
        {
            return true; // 자식이 처리했으면 이벤트 소비
        }
    }

    bool wasPressed = m_IsPressed;
    m_IsPressed = false;

    if (IsPointInside(x, y))
    {
        OnRelease();

        // 클릭 이벤트 (눌렸다가 같은 UI에서 놓였을 때)
        if (wasPressed)
        {
            OnClick();
        }
        return true;
    }

    return false;
}

void UI::SetPosition(float x, float y)
{
    m_Position.x = x;
    m_Position.y = y;
}

void UI::SetPosition(const Vector2& position)
{
    m_Position = position;
}

Vector2 UI::GetPosition() const
{
    return m_Position;
}

void UI::SetSize(float width, float height)
{
    m_Size.x = width;
    m_Size.y = height;
}

void UI::SetSize(const Vector2& size)
{
    m_Size = size;
}

Vector2 UI::GetSize() const
{
    return m_Size;
}

void UI::SetRect(float x, float y, float width, float height)
{
    SetPosition(x, y);
    SetSize(width, height);
}

void UI::SetRect(const Vector2& position, const Vector2& size)
{
    SetPosition(position);
    SetSize(size);
}

void UI::SetAnchor(UIAnchor anchor)
{
    m_Anchor = anchor;
}

UIAnchor UI::GetAnchor() const
{
    return m_Anchor;
}

void UI::SetAnchorOffset(float x, float y)
{
    m_AnchorOffset.x = x;
    m_AnchorOffset.y = y;
}

void UI::SetAnchorOffset(const Vector2& offset)
{
    m_AnchorOffset = offset;
}

Vector2 UI::GetAnchorOffset() const
{
    return m_AnchorOffset;
}

Vector2 UI::GetScreenPosition() const
{
    Vector2 anchorPos = CalculateAnchorPosition();
    return Vector2(anchorPos.x + m_Position.x + m_AnchorOffset.x,
        anchorPos.y + m_Position.y + m_AnchorOffset.y);
}

Vector2 UI::GetActualSize() const
{
    return m_Size;
}

void UI::SetVisible(bool visible)
{
    m_Visible = visible;
}

bool UI::IsVisible() const
{
    return m_Visible;
}

void UI::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}

bool UI::IsEnabled() const
{
    return m_Enabled;
}

void UI::SetAlpha(float alpha)
{
    m_Alpha = max(0.0f, min(1.0f, alpha));
}

float UI::GetAlpha() const
{
    return m_Alpha;
}

void UI::SetColor(COLORREF color)
{
    m_Color = color;
}

COLORREF UI::GetColor() const
{
    return m_Color;
}

void UI::SetZOrder(int zOrder)
{
    m_ZOrder = zOrder;
}

int UI::GetZOrder() const
{
    return m_ZOrder;
}

bool UI::IsPointInside(int x, int y) const
{
    Vector2 screenPos = GetScreenPosition();
    Vector2 screenSize = GetActualSize();

    return (x >= screenPos.x && x <= screenPos.x + screenSize.x &&
        y >= screenPos.y && y <= screenPos.y + screenSize.y);
}

void UI::SetClickCallback(UIEventCallback callback)
{
    m_ClickCallback = callback;
}

void UI::SetHoverCallback(UIEventCallback callback)
{
    m_HoverCallback = callback;
}

void UI::AddChild(shared_ptr<UI> child)
{
    if (child)
    {
        child->SetParent(this);
        m_Children.push_back(child);
    }
}

void UI::RemoveChild(shared_ptr<UI> child)
{
    auto it = find(m_Children.begin(), m_Children.end(), child);
    if (it != m_Children.end())
    {
        (*it)->SetParent(nullptr);
        m_Children.erase(it);
    }
}

void UI::ClearChildren()
{
    for (auto& child : m_Children)
    {
        if (child)
        {
            child->SetParent(nullptr);
        }
    }
    m_Children.clear();
}

const vector<shared_ptr<UI>>& UI::GetChildren() const
{
    return m_Children;
}

void UI::SetParent(UI* parent)
{
    m_Parent = parent;
}

UI* UI::GetParent() const
{
    return m_Parent;
}

void UI::SetScreenSize(float width, float height)
{
    s_ScreenSize.x = width;
    s_ScreenSize.y = height;
}

Vector2 UI::GetGlobalScreenSize()
{
    return s_ScreenSize;
}

Vector2 UI::CalculateAnchorPosition() const
{
    Vector2 anchorPos(0.0f, 0.0f);

    switch (m_Anchor)
    {
    case UIAnchor::TopLeft:
        anchorPos = Vector2(0.0f, 0.0f);
        break;
    case UIAnchor::TopCenter:
        anchorPos = Vector2(s_ScreenSize.x * 0.5f, 0.0f);
        break;
    case UIAnchor::TopRight:
        anchorPos = Vector2(s_ScreenSize.x, 0.0f);
        break;
    case UIAnchor::CenterLeft:
        anchorPos = Vector2(0.0f, s_ScreenSize.y * 0.5f);
        break;
    case UIAnchor::Center:
        anchorPos = Vector2(s_ScreenSize.x * 0.5f, s_ScreenSize.y * 0.5f);
        break;
    case UIAnchor::CenterRight:
        anchorPos = Vector2(s_ScreenSize.x, s_ScreenSize.y * 0.5f);
        break;
    case UIAnchor::BottomLeft:
        anchorPos = Vector2(0.0f, s_ScreenSize.y);
        break;
    case UIAnchor::BottomCenter:
        anchorPos = Vector2(s_ScreenSize.x * 0.5f, s_ScreenSize.y);
        break;
    case UIAnchor::BottomRight:
        anchorPos = Vector2(s_ScreenSize.x, s_ScreenSize.y);
        break;
    }

    return anchorPos;
}

void UI::OnClick()
{
    if (m_ClickCallback)
    {
        m_ClickCallback();
    }
}

void UI::OnHover()
{
    if (m_HoverCallback)
    {
        m_HoverCallback();
    }
}

void UI::OnPress()
{
    // 기본 구현은 비어있음 - 상속받은 클래스에서 구현
}

void UI::OnRelease()
{
    // 기본 구현은 비어있음 - 상속받은 클래스에서 구현
}