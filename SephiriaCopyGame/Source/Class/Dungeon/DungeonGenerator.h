#pragma once
#include <pch.h>
#include <Engine/Object/Component/Component.h>
#include <Engine/Object/Component/Collider/BoxCollider.h>

// 타일 타입 열거형
enum class TileType
{
    EMPTY = 0,
    WALL = 1,
    FLOOR = 2,
    DOOR = 3,
    CORRIDOR = 4
};

// 방향 열거형
enum class Direction
{
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

// 방 구조체
struct Room
{
    int x, y;           // 방의 위치
    int width, height;  // 방의 크기
    bool doors[4];      // 각 방향의 문 여부 (북, 동, 남, 서)
    vector<pair<int, int>> tilePositions; // 방의 타일 위치들

    Room() : x(0), y(0), width(0), height(0)
    {
        for (int i = 0; i < 4; i++) doors[i] = false;
    }

    Room(int _x, int _y, int _w, int _h) : x(_x), y(_y), width(_w), height(_h)
    {
        for (int i = 0; i < 4; i++) doors[i] = false;
    }

    // 방의 중심점 반환
    pair<int, int> GetCenter() const
    {
        return { x + width / 2, y + height / 2 };
    }

    // 방이 겹치는지 확인
    bool Overlaps(const Room& other) const
    {
        return !(x + width <= other.x || other.x + other.width <= x ||
            y + height <= other.y || other.y + other.height <= y);
    }
};

// 타일 데이터 구조체
struct TileData
{
    int layer;
    int x, y;
    TileType type;
    bool hasCollider;

    TileData() : layer(0), x(0), y(0), type(TileType::EMPTY), hasCollider(false) {}
    TileData(int _layer, int _x, int _y, TileType _type, bool _collider = false)
        : layer(_layer), x(_x), y(_y), type(_type), hasCollider(_collider) {
    }
};

// 던전 생성기 컴포넌트
class DungeonGenerator : public Component
{
private:
    int m_dungeonWidth;     // 던전 너비
    int m_dungeonHeight;    // 던전 높이
    int m_maxRooms;         // 최대 방 개수
    int m_minRoomSize;      // 최소 방 크기
    int m_maxRoomSize;      // 최대 방 크기

    vector<Room> m_rooms;   // 생성된 방들
    vector<vector<TileType>> m_dungeonMap;  // 던전 맵 데이터
    vector<TileData> m_tileData;            // 렌더링용 타일 데이터
    vector<Object*> m_colliderObjects;     // 콜라이더 오브젝트들

    // 내부 메서드들
    void InitializeDungeon();
    void GenerateRooms();
    void ConnectRooms();
    void CreateCorridors();
    void PlaceTiles();
    void CreateColliders();

    // 코리더 생성 관련
    void CreateCorridor(const Room& room1, const Room& room2);
    void CreateHorizontalCorridor(int x1, int x2, int y);
    void CreateVerticalCorridor(int y1, int y2, int x);

    // 유틸리티 메서드들
    bool IsValidPosition(int x, int y);
    bool CanPlaceRoom(const Room& room);
    pair<int, int> FindNearestRoomConnection(const Room& room1, const Room& room2);
    void SetTile(int x, int y, TileType type);
    TileType GetTile(int x, int y);

    // 맵 파일 로딩
    void LoadMapFromFile(const string& filePath);
    void ParseTileData(const string& line);

public:
    DungeonGenerator(Object* owner);
    DungeonGenerator(const DungeonGenerator& other); // 복사 생성자

    // Component 인터페이스 구현
    virtual void Init() override;
    virtual void Update() override;
    virtual void Render(HDC hdc) override;
	virtual void OnDestroy() override;

    // 던전 생성 메서드들
    void GenerateDungeon();
    void GenerateDungeonFromFile(const string& filePath);
    void ClearDungeon();

    // 설정 메서드들
    void SetDungeonSize(int width, int height);
    void SetRoomCount(int maxRooms);
    void SetRoomSizeRange(int minSize, int maxSize);

    // 접근자 메서드들
    const vector<Room>& GetRooms() const { return m_rooms; }
    const vector<TileData>& GetTileData() const { return m_tileData; }
    TileType GetTileAt(int x, int y);
    bool HasColliderAt(int x, int y);

    // 게임 오브젝트 관련
    void CreateTileObject(const TileData& tile);
    void CreateColliderObject(int x, int y, int tileSize = 32);
};