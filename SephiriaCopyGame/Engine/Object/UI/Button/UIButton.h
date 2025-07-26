#pragma once
#include <Engine/Object/Component/Component.h>

class UIButtonComponent : public Component
{
private:
    std::function<void()> m_OnClick;
    std::function<void()> m_OnHover;
    std::function<void()> m_OnHoverExit;

    ButtonState m_CurrentState;
    ButtonState m_PreviousState;

    std::string m_Text;
    Color m_TextColor;
    Color m_BackgroundColor;

    bool m_IsInteractable;

public:
    explicit UIButtonComponent(UI* owner);
    UIButtonComponent(const UIButtonComponent& other);

    Component* CloneImpl() const override;

    void Update(float deltaTime) override;
    void Render(HDC hdc) override;

    // �ݹ� ����
    void SetOnClick(std::function<void()> callback) { m_OnClick = callback; }
    void SetOnHover(std::function<void()> callback) { m_OnHover = callback; }
    void SetOnHoverExit(std::function<void()> callback) { m_OnHoverExit = callback; }

    // �ؽ�Ʈ ����
    void SetText(const std::string& text) { m_Text = text; }
    const std::string& GetText() const { return m_Text; }

    void SetTextColor(const Color& color) { m_TextColor = color; }
    const Color& GetTextColor() const { return m_TextColor; }

    // ���� ����
    ButtonState GetState() const { return m_CurrentState; }
    void SetInteractable(bool interactable) { m_IsInteractable = interactable; }
    bool IsInteractable() const { return m_IsInteractable; }

private:
    void UpdateButtonState();
    void HandleStateChange();
    bool IsMouseOverButton() const;
};