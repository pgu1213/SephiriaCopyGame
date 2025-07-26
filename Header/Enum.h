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
    Hover,
    Pressed,
    Disabled
};

enum class LayerType
{
    GROUND = 0,     // �׶���
    DECORATION = 1, // ���
    CLIFF = 2,      // ��������
    COLLIDER = 3    // �ݶ��̴�
};

// �� Ÿ��
enum class RoomType
{
    ENTRANCE = 0,   // �Ա���
    BATTLE = 1,     // ������
    EXIT = 2,       // �ⱸ��
    BOSS = 3        // ������
};

struct TileData
{
    wstring tileName;
    int x, y;
    bool isEmpty;
    LayerType layer;

    TileData() : tileName(L""), x(0), y(0), isEmpty(true), layer(LayerType::GROUND) {}
    TileData(const wstring& name, int posX, int posY, LayerType layerType = LayerType::GROUND)
        : tileName(name), x(posX), y(posY), isEmpty(false), layer(layerType) {
    }
};

struct LayerData
{
    // Ÿ�� ���̾� (�׶���, ���)
    map<pair<int, int>, TileData> groundLayer;
    map<pair<int, int>, TileData> decorationLayer;

    // �׸��� ���̾� (��������, �ݶ��̴�) - bool ������ ����
    map<pair<int, int>, bool> cliffLayer;
    map<pair<int, int>, bool> colliderLayer;
};