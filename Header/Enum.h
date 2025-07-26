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
    Hover,
    Pressed,
    Disabled
};

enum class LayerType
{
    GROUND = 0,     // 그라운드
    DECORATION = 1, // 장식
    CLIFF = 2,      // 낭떠러지
    COLLIDER = 3    // 콜라이더
};

// 방 타입
enum class RoomType
{
    ENTRANCE = 0,   // 입구방
    BATTLE = 1,     // 전투방
    EXIT = 2,       // 출구방
    BOSS = 3        // 보스방
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
    // 타일 레이어 (그라운드, 장식)
    map<pair<int, int>, TileData> groundLayer;
    map<pair<int, int>, TileData> decorationLayer;

    // 그리드 레이어 (낭떠러지, 콜라이더) - bool 값으로 관리
    map<pair<int, int>, bool> cliffLayer;
    map<pair<int, int>, bool> colliderLayer;
};