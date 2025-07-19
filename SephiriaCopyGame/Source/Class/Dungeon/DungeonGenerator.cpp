#include <pch.h>
#include "DungeonGenerator.h"
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Managers/Engine/Engine.h>
#include <fstream>
#include <sstream>
#include <random>

DungeonGenerator::DungeonGenerator(Object* owner)
    : Component(owner)
    , m_dungeonWidth(50)
    , m_dungeonHeight(50)
    , m_maxRooms(10)
    , m_minRoomSize(5)
    , m_maxRoomSize(15)
{
}

DungeonGenerator::DungeonGenerator(const DungeonGenerator& other)
    : Component(other.m_pOwner)
    , m_dungeonWidth(other.m_dungeonWidth)
    , m_dungeonHeight(other.m_dungeonHeight)
	, m_maxRooms(other.m_maxRooms)
    , m_minRoomSize(other.m_minRoomSize)
    , m_maxRoomSize(other.m_maxRoomSize)
    , m_rooms(other.m_rooms)
    , m_dungeonMap(other.m_dungeonMap)
    , m_tileData(other.m_tileData)
	, m_colliderObjects(other.m_colliderObjects)
{
}

void DungeonGenerator::Init()
{
    // 시작 시 던전 초기화
    InitializeDungeon();
}

void DungeonGenerator::Update()
{
    // 업데이트 로직이 필요한 경우 여기에 구현
}

void DungeonGenerator::Render(HDC hdc)
{
    // 던전 타일들을 렌더링
    for (const auto& tile : m_tileData)
    {
        // 타일 타입에 따른 색상 설정
        COLORREF color;
        switch (tile.type)
        {
        case TileType::WALL:
            color = RGB(100, 100, 100);  // 회색
            break;
        case TileType::FLOOR:
            color = RGB(200, 200, 200);  // 밝은 회색
            break;
        case TileType::DOOR:
            color = RGB(139, 69, 19);    // 갈색
            break;
        case TileType::CORRIDOR:
            color = RGB(150, 150, 150);  // 중간 회색
            break;
        default:
            continue; // 빈 타일은 그리지 않음
        }

        // 타일 그리기 (32x32 크기로 가정)
        HBRUSH brush = CreateSolidBrush(color);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

        Rectangle(hdc, tile.x * 32, tile.y * 32,
            (tile.x + 1) * 32, (tile.y + 1) * 32);

        SelectObject(hdc, oldBrush);
        DeleteObject(brush);
    }
}

void DungeonGenerator::OnDestroy()
{
    // 콜라이더 오브젝트들 정리
    for (auto* obj : m_colliderObjects)
    {
        if (obj)
        {
            delete obj;
        }
    }
    m_colliderObjects.clear();

    // 기타 메모리 정리
    m_rooms.clear();
    m_tileData.clear();
    m_dungeonMap.clear();
}

void DungeonGenerator::InitializeDungeon()
{
    // 던전 맵 초기화
    m_dungeonMap.assign(m_dungeonHeight, vector<TileType>(m_dungeonWidth, TileType::EMPTY));
    m_rooms.clear();
    m_tileData.clear();
}

void DungeonGenerator::GenerateDungeon()
{
    InitializeDungeon();
    GenerateRooms();
    ConnectRooms();
    PlaceTiles();
    CreateColliders();
}

void DungeonGenerator::GenerateDungeonFromFile(const string& filePath)
{
    InitializeDungeon();
    LoadMapFromFile(filePath);
    CreateColliders();
}

void DungeonGenerator::LoadMapFromFile(const string& filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
    {
        return;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        if (line.substr(0, 2) == "T=")
        {
            ParseTileData(line);
        }
    }

    file.close();
}

void DungeonGenerator::ParseTileData(const string& line)
{
    // "T=Layer,X,Y,Type" 형식 파싱
    size_t equalPos = line.find('=');
    if (equalPos == string::npos) return;

    string data = line.substr(equalPos + 1);
    istringstream iss(data);
    string token;

    vector<int> values;
    while (getline(iss, token, ','))
    {
        values.push_back(stoi(token));
    }

    if (values.size() >= 4)
    {
        int layer = values[0];
        int x = values[1];
        int y = values[2];
        int typeValue = values[3];

        // 타일 타입 변환 (1 = 벽으로 가정)
        TileType type = (typeValue == 1) ? TileType::WALL : TileType::FLOOR;

        // 던전 맵에 설정
        if (IsValidPosition(x, y))
        {
            SetTile(x, y, type);

            // 타일 데이터 추가
            bool hasCollider = (type == TileType::WALL);
            m_tileData.emplace_back(layer, x, y, type, hasCollider);
        }
    }
}

void DungeonGenerator::GenerateRooms()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> sizeDist(m_minRoomSize, m_maxRoomSize);
    uniform_int_distribution<> xDist(1, m_dungeonWidth - m_maxRoomSize - 1);
    uniform_int_distribution<> yDist(1, m_dungeonHeight - m_maxRoomSize - 1);

    for (int i = 0; i < m_maxRooms; ++i)
    {
        int width = sizeDist(gen);
        int height = sizeDist(gen);
        int x = xDist(gen);
        int y = yDist(gen);

        Room newRoom(x, y, width, height);

        if (CanPlaceRoom(newRoom))
        {
            m_rooms.push_back(newRoom);

            // 방 영역을 바닥으로 설정
            for (int rx = x; rx < x + width; ++rx)
            {
                for (int ry = y; ry < y + height; ++ry)
                {
                    SetTile(rx, ry, TileType::FLOOR);
                }
            }

            // 방 테두리를 벽으로 설정
            for (int rx = x - 1; rx <= x + width; ++rx)
            {
                if (IsValidPosition(rx, y - 1) && GetTile(rx, y - 1) == TileType::EMPTY)
                    SetTile(rx, y - 1, TileType::WALL);
                if (IsValidPosition(rx, y + height) && GetTile(rx, y + height) == TileType::EMPTY)
                    SetTile(rx, y + height, TileType::WALL);
            }
            for (int ry = y - 1; ry <= y + height; ++ry)
            {
                if (IsValidPosition(x - 1, ry) && GetTile(x - 1, ry) == TileType::EMPTY)
                    SetTile(x - 1, ry, TileType::WALL);
                if (IsValidPosition(x + width, ry) && GetTile(x + width, ry) == TileType::EMPTY)
                    SetTile(x + width, ry, TileType::WALL);
            }
        }
    }
}

void DungeonGenerator::ConnectRooms()
{
    if (m_rooms.size() < 2) return;

    // 모든 방을 연결
    for (size_t i = 1; i < m_rooms.size(); ++i)
    {
        CreateCorridor(m_rooms[i - 1], m_rooms[i]);
    }

    // 추가 연결 생성 (순환 구조)
    if (m_rooms.size() > 2)
    {
        CreateCorridor(m_rooms.back(), m_rooms.front());
    }
}

void DungeonGenerator::CreateCorridor(const Room& room1, const Room& room2)
{
    auto center1 = room1.GetCenter();
    auto center2 = room2.GetCenter();

    // L자형 코리더 생성
    CreateHorizontalCorridor(center1.first, center2.first, center1.second);
    CreateVerticalCorridor(center1.second, center2.second, center2.first);
}

void DungeonGenerator::CreateHorizontalCorridor(int x1, int x2, int y)
{
    int startX = min(x1, x2);
    int endX = max(x1, x2);

    for (int x = startX; x <= endX; ++x)
    {
        if (IsValidPosition(x, y))
        {
            if (GetTile(x, y) == TileType::EMPTY || GetTile(x, y) == TileType::WALL)
            {
                SetTile(x, y, TileType::CORRIDOR);
            }

            // 코리더 벽 생성
            if (IsValidPosition(x, y - 1) && GetTile(x, y - 1) == TileType::EMPTY)
                SetTile(x, y - 1, TileType::WALL);
            if (IsValidPosition(x, y + 1) && GetTile(x, y + 1) == TileType::EMPTY)
                SetTile(x, y + 1, TileType::WALL);
        }
    }
}

void DungeonGenerator::CreateVerticalCorridor(int y1, int y2, int x)
{
    int startY = min(y1, y2);
    int endY = max(y1, y2);

    for (int y = startY; y <= endY; ++y)
    {
        if (IsValidPosition(x, y))
        {
            if (GetTile(x, y) == TileType::EMPTY || GetTile(x, y) == TileType::WALL)
            {
                SetTile(x, y, TileType::CORRIDOR);
            }

            // 코리더 벽 생성
            if (IsValidPosition(x - 1, y) && GetTile(x - 1, y) == TileType::EMPTY)
                SetTile(x - 1, y, TileType::WALL);
            if (IsValidPosition(x + 1, y) && GetTile(x + 1, y) == TileType::EMPTY)
                SetTile(x + 1, y, TileType::WALL);
        }
    }
}

void DungeonGenerator::PlaceTiles()
{
    m_tileData.clear();

    for (int y = 0; y < m_dungeonHeight; ++y)
    {
        for (int x = 0; x < m_dungeonWidth; ++x)
        {
            TileType type = GetTile(x, y);
            if (type != TileType::EMPTY)
            {
                bool hasCollider = (type == TileType::WALL);
                m_tileData.emplace_back(0, x, y, type, hasCollider);
            }
        }
    }
}

void DungeonGenerator::CreateColliders()
{
    // 기존 콜라이더 정리
    for (auto* obj : m_colliderObjects)
    {
        if (obj) delete obj;
    }
    m_colliderObjects.clear();

    // 벽 타일에 콜라이더 생성
    for (const auto& tile : m_tileData)
    {
        if (tile.hasCollider)
        {
            CreateColliderObject(tile.x, tile.y);
        }
    }
}

void DungeonGenerator::CreateColliderObject(int x, int y, int tileSize)
{
    Object* colliderObj = new Object();
    colliderObj->SetPosition(Vector2(x * tileSize + tileSize / 2.0f,
        y * tileSize + tileSize / 2.0f));

    BoxCollider* boxCollider = new BoxCollider();
    boxCollider->SetSize(Vector2(tileSize, tileSize));
    colliderObj->AddComponent(boxCollider);

    m_colliderObjects.push_back(colliderObj);
}

// 유틸리티 메서드들
bool DungeonGenerator::IsValidPosition(int x, int y)
{
    return x >= 0 && x < m_dungeonWidth && y >= 0 && y < m_dungeonHeight;
}

bool DungeonGenerator::CanPlaceRoom(const Room& room)
{
    // 던전 경계 확인
    if (room.x <= 0 || room.y <= 0 ||
        room.x + room.width >= m_dungeonWidth ||
        room.y + room.height >= m_dungeonHeight)
    {
        return false;
    }

    // 다른 방과의 겹침 확인
    for (const auto& existingRoom : m_rooms)
    {
        if (room.Overlaps(existingRoom))
        {
            return false;
        }
    }

    return true;
}

void DungeonGenerator::SetTile(int x, int y, TileType type)
{
    if (IsValidPosition(x, y))
    {
        m_dungeonMap[y][x] = type;
    }
}

TileType DungeonGenerator::GetTile(int x, int y)
{
    if (IsValidPosition(x, y))
    {
        return m_dungeonMap[y][x];
    }
    return TileType::EMPTY;
}

TileType DungeonGenerator::GetTileAt(int x, int y)
{
    return GetTile(x, y);
}

bool DungeonGenerator::HasColliderAt(int x, int y)
{
    TileType type = GetTile(x, y);
    return type == TileType::WALL;
}

void DungeonGenerator::SetDungeonSize(int width, int height)
{
    m_dungeonWidth = width;
    m_dungeonHeight = height;
}

void DungeonGenerator::SetRoomCount(int maxRooms)
{
    m_maxRooms = maxRooms;
}

void DungeonGenerator::SetRoomSizeRange(int minSize, int maxSize)
{
    m_minRoomSize = minSize;
    m_maxRoomSize = maxSize;
}

void DungeonGenerator::ClearDungeon()
{
    Release();
    InitializeDungeon();
}