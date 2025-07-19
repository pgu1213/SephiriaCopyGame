#pragma once
#include <Engine/Object/IPrototypeable/IPrototypeable.h>

// UI ��Ŀ Ÿ��
enum class UIAnchor
{
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// UI �̺�Ʈ Ÿ��
enum class UIEventType
{
    None,
    Click,
    Hover,
    Press,
    Release
};

// UI �̺�Ʈ �ݹ� �Լ� Ÿ��
using UIEventCallback = function<void()>;

class UI : public IPrototypeable
{
public:
    UI();
    UI(const UI& other);
    virtual ~UI();

public:
    // IPrototypeable �������̽�
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;

public:
    // ������Ʈ �� ������
    virtual void Update(float deltaTime);
    virtual void Render(HDC hdc);

    // ���콺 �̺�Ʈ ó��
    virtual bool OnMouseMove(int x, int y);
    virtual bool OnMouseDown(int x, int y);
    virtual bool OnMouseUp(int x, int y);

    // ��ġ �� ũ�� ����
    void SetPosition(float x, float y);
    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    void SetSize(float width, float height);
    void SetSize(const Vector2& size);
    Vector2 GetSize() const;

    void SetRect(float x, float y, float width, float height);
    void SetRect(const Vector2& position, const Vector2& size);

    // ��Ŀ ����
    void SetAnchor(UIAnchor anchor);
    UIAnchor GetAnchor() const;

    // ��Ŀ ������ ���� (��Ŀ ���������� ����� ��ġ)
    void SetAnchorOffset(float x, float y);
    void SetAnchorOffset(const Vector2& offset);
    Vector2 GetAnchorOffset() const;

    // ȭ�� ũ�⿡ ���� ���� ��ġ ���
    Vector2 GetScreenPosition() const;
    Vector2 GetActualSize() const;

    // ���ü�
    void SetVisible(bool visible);
    bool IsVisible() const;

    // Ȱ��ȭ ����
    void SetEnabled(bool enabled);
    bool IsEnabled() const;

    // ����
    void SetAlpha(float alpha);
    float GetAlpha() const;

    // ����
    void SetColor(COLORREF color);
    COLORREF GetColor() const;

    // ���� ���� (�������� ���� �׷���)
    void SetZOrder(int zOrder);
    int GetZOrder() const;

    // ���콺 �浹 �˻�
    bool IsPointInside(int x, int y) const;

    // �̺�Ʈ �ݹ� ����
    void SetClickCallback(UIEventCallback callback);
    void SetHoverCallback(UIEventCallback callback);

    // �ڽ� UI ����
    void AddChild(shared_ptr<UI> child);
    void RemoveChild(shared_ptr<UI> child);
    void ClearChildren();
    const vector<shared_ptr<UI>>& GetChildren() const;

    // �θ� UI
    void SetParent(UI* parent);
    UI* GetParent() const;

    // UI �Ŵ������� ����ϴ� ���� �Լ���
    static void SetScreenSize(float width, float height);
    static Vector2 GetGlobalScreenSize();

protected:
    // ��Ŀ ���� ��ġ ���
    Vector2 CalculateAnchorPosition() const;

    // ��ӹ��� Ŭ�������� �������̵��� ���� �Լ���
    virtual void OnClick();
    virtual void OnHover();
    virtual void OnPress();
    virtual void OnRelease();

protected:
    Vector2 m_Position;         // UI ��ġ
    Vector2 m_Size;             // UI ũ��
    Vector2 m_AnchorOffset;     // ��Ŀ ������
    UIAnchor m_Anchor;          // ��Ŀ Ÿ��

    bool m_Visible;             // ���ü�
    bool m_Enabled;             // Ȱ��ȭ ����
    float m_Alpha;              // ���� (0.0f ~ 1.0f)
    COLORREF m_Color;           // ����
    int m_ZOrder;               // ���� ����

    // ���콺 ����
    bool m_IsHovered;           // ���콺 ȣ�� ����
    bool m_IsPressed;           // ���콺 ���� ����

    // �̺�Ʈ �ݹ�
    UIEventCallback m_ClickCallback;
    UIEventCallback m_HoverCallback;

    // ���� ����
    UI* m_Parent;
    vector<shared_ptr<UI>> m_Children;

    // ���� ���
    static Vector2 s_ScreenSize;
};