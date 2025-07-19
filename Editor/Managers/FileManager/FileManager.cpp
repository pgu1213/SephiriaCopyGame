#include "../../pch.h"
#include "FileManager.h"
#include <iostream>
#include <iomanip>
#include <chrono>

FileManager::FileManager()
    : m_IsInitialized(false)
    , m_DefaultMapDirectory(L"../Maps/")
    , m_BackupDirectory(L"../Maps/Backup/")
{
}

FileManager::~FileManager()
{
    Release();
}

bool FileManager::Init()
{
    if (m_IsInitialized) return true;

    try
    {
        // 디렉토리 생성
        CreateDirectory(m_DefaultMapDirectory);
        CreateDirectory(m_BackupDirectory);

        // 로그 파일 초기화
        wstring logPath = m_DefaultMapDirectory + L"file_manager.log";
        m_LogFile.open(logPath, ios::app);

        m_IsInitialized = true;
        LogError("FileManager initialized successfully");
        return true;
    }
    catch (const exception& e)
    {
        LogError("Failed to initialize FileManager: " + string(e.what()));
        return false;
    }
}

void FileManager::Release()
{
    if (m_LogFile.is_open())
    {
        LogError("FileManager shutting down");
        m_LogFile.close();
    }
    m_IsInitialized = false;
}

bool FileManager::SaveTileMap(const wstring& fileName, const TileMapSaveData& mapData)
{
    if (!m_IsInitialized)
    {
        LogError("FileManager not initialized");
        return false;
    }

    if (!ValidateMapData(mapData))
    {
        LogError("Invalid map data provided for saving");
        return false;
    }

    try
    {
        // 백업 생성 (기존 파일이 있는 경우)
        if (FileExists(fileName))
        {
            CreateBackup(fileName);
        }

        ofstream file(fileName);
        if (!file.is_open())
        {
            LogError("Failed to open file for writing: " + string(fileName.begin(), fileName.end()));
            return false;
        }

        // 헤더 정보 저장
        file << "# Room Data for Game (Tile ID Format v2.0)" << endl;
        file << "RoomType=" << mapData.roomType << endl;
        file << "GridSize=" << mapData.gridWidth << "," << mapData.gridHeight << endl;
        file << "CanSpawnMonsters=" << (mapData.canSpawnMonsters ? 1 : 0) << endl;

        // 문 정보 저장
        file << "Doors=";
        for (int i = 0; i < 4; ++i)
        {
            file << (mapData.doors[i] ? 1 : 0);
            if (i < 3) file << ",";
        }
        file << endl << endl;

        file << "# Tile Data (Layer,X,Y,Value)" << endl;
        file << "# Layer 0: Ground (TileID), Layer 1: UpperGround (TileID), Layer 2: Collider (0/1)" << endl;

        // Ground 레이어 저장 (타일 ID)
        for (int y = 0; y < mapData.gridHeight; ++y)
        {
            for (int x = 0; x < mapData.gridWidth; ++x)
            {
                if (mapData.groundLayer[y][x] > 0)
                {
                    file << "T=0," << x << "," << y << "," << mapData.groundLayer[y][x] << endl;
                }
            }
        }

        // UpperGround 레이어 저장 (타일 ID)
        for (int y = 0; y < mapData.gridHeight; ++y)
        {
            for (int x = 0; x < mapData.gridWidth; ++x)
            {
                if (mapData.upperGroundLayer[y][x] > 0)
                {
                    file << "T=1," << x << "," << y << "," << mapData.upperGroundLayer[y][x] << endl;
                }
            }
        }

        // Collider 레이어 저장 (0/1)
        for (int y = 0; y < mapData.gridHeight; ++y)
        {
            for (int x = 0; x < mapData.gridWidth; ++x)
            {
                if (mapData.colliderLayer[y][x])
                {
                    file << "T=2," << x << "," << y << ",1" << endl;
                }
            }
        }

        file.close();

        LogError("Map saved successfully: " + string(fileName.begin(), fileName.end()));
        return true;
    }
    catch (const exception& e)
    {
        LogError("Exception while saving map: " + string(e.what()));
        return false;
    }
}

bool FileManager::LoadTileMap(const wstring& fileName, TileMapSaveData& mapData)
{
    if (!m_IsInitialized)
    {
        LogError("FileManager not initialized");
        return false;
    }

    if (!FileExists(fileName))
    {
        LogError("Map file does not exist: " + string(fileName.begin(), fileName.end()));
        return false;
    }

    try
    {
        ifstream file(fileName);
        if (!file.is_open())
        {
            LogError("Failed to open file for reading: " + string(fileName.begin(), fileName.end()));
            return false;
        }

        // 기본값 초기화
        mapData.roomType = 1;
        mapData.gridWidth = 50;
        mapData.gridHeight = 50;
        mapData.canSpawnMonsters = true;
        memset(mapData.doors, false, sizeof(mapData.doors));

        string line;
        while (getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;

            // 룸 타입 파싱
            if (line.find("RoomType=") == 0)
            {
                mapData.roomType = stoi(line.substr(9));
            }
            // 그리드 크기 파싱
            else if (line.find("GridSize=") == 0)
            {
                string sizeStr = line.substr(9);
                size_t commaPos = sizeStr.find(',');
                if (commaPos != string::npos)
                {
                    mapData.gridWidth = stoi(sizeStr.substr(0, commaPos));
                    mapData.gridHeight = stoi(sizeStr.substr(commaPos + 1));
                }
            }
            // 몬스터 스폰 가능 여부
            else if (line.find("CanSpawnMonsters=") == 0)
            {
                mapData.canSpawnMonsters = (stoi(line.substr(17)) == 1);
            }
            // 문 정보 파싱
            else if (line.find("Doors=") == 0)
            {
                string doorStr = line.substr(6);
                stringstream ss(doorStr);
                string token;
                int doorIndex = 0;

                while (getline(ss, token, ',') && doorIndex < 4)
                {
                    mapData.doors[doorIndex] = (stoi(token) == 1);
                    doorIndex++;
                }
            }
        }

        // 레이어 데이터 초기화
        mapData.groundLayer.resize(mapData.gridHeight);
        mapData.upperGroundLayer.resize(mapData.gridHeight);
        mapData.colliderLayer.resize(mapData.gridHeight);

        for (int y = 0; y < mapData.gridHeight; ++y)
        {
            mapData.groundLayer[y].resize(mapData.gridWidth, 0);
            mapData.upperGroundLayer[y].resize(mapData.gridWidth, 0);
            mapData.colliderLayer[y].resize(mapData.gridWidth, false);
        }

        // 파일을 다시 읽어서 타일 데이터 파싱
        file.clear();
        file.seekg(0, ios::beg);

        while (getline(file, line))
        {
            if (line.find("T=") == 0)
            {
                // T=Layer,X,Y,Value 형식 파싱
                stringstream ss(line.substr(2));
                string token;
                vector<int> values;

                while (getline(ss, token, ','))
                {
                    values.push_back(stoi(token));
                }

                if (values.size() == 4)
                {
                    int layer = values[0];
                    int x = values[1];
                    int y = values[2];
                    int value = values[3];

                    // 범위 체크
                    if (x >= 0 && x < mapData.gridWidth && y >= 0 && y < mapData.gridHeight)
                    {
                        if (layer == 0) // Ground
                        {
                            mapData.groundLayer[y][x] = value;
                        }
                        else if (layer == 1) // UpperGround
                        {
                            mapData.upperGroundLayer[y][x] = value;
                        }
                        else if (layer == 2) // Collider
                        {
                            mapData.colliderLayer[y][x] = (value == 1);
                        }
                    }
                }
            }
        }

        file.close();

        // 로드된 데이터 검증 및 수정
        FixCorruptedMapData(mapData);

        LogError("Map loaded successfully: " + string(fileName.begin(), fileName.end()));
        return true;
    }
    catch (const exception& e)
    {
        LogError("Exception while loading map: " + string(e.what()));
        return false;
    }
}

bool FileManager::ValidateMapData(const TileMapSaveData& mapData) const
{
    // 기본 검증
    if (mapData.gridWidth <= 0 || mapData.gridHeight <= 0)
    {
        LogError("Invalid grid size in map data");
        return false;
    }

    if (mapData.gridWidth > 1000 || mapData.gridHeight > 1000)
    {
        LogWarning("Large grid size detected, this may impact performance");
    }

    // 레이어 크기 검증
    if (mapData.groundLayer.size() != mapData.gridHeight ||
        mapData.upperGroundLayer.size() != mapData.gridHeight ||
        mapData.colliderLayer.size() != mapData.gridHeight)
    {
        LogError("Layer height mismatch in map data");
        return false;
    }

    for (int y = 0; y < mapData.gridHeight; ++y)
    {
        if (mapData.groundLayer[y].size() != mapData.gridWidth ||
            mapData.upperGroundLayer[y].size() != mapData.gridWidth ||
            mapData.colliderLayer[y].size() != mapData.gridWidth)
        {
            LogError("Layer width mismatch in map data at row " + to_string(y));
            return false;
        }
    }

    return true;
}

void FileManager::FixCorruptedMapData(TileMapSaveData& mapData) const
{
    // 타일 ID 범위 검증 (0-1000 범위로 제한)
    for (int y = 0; y < mapData.gridHeight; ++y)
    {
        for (int x = 0; x < mapData.gridWidth; ++x)
        {
            if (mapData.groundLayer[y][x] < 0 || mapData.groundLayer[y][x] > 1000)
            {
                LogWarning("Invalid tile ID found at Ground(" + to_string(x) + "," + to_string(y) + "), resetting to 0");
                mapData.groundLayer[y][x] = 0;
            }

            if (mapData.upperGroundLayer[y][x] < 0 || mapData.upperGroundLayer[y][x] > 1000)
            {
                LogWarning("Invalid tile ID found at UpperGround(" + to_string(x) + "," + to_string(y) + "), resetting to 0");
                mapData.upperGroundLayer[y][x] = 0;
            }
        }
    }
}

bool FileManager::FileExists(const wstring& fileName) const
{
    return filesystem::exists(fileName);
}

bool FileManager::CreateDirectory(const wstring& dirPath) const
{
    try
    {
        if (!filesystem::exists(dirPath))
        {
            return filesystem::create_directories(dirPath);
        }
        return true;
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Failed to create directory: " + string(e.what()));
        return false;
    }
}

vector<wstring> FileManager::GetMapFileList(const wstring& directory) const
{
    vector<wstring> mapFiles;

    try
    {
        if (filesystem::exists(directory))
        {
            for (const auto& entry : filesystem::directory_iterator(directory))
            {
                if (entry.is_regular_file())
                {
                    wstring extension = entry.path().extension().wstring();
                    if (extension == L".txt" || extension == L".map")
                    {
                        mapFiles.push_back(entry.path().wstring());
                    }
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Error reading map file list: " + string(e.what()));
    }

    // 파일명으로 정렬
    sort(mapFiles.begin(), mapFiles.end());
    return mapFiles;
}

wstring FileManager::GetLatestMapFile(const wstring& directory) const
{
    wstring latestFile;
    filesystem::file_time_type latestTime{};

    try
    {
        if (filesystem::exists(directory))
        {
            for (const auto& entry : filesystem::directory_iterator(directory))
            {
                if (entry.is_regular_file())
                {
                    wstring extension = entry.path().extension().wstring();
                    if (extension == L".txt" || extension == L".map")
                    {
                        auto fileTime = entry.last_write_time();
                        if (fileTime > latestTime)
                        {
                            latestTime = fileTime;
                            latestFile = entry.path().wstring();
                        }
                    }
                }
            }
        }
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Error finding latest map file: " + string(e.what()));
    }

    return latestFile;
}

bool FileManager::CreateBackup(const wstring& fileName) const
{
    try
    {
        if (!FileExists(fileName)) return false;

        wstring backupName = m_BackupDirectory + GetFileNameWithoutExtension(fileName) +
            L"_backup_" + GenerateTimestampedFileName(L"") + L".txt";

        filesystem::copy_file(fileName, backupName);
        LogError("Backup created: " + string(backupName.begin(), backupName.end()));
        return true;
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Failed to create backup: " + string(e.what()));
        return false;
    }
}

FileManager::MapStatistics FileManager::GetMapStatistics(const wstring& fileName) const
{
    MapStatistics stats{};

    if (!FileExists(fileName)) return stats;

    try
    {
        TileMapSaveData mapData;
        // const_cast 제거하고 임시 FileManager 인스턴스 사용
        FileManager* nonConstThis = const_cast<FileManager*>(this);
        if (nonConstThis->LoadTileMap(fileName, mapData))
        {
            stats.totalTiles = 0;
            stats.colliderCount = 0;

            for (int y = 0; y < mapData.gridHeight; ++y)
            {
                for (int x = 0; x < mapData.gridWidth; ++x)
                {
                    // Ground 레이어 통계
                    if (mapData.groundLayer[y][x] > 0)
                    {
                        stats.totalTiles++;
                        stats.tileUsageCount[mapData.groundLayer[y][x]]++;
                    }

                    // UpperGround 레이어 통계
                    if (mapData.upperGroundLayer[y][x] > 0)
                    {
                        stats.totalTiles++;
                        stats.tileUsageCount[mapData.upperGroundLayer[y][x]]++;
                    }

                    // Collider 레이어 통계
                    if (mapData.colliderLayer[y][x])
                    {
                        stats.colliderCount++;
                    }
                }
            }

            stats.uniqueTileCount = stats.tileUsageCount.size();
        }

        // 파일 크기
        stats.fileSize = filesystem::file_size(fileName);
    }
    catch (const exception& e)
    {
        LogError("Error calculating map statistics: " + string(e.what()));
    }

    return stats;
}

bool FileManager::ParseRoomInfo(const string& line, MapData::RoomInfo& roomInfo) const
{
    return false;
}

bool FileManager::ParseTileData(const string& line, MapData::TileData& tileData) const
{
    return false;
}

void FileManager::LogError(const string& message) const
{
    if (m_LogFile.is_open())
    {
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);

        // localtime_s 사용으로 안전하게 변경
        tm timeStruct;
        localtime_s(&timeStruct, &time_t);

        m_LogFile << "[" << put_time(&timeStruct, "%Y-%m-%d %H:%M:%S") << "] "
            << message << endl;
        m_LogFile.flush();
    }

    cout << "[FileManager] " << message << endl;
}

void FileManager::LogWarning(const string& message) const
{
    LogError("[WARNING] " + message);
}

wstring FileManager::GenerateTimestampedFileName(const wstring& baseName) const
{
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);

    // localtime_s 사용으로 안전하게 변경
    tm timeStruct;
    localtime_s(&timeStruct, &time_t);

    wstringstream ss;
    ss << baseName << put_time(&timeStruct, L"%Y%m%d_%H%M%S");
    return ss.str();
}

bool FileManager::DeleteMapFile(const wstring& fileName) const
{
    try
    {
        if (FileExists(fileName))
        {
            filesystem::remove(fileName);
            LogError("File deleted: " + string(fileName.begin(), fileName.end()));
            return true;
        }
        return false;
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Failed to delete file: " + string(e.what()));
        return false;
    }
}

bool FileManager::RestoreBackup(const wstring& backupFileName, const wstring& targetFileName) const
{
    try
    {
        if (!FileExists(backupFileName))
        {
            LogError("Backup file does not exist: " + string(backupFileName.begin(), backupFileName.end()));
            return false;
        }

        filesystem::copy_file(backupFileName, targetFileName, filesystem::copy_options::overwrite_existing);
        LogError("Backup restored: " + string(backupFileName.begin(), backupFileName.end()) +
            " -> " + string(targetFileName.begin(), targetFileName.end()));
        return true;
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Failed to restore backup: " + string(e.what()));
        return false;
    }
}

void FileManager::CleanOldBackups(const wstring& directory, int keepDays) const
{
    try
    {
        if (!filesystem::exists(directory)) return;

        auto cutoffTime = chrono::system_clock::now() - chrono::hours(24 * keepDays);
        int deletedCount = 0;

        for (const auto& entry : filesystem::directory_iterator(directory))
        {
            if (entry.is_regular_file())
            {
                wstring fileName = entry.path().filename().wstring();
                if (fileName.find(L"_backup_") != wstring::npos)
                {
                    auto fileTime = entry.last_write_time();
                    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                        fileTime - filesystem::file_time_type::clock::now() + chrono::system_clock::now());

                    if (sctp < cutoffTime)
                    {
                        filesystem::remove(entry.path());
                        deletedCount++;
                    }
                }
            }
        }

        LogError("Cleaned " + to_string(deletedCount) + " old backup files");
    }
    catch (const filesystem::filesystem_error& e)
    {
        LogError("Error cleaning old backups: " + string(e.what()));
    }
}

bool FileManager::ConvertOldMapFormat(const wstring& oldFileName, const wstring& newFileName)
{
    // 이전 버전 맵 파일을 새로운 타일 ID 형식으로 변환
    if (!FileExists(oldFileName))
    {
        LogError("Old map file does not exist: " + string(oldFileName.begin(), oldFileName.end()));
        return false;
    }

    try
    {
        ifstream oldFile(oldFileName);
        if (!oldFile.is_open()) return false;

        TileMapSaveData convertedData;
        convertedData.roomType = 1;
        convertedData.gridWidth = 50;
        convertedData.gridHeight = 50;
        convertedData.canSpawnMonsters = true;
        memset(convertedData.doors, false, sizeof(convertedData.doors));
        convertedData.doors[0] = true; // 기본적으로 북쪽 문만 열려있음

        string line;
        while (getline(oldFile, line))
        {
            if (line.empty() || line[0] == '#') continue;

            // 기존 형식 파싱 로직
            if (line.find("T=") == 0)
            {
                // 기존: T=layer,x,y,1 (모든 타일이 1로 저장됨)
                // 새로운: T=layer,x,y,tileID (실제 타일 ID 저장)

                stringstream ss(line.substr(2));
                string token;
                vector<int> values;

                while (getline(ss, token, ','))
                {
                    values.push_back(stoi(token));
                }

                if (values.size() == 4)
                {
                    int layer = values[0];
                    int x = values[1];
                    int y = values[2];
                    int oldValue = values[3];

                    // 범위 체크
                    if (x >= 0 && x < convertedData.gridWidth &&
                        y >= 0 && y < convertedData.gridHeight)
                    {
                        if (layer == 0 || layer == 1) // Ground/UpperGround
                        {
                            // 기존 파일에서는 모든 타일이 1로 저장되어 있으므로
                            // 기본 타일 ID로 변환 (실제로는 사용자가 수동으로 수정해야 함)
                            int defaultTileID = (layer == 0) ? 1 : 2; // Ground는 1, UpperGround는 2

                            if (layer == 0)
                                convertedData.groundLayer[y][x] = defaultTileID;
                            else
                                convertedData.upperGroundLayer[y][x] = defaultTileID;
                        }
                        else if (layer == 2) // Collider
                        {
                            convertedData.colliderLayer[y][x] = (oldValue == 1);
                        }
                    }
                }
            }
        }

        oldFile.close();

        // 변환된 데이터를 새 형식으로 저장
        bool result = SaveTileMap(newFileName, convertedData);

        if (result)
        {
            LogError("Map converted successfully: " + string(oldFileName.begin(), oldFileName.end()) +
                " -> " + string(newFileName.begin(), newFileName.end()));
            LogWarning("Converted map uses default tile IDs. Please review and update manually.");
        }

        return result;
    }
    catch (const exception& e)
    {
        LogError("Error converting map format: " + string(e.what()));
        return false;
    }
}

wstring FileManager::GetFileExtension(const wstring& fileName) const
{
    filesystem::path path(fileName);
    return path.extension().wstring();
}

wstring FileManager::GetFileNameWithoutExtension(const wstring& fileName) const
{
    return wstring();
}
