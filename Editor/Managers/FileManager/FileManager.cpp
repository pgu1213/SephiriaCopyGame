#include "../../pch.h"
#include "FileManager.h"
#include "../../Editor/TileMapEditor/TileMapEditor.h"
#include <fstream>
#include <sstream>

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

bool FileManager::SaveMap(const string& filename, TileMapEditor* tileMapEditor)
{
    if (!tileMapEditor)
    {
        printf("Error: TileMapEditor is null\n");
        return false;
    }

    string fullPath = "Maps/" + filename;

    // Maps 폴더가 없으면 생성
    CreateDirectoryA("Maps", NULL);

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, fullPath.c_str(), "w");

    if (err != 0 || !file)
    {
        printf("Error: Failed to create file %s\n", fullPath.c_str());
        return false;
    }

    printf("Saving map to: %s\n", fullPath.c_str());

    bool success = true;

    // XML 헤더 작성
    success &= WriteXMLHeader(file);

    // 방 설정 작성
    RoomConfig config = ConfigManager::GetInstance()->GetCurrentConfig();
    success &= WriteRoomConfig(file, config);

    // 타일 데이터 작성
    success &= WriteTileData(file, tileMapEditor);

    // 프롭 데이터 작성
    success &= WritePropData(file);

    // XML 푸터 작성
    success &= WriteXMLFooter(file);

    fclose(file);

    if (success)
    {
        printf("Map saved successfully: %s\n", fullPath.c_str());

        // 게임 호환 포맷으로도 저장
        string gameFilename = filename;
        size_t pos = gameFilename.find(".xml");
        if (pos != string::npos)
        {
            gameFilename.replace(pos, 4, "_game.txt");
        }
        else
        {
            gameFilename += "_game.txt";
        }

        SaveForGame(gameFilename, tileMapEditor);
    }
    else
    {
        printf("Error: Failed to save map completely\n");
    }

    return success;
}

bool FileManager::LoadMap(const string& filename, TileMapEditor* tileMapEditor)
{
    if (!tileMapEditor)
    {
        printf("Error: TileMapEditor is null\n");
        return false;
    }

    string fullPath = "Maps/" + filename;

    if (!ValidateMapFile(fullPath))
    {
        printf("Error: Invalid or missing map file: %s\n", fullPath.c_str());
        return false;
    }

    RoomConfig config;
    bool success = ParseXMLFile(fullPath, config, tileMapEditor);

    if (success)
    {
        // 로드된 설정을 ConfigManager에 적용
        ConfigManager::GetInstance()->SetCurrentConfig(config);
        printf("Map loaded successfully: %s\n", fullPath.c_str());
    }
    else
    {
        printf("Error: Failed to load map: %s\n", fullPath.c_str());
    }

    return success;
}

void FileManager::AddProp(const PropData& prop)
{
    // 같은 위치에 이미 프롭이 있는지 확인
    for (auto it = m_Props.begin(); it != m_Props.end(); ++it)
    {
        if (it->gridX == prop.gridX && it->gridY == prop.gridY)
        {
            // 기존 프롭 교체
            *it = prop;
            return;
        }
    }

    // 새 프롭 추가
    m_Props.push_back(prop);
}

void FileManager::Removeprop(int gridX, int gridY)
{
    for (auto it = m_Props.begin(); it != m_Props.end(); ++it)
    {
        if (it->gridX == gridX && it->gridY == gridY)
        {
            m_Props.erase(it);
            break;
        }
    }
}

void FileManager::ClearProps()
{
    m_Props.clear();
}

bool FileManager::ValidateMapFile(const string& filename)
{
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename.c_str(), "r");

    if (err != 0 || !file)
    {
        return false;
    }

    // 기본적인 XML 구조 검증
    char line[1024];
    bool hasXmlDeclaration = false;
    bool hasRoomTag = false;

    while (fgets(line, sizeof(line), file))
    {
        string lineStr(line);

        if (lineStr.find("<?xml") != string::npos)
        {
            hasXmlDeclaration = true;
        }

        if (lineStr.find("<room") != string::npos)
        {
            hasRoomTag = true;
        }
    }

    fclose(file);

    return hasXmlDeclaration && hasRoomTag;
}

bool FileManager::SaveForGame(const string& filename, TileMapEditor* tileMapEditor)
{
    string fullPath = "Maps/" + filename;

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, fullPath.c_str(), "w");

    if (err != 0 || !file)
    {
        printf("Error: Failed to create game file %s\n", fullPath.c_str());
        return false;
    }

    RoomConfig config = ConfigManager::GetInstance()->GetCurrentConfig();

    // 게임에서 사용할 간단한 포맷으로 저장
    fprintf(file, "# Room Data for Game\n");
    fprintf(file, "RoomType=%d\n", static_cast<int>(config.roomType));
    fprintf(file, "GridSize=%d,%d\n", config.gridWidth, config.gridHeight);
    fprintf(file, "CanSpawnMonsters=%d\n", config.canSpawnMonsters ? 1 : 0);
    fprintf(file, "Doors=%d,%d,%d,%d\n",
        config.doors[0] ? 1 : 0, config.doors[1] ? 1 : 0,
        config.doors[2] ? 1 : 0, config.doors[3] ? 1 : 0);

    fprintf(file, "\n# Tile Data (Layer,X,Y,Type)\n");

    // 타일 데이터를 간단한 형태로 저장
    for (int layer = 0; layer < 4; ++layer)
    {
        for (int x = 0; x < config.gridWidth; ++x)
        {
            for (int y = 0; y < config.gridHeight; ++y)
            {
                TileData* tile = tileMapEditor->GetTileAt(x, y); // 해당 레이어의 타일 가져오기
                if (tile && tile->type != TileType::EMPTY)
                {
                    fprintf(file, "T=%d,%d,%d,%d\n",
                        layer, x, y, static_cast<int>(tile->type));
                }
            }
        }
    }

    fprintf(file, "\n# Prop Data (X,Y,Type,Sprite)\n");
    for (const PropData& prop : m_Props)
    {
        fprintf(file, "P=%d,%d,%s,%s\n",
            prop.gridX, prop.gridY,
            prop.propType.c_str(), prop.spriteName.c_str());
    }

    fclose(file);

    printf("Game format saved: %s\n", fullPath.c_str());
    return true;
}

bool FileManager::WriteXMLHeader(FILE* file)
{
    fprintf(file, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    return true;
}

bool FileManager::WriteRoomConfig(FILE* file, const RoomConfig& config)
{
    fprintf(file, "  <RoomConfig>\n");
    fprintf(file, "    <Name>%s</Name>\n", EscapeXML(config.roomName).c_str());
    fprintf(file, "    <Type>%d</Type>\n", static_cast<int>(config.roomType));
    fprintf(file, "    <GridWidth>%d</GridWidth>\n", config.gridWidth);
    fprintf(file, "    <GridHeight>%d</GridHeight>\n", config.gridHeight);
    fprintf(file, "    <CanSpawnMonsters>%s</CanSpawnMonsters>\n", config.canSpawnMonsters ? "true" : "false");
    fprintf(file, "    <Doors>\n");
    fprintf(file, "      <North>%s</North>\n", config.doors[0] ? "true" : "false");
    fprintf(file, "      <South>%s</South>\n", config.doors[1] ? "true" : "false");
    fprintf(file, "      <East>%s</East>\n", config.doors[2] ? "true" : "false");
    fprintf(file, "      <West>%s</West>\n", config.doors[3] ? "true" : "false");
    fprintf(file, "    </Doors>\n");
    fprintf(file, "  </RoomConfig>\n");
    return true;
}

bool FileManager::WriteTileData(FILE* file, TileMapEditor* tileMapEditor)
{
    RoomConfig config = ConfigManager::GetInstance()->GetCurrentConfig();

    // mainLayer 시작
    fprintf(file, "  <mainLayer>\n");

    // 각 레이어별로 데이터 작성 (ground, upperGround, wall, cliff)
    const char* layerNames[] = { "ground", "upperGround", "wall", "cliff" };

    for (int layer = 0; layer < 4; ++layer)
    {
        fprintf(file, "    <%s>", layerNames[layer]);

        // 그리드 데이터를 한 줄씩 작성
        for (int y = 0; y < config.gridHeight; ++y)
        {
            for (int x = 0; x < config.gridWidth; ++x)
            {
                // 여기서 실제 타일 데이터를 가져와야 함 (TileMapEditor에서 구현 필요)
                TileData* tile = tileMapEditor->GetTileAtLayer(layer, x, y);

                char tileChar = 'X'; // 기본값
                if (tile && tile->type != TileType::EMPTY)
                {
                    // 타일 타입에 따른 문자 설정
                    switch (tile->type)
                    {
                    case TileType::FLOOR: tileChar = '0'; break;
                    case TileType::WALL: tileChar = '1'; break;
                    case TileType::CLIFF: tileChar = '2'; break;
                    case TileType::MONSTER_SPAWN: tileChar = '3'; break;
                    case TileType::MONSTER_MOVE: tileChar = '4'; break;
                    case TileType::DOOR_NORTH:
                    case TileType::DOOR_SOUTH:
                    case TileType::DOOR_EAST:
                    case TileType::DOOR_WEST: tileChar = '5'; break;
                    default: tileChar = 'X'; break;
                    }
                }

                fprintf(file, "%c ", tileChar);
            }
            fprintf(file, "\n");
        }

        fprintf(file, "</%s>\n", layerNames[layer]);
    }

    // props 섹션
    fprintf(file, "    <props>\n");

    // 프롭 데이터 작성
    for (const PropData& prop : m_Props)
    {
        fprintf(file, "      <%s position=\"%.4f,%.4f\" localScale=\"1,1,1\" />\n",
            prop.propType.c_str(),
            static_cast<float>(prop.gridX) + 0.5f, // 그리드 중앙 위치
            static_cast<float>(prop.gridY) + 0.5f);
    }

    fprintf(file, "    </props>\n");
    fprintf(file, "  </mainLayer>\n");

    return true;
}

bool FileManager::WritePropData(FILE* file)
{
    // 패시지 레이어들 작성 (passageLayer0~passageLayer5)
    for (int passageLayer = 0; passageLayer < 6; ++passageLayer)
    {
        fprintf(file, "  <passageLayer%d position=\"0,0\" size=\"0,0\">\n", passageLayer);

        // 각 패시지 레이어도 ground, upperGround, wall, cliff 구조
        const char* layerNames[] = { "ground", "upperGround", "wall", "cliff" };

        for (int i = 0; i < 4; ++i)
        {
            fprintf(file, "    <%s>", layerNames[i]);

            // 기본적으로 빈 레이어 (모든 X)
            RoomConfig config = ConfigManager::GetInstance()->GetCurrentConfig();
            for (int y = 0; y < config.gridHeight; ++y)
            {
                for (int x = 0; x < config.gridWidth; ++x)
                {
                    fprintf(file, "X ");
                }
                fprintf(file, "\n");
            }

            fprintf(file, "</%s>\n", layerNames[i]);
        }

        fprintf(file, "    <props />\n");
        fprintf(file, "  </passageLayer%d>\n", passageLayer);
    }

    return true;
}

bool FileManager::WriteXMLFooter(FILE* file)
{
    fprintf(file, "</room>\n");
    return true;
}

bool FileManager::ParseXMLFile(const string& filename, RoomConfig& config, TileMapEditor* tileMapEditor)
{
    // 간단한 XML 파싱 구현 (실제로는 더 정교한 XML 파서 사용 권장)
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename.c_str(), "r");

    if (err != 0 || !file)
    {
        printf("Error: Cannot open file %s for reading\n", filename.c_str());
        return false;
    }

    char line[1024];
    bool inRoom = false;
    bool inMainLayer = false;
    string currentLayer = "";
    int currentY = 0;
    LayerType currentLayerType = LayerType::BACKGROUND;

    while (fgets(line, sizeof(line), file))
    {
        string lineStr(line);

        // room 태그 파싱
        if (lineStr.find("<room") != string::npos)
        {
            inRoom = true;
            ParseRoomAttributes(lineStr, config);
        }

        // mainLayer 시작
        else if (lineStr.find("<mainLayer>") != string::npos)
        {
            inMainLayer = true;
        }

        // 레이어 시작 태그들
        else if (lineStr.find("<ground>") != string::npos)
        {
            currentLayer = "ground";
            currentLayerType = LayerType::BACKGROUND;
            currentY = 0;
        }
        else if (lineStr.find("<upperGround>") != string::npos)
        {
            currentLayer = "upperGround";
            currentLayerType = LayerType::DECORATION;
            currentY = 0;
        }
        else if (lineStr.find("<wall>") != string::npos)
        {
            currentLayer = "wall";
            currentLayerType = LayerType::COLLISION;
            currentY = 0;
        }
        else if (lineStr.find("<cliff>") != string::npos)
        {
            currentLayer = "cliff";
            currentLayerType = LayerType::INTERACTION;
            currentY = 0;
        }

        // 레이어 종료 태그들
        else if (lineStr.find("</ground>") != string::npos ||
            lineStr.find("</upperGround>") != string::npos ||
            lineStr.find("</wall>") != string::npos ||
            lineStr.find("</cliff>") != string::npos)
        {
            currentLayer = "";
        }

        // 타일 데이터 파싱
        else if (!currentLayer.empty() && inMainLayer)
        {
            ParseTileRow(lineStr, tileMapEditor, currentLayerType, currentY);
            currentY++;
        }

        // props 파싱
        else if (lineStr.find("<") != string::npos && lineStr.find("position=") != string::npos)
        {
            ParsePropData(lineStr);
        }
    }

    fclose(file);
    return true;
}

void FileManager::ParseRoomAttributes(const string& roomLine, RoomConfig& config)
{
    // spawnMonster 파싱
    size_t pos = roomLine.find("spawnMonster=\"");
    if (pos != string::npos)
    {
        pos += 14; // "spawnMonster=\"" 길이
        config.canSpawnMonsters = (roomLine.substr(pos, 4) == "true");
    }

    // type 파싱
    pos = roomLine.find("type=\"");
    if (pos != string::npos)
    {
        pos += 6; // "type=\"" 길이
        int type = stoi(roomLine.substr(pos, 1));
        config.roomType = static_cast<RoomType>(type);
    }

    // passages 파싱
    pos = roomLine.find("passages=\"");
    if (pos != string::npos)
    {
        pos += 10; // "passages=\"" 길이
        string passages = roomLine.substr(pos, 8);

        config.doors[static_cast<int>(DoorDirection::NORTH)] = (passages[0] == '1');
        config.doors[static_cast<int>(DoorDirection::SOUTH)] = (passages[1] == '1');
        config.doors[static_cast<int>(DoorDirection::EAST)] = (passages[2] == '1');
        config.doors[static_cast<int>(DoorDirection::WEST)] = (passages[3] == '1');
    }
}

void FileManager::ParseTileRow(const string& row, TileMapEditor* tileMapEditor, LayerType layer, int y)
{
    if (!tileMapEditor) return;

    stringstream ss(row);
    string token;
    int x = 0;

    while (ss >> token && x < 100) // 최대 그리드 크기 제한
    {
        if (token.length() == 1)
        {
            TileType tileType = TileType::EMPTY;

            // 문자를 타일 타입으로 변환
            switch (token[0])
            {
            case '0': tileType = TileType::FLOOR; break;
            case '1': tileType = TileType::WALL; break;
            case '2': tileType = TileType::CLIFF; break;
            case '3': tileType = TileType::MONSTER_SPAWN; break;
            case '4': tileType = TileType::MONSTER_MOVE; break;
            case '5': tileType = TileType::DOOR_NORTH; break; // 문 타입은 위치에 따라 결정
            default: tileType = TileType::EMPTY; break;
            }

            if (tileType != TileType::EMPTY)
            {
                // 직접 타일맵에 설정 (레이어별로)
                TileData* tile = tileMapEditor->GetTileAtLayer(static_cast<int>(layer), x, y);
                if (tile)
                {
                    tile->type = tileType;
                    tile->spriteIndex = 0;

                    // 타일 속성 설정
                    switch (tileType)
                    {
                    case TileType::WALL:
                    case TileType::CLIFF:
                        tile->isWalkable = false;
                        tile->canSpawnMonster = false;
                        break;
                    case TileType::MONSTER_SPAWN:
                        tile->isWalkable = true;
                        tile->canSpawnMonster = true;
                        break;
                    default:
                        tile->isWalkable = true;
                        tile->canSpawnMonster = false;
                        break;
                    }
                }
            }
        }
        x++;
    }
}

void FileManager::ParsePropData(const string& propLine)
{
    // 프롭 이름 추출
    size_t startPos = propLine.find('<');
    size_t endPos = propLine.find(' ', startPos);
    if (startPos == string::npos || endPos == string::npos) return;

    string propType = propLine.substr(startPos + 1, endPos - startPos - 1);

    // position 파싱
    size_t posStart = propLine.find("position=\"");
    if (posStart == string::npos) return;

    posStart += 10; // "position=\"" 길이
    size_t posEnd = propLine.find("\"", posStart);
    if (posEnd == string::npos) return;

    string posStr = propLine.substr(posStart, posEnd - posStart);
    size_t commaPos = posStr.find(',');
    if (commaPos == string::npos) return;

    float x = stof(posStr.substr(0, commaPos));
    float y = stof(posStr.substr(commaPos + 1));

    // 그리드 좌표로 변환 (0.5를 빼서 정수 좌표로)
    int gridX = static_cast<int>(x);
    int gridY = static_cast<int>(y);

    // 프롭 데이터 추가
    PropData prop(gridX, gridY, propType, "");
    AddProp(prop);
}

string FileManager::EscapeXML(const string& text)
{
    string result = text;

    // XML 특수 문자 이스케이프
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != string::npos)
    {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }

    pos = 0;
    while ((pos = result.find("<", pos)) != string::npos)
    {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }

    pos = 0;
    while ((pos = result.find(">", pos)) != string::npos)
    {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }

    pos = 0;
    while ((pos = result.find("\"", pos)) != string::npos)
    {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }

    return result;
}

string FileManager::GetTileTypeName(int tileType)
{
    const char* typeNames[] = {
        "EMPTY", "FLOOR", "WALL", "CLIFF", "MONSTER_SPAWN",
        "MONSTER_MOVE", "DOOR_NORTH", "DOOR_SOUTH", "DOOR_EAST", "DOOR_WEST"
    };

    if (tileType >= 0 && tileType < sizeof(typeNames) / sizeof(typeNames[0]))
    {
        return string(typeNames[tileType]);
    }

    return "UNKNOWN";
}

int FileManager::GetTileTypeFromName(const string& name)
{
    map<string, int> nameToType = {
        {"EMPTY", 0}, {"FLOOR", 1}, {"WALL", 2}, {"CLIFF", 3},
        {"MONSTER_SPAWN", 4}, {"MONSTER_MOVE", 5},
        {"DOOR_NORTH", 6}, {"DOOR_SOUTH", 7}, {"DOOR_EAST", 8}, {"DOOR_WEST", 9}
    };

    auto it = nameToType.find(name);
    return (it != nameToType.end()) ? it->second : 0;
}
