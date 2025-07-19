#pragma once
#include <Engine/Object/IPrototypeable/IPrototypeable.h>

// UI 앵커 타입
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

// UI 이벤트 타입
enum class UIEventType
{
    None,
    Click,
    Hover,
    Press,
    Release
};

// UI 이벤트 콜백 함수 타입
using UIEventCallback = function<void()>;

class UI : public IPrototypeable
{
public:
    UI();
    UI(const UI& other);
    virtual ~UI();

public:
    // IPrototypeable 인터페이스
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;

public:
    // 업데이트 및 렌더링
    virtual void Update(float deltaTime);
    virtual void Render(HDC hdc);

    // 마우스 이벤트 처리
    virtual bool OnMouseMove(int x, int y);
    virtual bool OnMouseDown(int x, int y);
    virtual bool OnMouseUp(int x, int y);

    // 위치 및 크기 설정
    void SetPosition(float x, float y);
    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    void SetSize(float width, float height);
    void SetSize(const Vector2& size);
    Vector2 GetSize() const;

    void SetRect(float x, float y, float width, float height);
    void SetRect(const Vector2& position, const Vector2& size);

    // 앵커 설정
    void SetAnchor(UIAnchor anchor);
    UIAnchor GetAnchor() const;

    // 앵커 오프셋 설정 (앵커 지점에서의 상대적 위치)
    void SetAnchorOffset(float x, float y);
    void SetAnchorOffset(const Vector2& offset);
    Vector2 GetAnchorOffset() const;

    // 화면 크기에 따른 실제 위치 계산
    Vector2 GetScreenPosition() const;
    Vector2 GetActualSize() const;

    // 가시성
    void SetVisible(bool visible);
    bool IsVisible() const;

    // 활성화 상태
    void SetEnabled(bool enabled);
    bool IsEnabled() const;

    // 투명도
    void SetAlpha(float alpha);
    float GetAlpha() const;

    // 색상
    void SetColor(COLORREF color);
    COLORREF GetColor() const;

    // 계층 순서 (높을수록 위에 그려짐)
    void SetZOrder(int zOrder);
    int GetZOrder() const;

    // 마우스 충돌 검사
    bool IsPointInside(int x, int y) const;

    // 이벤트 콜백 설정
    void SetClickCallback(UIEventCallback callback);
    void SetHoverCallback(UIEventCallback callback);

    // 자식 UI 관리
    void AddChild(shared_ptr<UI> child);
    void RemoveChild(shared_ptr<UI> child);
    void ClearChildren();
    const vector<shared_ptr<UI>>& GetChildren() const;

    // 부모 UI
    void SetParent(UI* parent);
    UI* GetParent() const;

    // UI 매니저에서 사용하는 정적 함수들
    static void SetScreenSize(float width, float height);
    static Vector2 GetGlobalScreenSize();

protected:
    // 앵커 기준 위치 계산
    Vector2 CalculateAnchorPosition() const;

    // 상속받은 클래스에서 오버라이드할 가상 함수들
    virtual void OnClick();
    virtual void OnHover();
    virtual void OnPress();
    virtual void OnRelease();

protected:
    Vector2 m_Position;         // UI 위치
    Vector2 m_Size;             // UI 크기
    Vector2 m_AnchorOffset;     // 앵커 오프셋
    UIAnchor m_Anchor;          // 앵커 타입

    bool m_Visible;             // 가시성
    bool m_Enabled;             // 활성화 상태
    float m_Alpha;              // 투명도 (0.0f ~ 1.0f)
    COLORREF m_Color;           // 색상
    int m_ZOrder;               // 계층 순서

    // 마우스 상태
    bool m_IsHovered;           // 마우스 호버 상태
    bool m_IsPressed;           // 마우스 눌림 상태

    // 이벤트 콜백
    UIEventCallback m_ClickCallback;
    UIEventCallback m_HoverCallback;

    // 계층 구조
    UI* m_Parent;
    vector<shared_ptr<UI>> m_Children;

    // 정적 멤버
    static Vector2 s_ScreenSize;
};