#pragma once
#include "../SingletonManager/SingletonManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

struct MapData
{
    struct RoomInfo
    {
        int roomType;
        int gridWidth, gridHeight;
        bool canSpawnMonsters;
        bool doors[4]; // North, East, South, West
    } roomInfo;

    struct TileData
    {
        int layer;      // 0: Ground, 1: UpperGround, 2: Collider
        int x, y;
        int value;      // Ÿ�� ID (Ground/UpperGround) �Ǵ� 0/1 (Collider)
    };

    vector<TileData> tiles;
};

struct TileMapSaveData
{
    // �� ����
    int roomType;
    int gridWidth, gridHeight;
    bool canSpawnMonsters;
    bool doors[4];

    // ���̾ Ÿ�� ������
    vector<vector<int>> groundLayer;      // Ÿ�� ID ����
    vector<vector<int>> upperGroundLayer; // Ÿ�� ID ����  
    vector<vector<bool>> colliderLayer;   // 0/1 ����
};

class FileManager : public SingleTon<FileManager>
{
    friend class SingleTon<FileManager>;
private:
    explicit FileManager();
public:
    virtual ~FileManager();

public:
    bool Init();
    void Release();

    // �� ���� ����/�ε� (���ο� Ÿ�� ID ���)
    bool SaveTileMap(const wstring& fileName, const TileMapSaveData& mapData);
    bool LoadTileMap(const wstring& fileName, TileMapSaveData& mapData);

    // ���� �� ���� ��ȯ (���� ���� ȣȯ��)
    bool ConvertOldMapFormat(const wstring& oldFileName, const wstring& newFileName);

    // ���� ��ƿ��Ƽ
    bool FileExists(const wstring& fileName) const;
    bool CreateDirectory(const wstring& dirPath) const;
    vector<wstring> GetMapFileList(const wstring& directory = L"../Maps/") const;
    wstring GetLatestMapFile(const wstring& directory = L"../Maps/") const;
    bool DeleteMapFile(const wstring& fileName) const;

    // ��� ���
    bool CreateBackup(const wstring& fileName) const;
    bool RestoreBackup(const wstring& backupFileName, const wstring& targetFileName) const;
    void CleanOldBackups(const wstring& directory = L"../Maps/Backup/", int keepDays = 7) const;

    // �� ���� �� ����
    bool ValidateMapData(const TileMapSaveData& mapData) const;
    void FixCorruptedMapData(TileMapSaveData& mapData) const;

    // ��� �� ����
    struct MapStatistics
    {
        int totalTiles;
        int uniqueTileCount;
        int colliderCount;
        map<int, int> tileUsageCount; // Ÿ�� ID�� ��� Ƚ��
        size_t fileSize;
    };
    MapStatistics GetMapStatistics(const wstring& fileName) const;

private:
    // ���� �Ľ� ����
    bool ParseRoomInfo(const string& line, MapData::RoomInfo& roomInfo) const;
    bool ParseTileData(const string& line, MapData::TileData& tileData) const;

    // ���� ó��
    void LogError(const string& message) const;
    void LogWarning(const string& message) const;

    // ���ϸ� ��ƿ��Ƽ
    wstring GenerateTimestampedFileName(const wstring& baseName) const;
    wstring GetFileExtension(const wstring& fileName) const;
    wstring GetFileNameWithoutExtension(const wstring& fileName) const;

private:
    bool m_IsInitialized;
    wstring m_DefaultMapDirectory;
    wstring m_BackupDirectory;
    mutable ofstream m_LogFile;
};