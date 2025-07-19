#pragma once
#include "../UI.h"

// 버튼 상태
enum class ButtonState
{
    Normal,
    Hovered,
    Pressed,
    Disabled
};

class UIButton : public UI
{
public:
    UIButton();
    UIButton(const UIButton& other);
    virtual ~UIButton();

public:
    // IPrototypeable 인터페이스
    unique_ptr<IPrototypeable> Clone() const override;
    void CopyFrom(const IPrototypeable* source) override;

public:
	void Update(float deltaTime) override;
    void Render(HDC hdc) override;

    // 텍스트 설정
    void SetText(const wstring& text);
    wstring GetText() const;

    // 폰트 설정
    void SetFont(const wstring& fontName, int fontSize, bool bold = false);
    void SetFontSize(int fontSize);
    void SetFontBold(bool bold);

    // 텍스트 색상 설정 (상태별)
    void SetTextColor(COLORREF color);
    void SetTextColor(ButtonState state, COLORREF color);
    COLORREF GetTextColor(ButtonState state) const;

    // 배경 색상 설정 (상태별)
    void SetBackgroundColor(ButtonState state, COLORREF color);
    COLORREF GetBackgroundColor(ButtonState state) const;

    // 테두리 설정
    void SetBorderEnabled(bool enabled);
    bool IsBorderEnabled() const;
    void SetBorderColor(COLORREF color);
    void SetBorderColor(ButtonState state, COLORREF color);
    COLORREF GetBorderColor(ButtonState state) const;
    void SetBorderWidth(int width);
    int GetBorderWidth() const;

    // 버튼 상태
    ButtonState GetButtonState() const;

    // 텍스트 정렬
    void SetTextAlignment(int alignment); // DT_CENTER, DT_LEFT, DT_RIGHT 등
    int GetTextAlignment() const;

    // 이미지 버튼 관련
    void SetImage(ButtonState state, Gdiplus::Bitmap* image);
    Gdiplus::Bitmap* GetImage(ButtonState state) const;
    void SetImageOnly(bool imageOnly); // 텍스트 없이 이미지만 표시
    bool IsImageOnly() const;

protected:
    // UI 이벤트 오버라이드
    void OnHover() override;
    void OnPress() override;
    void OnRelease() override;

    // 렌더링 헬퍼 함수들
    void RenderBackground(HDC hdc);
    void RenderBorder(HDC hdc);
    void RenderText(HDC hdc);
    void RenderImage(HDC hdc);

    // 현재 상태에 따른 색상 가져오기
    COLORREF GetCurrentTextColor() const;
    COLORREF GetCurrentBackgroundColor() const;
    COLORREF GetCurrentBorderColor() const;

    // 폰트 생성/해제
    void UICreateFont();
    void UIDestroyFont();

private:
    wstring m_Text;                     // 버튼 텍스트
    wstring m_FontName;                 // 폰트 이름
    int m_FontSize;                     // 폰트 크기
    bool m_FontBold;                    // 폰트 굵기
    HFONT m_Font;                       // 폰트 핸들
    int m_TextAlignment;                // 텍스트 정렬

    // 상태별 색상
    COLORREF m_TextColors[4];           // 텍스트 색상 (Normal, Hovered, Pressed, Disabled)
    COLORREF m_BackgroundColors[4];     // 배경 색상
    COLORREF m_BorderColors[4];         // 테두리 색상

    // 테두리 설정
    bool m_BorderEnabled;               // 테두리 사용 여부
    int m_BorderWidth;                  // 테두리 두께

    // 이미지 관련
    Gdiplus::Bitmap* m_Images[4];       // 상태별 이미지
    bool m_ImageOnly;                   // 이미지만 표시 여부

    // 현재 버튼 상태
    ButtonState m_CurrentState;
};