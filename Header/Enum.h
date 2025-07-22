#pragma once

enum class CollisionLayer : uint32_t
{
    None = 0,
    Player = 1 << 0,        // 1
    Enemy = 1 << 1,         // 2
    Wall = 1 << 2,          // 4
    Item = 1 << 3,          // 8
    Trigger = 1 << 4,       // 16
    All = 0xFFFFFFFF
};

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

// 버튼 상태
enum class ButtonState
{
    Normal,
    Hovered,
    Pressed,
    Disabled
};