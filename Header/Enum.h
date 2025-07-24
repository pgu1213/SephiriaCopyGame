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

// ��ư ����
enum class ButtonState
{
    Normal,
    Hovered,
    Pressed,
    Disabled
};

// �� Ÿ��
enum class RoomType
{
    ENTRANCE = 0,   // �Ա���
    BATTLE = 1,     // ������
    EXIT = 2,       // �ⱸ��
    BOSS = 3        // ������
};