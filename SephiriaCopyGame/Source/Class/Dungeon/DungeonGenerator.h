#pragma once
#include <pch.h>
#include <Engine/Object/Component/Component.h>
#include <Engine/Object/Component/Collider/BoxCollider.h>

// Ÿ�� Ÿ�� ������
enum class TileType
{
    EMPTY = 0,
    WALL = 1,
    FLOOR = 2,
    DOOR = 3,
    CORRIDOR = 4
};

// ���� ������
enum class Direction
{
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

// �� ����ü
struct Room
{
    int x, y;           // ���� ��ġ
    int width, height;  // ���� ũ��
    bool doors[4];      // �� ������ �� ���� (��, ��, ��, ��)
    vector<pair<int, int>> tilePositions; // ���� Ÿ�� ��ġ��

    Room() : x(0), y(0), width(0), height(0)
    {
        for (int i = 0; i < 4; i++) doors[i] = false;
    }

    Room(int _x, int _y, int _w, int _h) : x(_x), y(_y), width(_w), height(_h)
    {
        for (int i = 0; i < 4; i++) doors[i] = false;
    }

    // ���� �߽��� ��ȯ
    pair<int, int> GetCenter() const
    {
        return { x + width / 2, y + height / 2 };
    }

    // ���� ��ġ���� Ȯ��
    bool Overlaps(const Room& other) const
    {
        return !(x + width <= other.x || other.x + other.width <= x ||
            y + height <= other.y || other.y + other.height <= y);
    }
};

// Ÿ�� ������ ����ü
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

// ���� ������ ������Ʈ
class DungeonGenerator : public Component
{
private:
    int m_dungeonWidth;     // ���� �ʺ�
    int m_dungeonHeight;    // ���� ����
    int m_maxRooms;         // �ִ� �� ����
    int m_minRoomSize;      // �ּ� �� ũ��
    int m_maxRoomSize;      // �ִ� �� ũ��

    vector<Room> m_rooms;   // ������ ���
    vector<vector<TileType>> m_dungeonMap;  // ���� �� ������
    vector<TileData> m_tileData;            // �������� Ÿ�� ������
    vector<Object*> m_colliderObjects;     // �ݶ��̴� ������Ʈ��

    // ���� �޼����
    void InitializeDungeon();
    void GenerateRooms();
    void ConnectRooms();
    void CreateCorridors();
    void PlaceTiles();
    void CreateColliders();

    // �ڸ��� ���� ����
    void CreateCorridor(const Room& room1, const Room& room2);
    void CreateHorizontalCorridor(int x1, int x2, int y);
    void CreateVerticalCorridor(int y1, int y2, int x);

    // ��ƿ��Ƽ �޼����
    bool IsValidPosition(int x, int y);
    bool CanPlaceRoom(const Room& room);
    pair<int, int> FindNearestRoomConnection(const Room& room1, const Room& room2);
    void SetTile(int x, int y, TileType type);
    TileType GetTile(int x, int y);

    // �� ���� �ε�
    void LoadMapFromFile(const string& filePath);
    void ParseTileData(const string& line);

public:
    DungeonGenerator(Object* owner);
    DungeonGenerator(const DungeonGenerator& other); // ���� ������

    // Component �������̽� ����
    virtual void Init() override;
    virtual void Update() override;
    virtual void Render(HDC hdc) override;
	virtual void OnDestroy() override;

    // ���� ���� �޼����
    void GenerateDungeon();
    void GenerateDungeonFromFile(const string& filePath);
    void ClearDungeon();

    // ���� �޼����
    void SetDungeonSize(int width, int height);
    void SetRoomCount(int maxRooms);
    void SetRoomSizeRange(int minSize, int maxSize);

    // ������ �޼����
    const vector<Room>& GetRooms() const { return m_rooms; }
    const vector<TileData>& GetTileData() const { return m_tileData; }
    TileType GetTileAt(int x, int y);
    bool HasColliderAt(int x, int y);

    // ���� ������Ʈ ����
    void CreateTileObject(const TileData& tile);
    void CreateColliderObject(int x, int y, int tileSize = 32);
};