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
        int value;      // 타일 ID (Ground/UpperGround) 또는 0/1 (Collider)
    };

    vector<TileData> tiles;
};

struct TileMapSaveData
{
    // 룸 정보
    int roomType;
    int gridWidth, gridHeight;
    bool canSpawnMonsters;
    bool doors[4];

    // 레이어별 타일 데이터
    vector<vector<int>> groundLayer;      // 타일 ID 저장
    vector<vector<int>> upperGroundLayer; // 타일 ID 저장  
    vector<vector<bool>> colliderLayer;   // 0/1 저장
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

    // 맵 파일 저장/로드 (새로운 타일 ID 방식)
    bool SaveTileMap(const wstring& fileName, const TileMapSaveData& mapData);
    bool LoadTileMap(const wstring& fileName, TileMapSaveData& mapData);

    // 기존 맵 파일 변환 (이전 버전 호환성)
    bool ConvertOldMapFormat(const wstring& oldFileName, const wstring& newFileName);

    // 파일 유틸리티
    bool FileExists(const wstring& fileName) const;
    bool CreateDirectory(const wstring& dirPath) const;
    vector<wstring> GetMapFileList(const wstring& directory = L"../Maps/") const;
    wstring GetLatestMapFile(const wstring& directory = L"../Maps/") const;
    bool DeleteMapFile(const wstring& fileName) const;

    // 백업 기능
    bool CreateBackup(const wstring& fileName) const;
    bool RestoreBackup(const wstring& backupFileName, const wstring& targetFileName) const;
    void CleanOldBackups(const wstring& directory = L"../Maps/Backup/", int keepDays = 7) const;

    // 맵 검증 및 수정
    bool ValidateMapData(const TileMapSaveData& mapData) const;
    void FixCorruptedMapData(TileMapSaveData& mapData) const;

    // 통계 및 정보
    struct MapStatistics
    {
        int totalTiles;
        int uniqueTileCount;
        int colliderCount;
        map<int, int> tileUsageCount; // 타일 ID별 사용 횟수
        size_t fileSize;
    };
    MapStatistics GetMapStatistics(const wstring& fileName) const;

private:
    // 파일 파싱 헬퍼
    bool ParseRoomInfo(const string& line, MapData::RoomInfo& roomInfo) const;
    bool ParseTileData(const string& line, MapData::TileData& tileData) const;

    // 에러 처리
    void LogError(const string& message) const;
    void LogWarning(const string& message) const;

    // 파일명 유틸리티
    wstring GenerateTimestampedFileName(const wstring& baseName) const;
    wstring GetFileExtension(const wstring& fileName) const;
    wstring GetFileNameWithoutExtension(const wstring& fileName) const;

private:
    bool m_IsInitialized;
    wstring m_DefaultMapDirectory;
    wstring m_BackupDirectory;
    mutable ofstream m_LogFile;
};