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
        // �⺻������ ��� ���� ��������
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
    // �� Ÿ�� ����
    void SetRoomType(RoomType type);
    RoomType GetRoomType() const { return m_CurrentConfig.roomType; }

    // ���� ���� ����
    void SetCanSpawnMonsters(bool canSpawn);
    bool CanSpawnMonsters() const { return m_CurrentConfig.canSpawnMonsters; }

    // �� ����
    void SetDoor(DoorDirection direction, bool hasdoor);
    bool HasDoor(DoorDirection direction) const;
    void ToggleDoor(DoorDirection direction);

    // �׸��� ũ�� ����
    void SetGridSize(int width, int height);
    int GetGridWidth() const { return m_CurrentConfig.gridWidth; }
    int GetGridHeight() const { return m_CurrentConfig.gridHeight; }

    // �� �̸� ����
    void SetRoomName(const string& name);
    string GetRoomName() const { return m_CurrentConfig.roomName; }

    // ���� �ʱ�ȭ
    void ResetToDefault();

    // ���� ���
    void PrintCurrentConfig() const;

    // ���� ����/�ε�
    RoomConfig GetCurrentConfig() const { return m_CurrentConfig; }
    void SetCurrentConfig(const RoomConfig& config);

private:
    RoomConfig m_CurrentConfig;

    const char* GetRoomTypeName(RoomType type) const;
    const char* GetDoorDirectionName(DoorDirection direction) const;
};