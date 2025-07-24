#pragma once
#include "../SingletonManager/SingletonManager.h"

enum class DoorDirection
{
    NORTH = 0,
    SOUTH = 1,
    EAST = 2,
    WEST = 3,
    MAX_DOORS = 4
};

struct RoomConfig
{
    RoomType roomType;
    bool canSpawnMonsters;
    bool doors[static_cast<int>(DoorDirection::MAX_DOORS)]; // N, S, E, W
    int gridWidth;
    int gridHeight;
    string roomName;

    RoomConfig()
        : roomType(RoomType::BATTLE), canSpawnMonsters(true),
        gridWidth(50), gridHeight(50), roomName("Untitled Room")
    {
        // 기본적으로 모든 문이 닫혀있음
        for (int i = 0; i < static_cast<int>(DoorDirection::MAX_DOORS); ++i)
        {
            doors[i] = false;
        }
    }
};

class ConfigManager : public SingleTon<ConfigManager>
{
    friend class SingleTon<ConfigManager>;
private:
    explicit ConfigManager();
public:
    virtual ~ConfigManager();

public:
    // 방 타입 설정
    void SetRoomType(RoomType type);
    RoomType GetRoomType() const { return m_CurrentConfig.roomType; }

    // 몬스터 스폰 설정
    void SetCanSpawnMonsters(bool canSpawn);
    bool CanSpawnMonsters() const { return m_CurrentConfig.canSpawnMonsters; }

    // 문 설정
    void SetDoor(DoorDirection direction, bool hasdoor);
    bool HasDoor(DoorDirection direction) const;
    void ToggleDoor(DoorDirection direction);

    // 그리드 크기 설정
    void SetGridSize(int width, int height);
    int GetGridWidth() const { return m_CurrentConfig.gridWidth; }
    int GetGridHeight() const { return m_CurrentConfig.gridHeight; }

    // 방 이름 설정
    void SetRoomName(const string& name);
    string GetRoomName() const { return m_CurrentConfig.roomName; }

    // 설정 초기화
    void ResetToDefault();

    // 설정 출력
    void PrintCurrentConfig() const;

    // 설정 저장/로드
    RoomConfig GetCurrentConfig() const { return m_CurrentConfig; }
    void SetCurrentConfig(const RoomConfig& config);

private:
    RoomConfig m_CurrentConfig;

    const char* GetRoomTypeName(RoomType type) const;
    const char* GetDoorDirectionName(DoorDirection direction) const;
};