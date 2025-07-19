#include "../../pch.h"
#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
    ResetToDefault();
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::SetRoomType(RoomType type)
{
    m_CurrentConfig.roomType = type;

    // 방 타입에 따른 기본 설정 적용
    switch (type)
    {
    case RoomType::ENTRANCE:
        m_CurrentConfig.canSpawnMonsters = false;
        // 입구방은 보통 남쪽에만 문이 있음
        SetDoor(DoorDirection::NORTH, false);
        SetDoor(DoorDirection::SOUTH, true);
        SetDoor(DoorDirection::EAST, false);
        SetDoor(DoorDirection::WEST, false);
        break;

    case RoomType::EXIT:
        m_CurrentConfig.canSpawnMonsters = false;
        // 출구방은 보통 북쪽에만 문이 있음
        SetDoor(DoorDirection::NORTH, true);
        SetDoor(DoorDirection::SOUTH, false);
        SetDoor(DoorDirection::EAST, false);
        SetDoor(DoorDirection::WEST, false);
        break;

    case RoomType::BOSS:
        m_CurrentConfig.canSpawnMonsters = true;
        // 보스방은 보통 하나의 문만 있음
        SetDoor(DoorDirection::NORTH, false);
        SetDoor(DoorDirection::SOUTH, true);
        SetDoor(DoorDirection::EAST, false);
        SetDoor(DoorDirection::WEST, false);
        break;

    case RoomType::COMBAT:
    default:
        m_CurrentConfig.canSpawnMonsters = true;
        // 전투방은 여러 문이 있을 수 있음
        break;
    }

    printf("Room type set to: %s\n", GetRoomTypeName(type));
}

void ConfigManager::SetCanSpawnMonsters(bool canSpawn)
{
    m_CurrentConfig.canSpawnMonsters = canSpawn;
    printf("Monster spawn: %s\n", canSpawn ? "Enabled" : "Disabled");
}

void ConfigManager::SetDoor(DoorDirection direction, bool hasdoor)
{
    int dirIndex = static_cast<int>(direction);
    if (dirIndex >= 0 && dirIndex < static_cast<int>(DoorDirection::MAX_DOORS))
    {
        m_CurrentConfig.doors[dirIndex] = hasdoor;
        /*printf("Door %s: %s\n",
            GetDoorDirectionName(direction),
            HasDoor ? "Open" : "Closed");*/
    }
}

bool ConfigManager::HasDoor(DoorDirection direction) const
{
    int dirIndex = static_cast<int>(direction);
    if (dirIndex >= 0 && dirIndex < static_cast<int>(DoorDirection::MAX_DOORS))
    {
        return m_CurrentConfig.doors[dirIndex];
    }
    return false;
}

void ConfigManager::ToggleDoor(DoorDirection direction)
{
    int dirIndex = static_cast<int>(direction);
    if (dirIndex >= 0 && dirIndex < static_cast<int>(DoorDirection::MAX_DOORS))
    {
        m_CurrentConfig.doors[dirIndex] = !m_CurrentConfig.doors[dirIndex];
        printf("Door %s toggled: %s\n",
            GetDoorDirectionName(direction),
            m_CurrentConfig.doors[dirIndex] ? "Open" : "Closed");
    }
}

void ConfigManager::SetGridSize(int width, int height)
{
    const int MIN_SIZE = 10;
    const int MAX_SIZE = 100;

    m_CurrentConfig.gridWidth = max(MIN_SIZE, min(MAX_SIZE, width));
    m_CurrentConfig.gridHeight = max(MIN_SIZE, min(MAX_SIZE, height));

    printf("Grid size set to: %dx%d\n", m_CurrentConfig.gridWidth, m_CurrentConfig.gridHeight);
}

void ConfigManager::SetRoomName(const string& name)
{
    m_CurrentConfig.roomName = name;
    printf("Room name set to: %s\n", name.c_str());
}

void ConfigManager::ResetToDefault()
{
    m_CurrentConfig = RoomConfig();
    printf("Configuration reset to default\n");
}

void ConfigManager::PrintCurrentConfig() const
{
    printf("\n=== Current Room Configuration ===\n");
    printf("Room Name: %s\n", m_CurrentConfig.roomName.c_str());
    printf("Room Type: %s\n", GetRoomTypeName(m_CurrentConfig.roomType));
    printf("Grid Size: %dx%d\n", m_CurrentConfig.gridWidth, m_CurrentConfig.gridHeight);
    printf("Monster Spawn: %s\n", m_CurrentConfig.canSpawnMonsters ? "Enabled" : "Disabled");
    printf("Doors:\n");
    printf("  North: %s\n", m_CurrentConfig.doors[static_cast<int>(DoorDirection::NORTH)] ? "Open" : "Closed");
    printf("  South: %s\n", m_CurrentConfig.doors[static_cast<int>(DoorDirection::SOUTH)] ? "Open" : "Closed");
    printf("  East: %s\n", m_CurrentConfig.doors[static_cast<int>(DoorDirection::EAST)] ? "Open" : "Closed");
    printf("  West: %s\n", m_CurrentConfig.doors[static_cast<int>(DoorDirection::WEST)] ? "Open" : "Closed");
    printf("================================\n\n");
}

void ConfigManager::SetCurrentConfig(const RoomConfig& config)
{
    m_CurrentConfig = config;
    printf("Configuration loaded\n");
}

const char* ConfigManager::GetRoomTypeName(RoomType type) const
{
    switch (type)
    {
    case RoomType::ENTRANCE: return "Entrance Room";
    case RoomType::COMBAT: return "Combat Room";
    case RoomType::EXIT: return "Exit Room";
    case RoomType::BOSS: return "Boss Room";
    default: return "Unknown";
    }
}

const char* ConfigManager::GetDoorDirectionName(DoorDirection direction) const
{
    switch (direction)
    {
    case DoorDirection::NORTH: return "North";
    case DoorDirection::SOUTH: return "South";
    case DoorDirection::EAST: return "East";
    case DoorDirection::WEST: return "West";
    default: return "Unknown";
    }
}