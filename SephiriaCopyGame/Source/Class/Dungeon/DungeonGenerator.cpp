#include "pch.h"
#include "DungeonGenerator.h"
#include <Engine/Managers/ResourceManager/ResourceManager.h>
#include <Engine/Managers/SceneManager/SceneManager.h>
#include <Engine/Object/Object/Object.h>
#include <Engine/Object/Component/SpriteRenderer/SpriteRenderer.h>
#include <Engine/Object/Component/Collider/BoxCollider.h>
#include <fstream>
#include <sstream>
#include <iostream>

DungeonGenerator::DungeonGenerator(Object* owner)
    : Component(owner)
    , m_tileSize(64.0f)
    , m_isLoaded(false)
    , m_isGenerated(false)
    , m_showDebugInfo(true)
	, m_showColliders(true)
{
}

DungeonGenerator::DungeonGenerator(const DungeonGenerator& other)
    : Component(other)
    , m_mapData(other.m_mapData)
    , m_tileObjects(other.m_tileObjects)
    , m_tileSize(other.m_tileSize)
    , m_isLoaded(other.m_isLoaded)
    , m_isGenerated(other.m_isGenerated)
    , m_showDebugInfo(other.m_showDebugInfo)
	, m_showColliders(other.m_showColliders)
{
}

Component* DungeonGenerator::CloneImpl() const
{
	return new DungeonGenerator(*this);
}

void DungeonGenerator::Init()
{
    std::wcout << L"DungeonGenerator 초기화됨" << std::endl;
}

void DungeonGenerator::Update(float deltaTime)
{
    // 필요시 타일 오브젝트들의 업데이트 처리
}

void DungeonGenerator::Render(HDC hdc)
{
    Component::Render(hdc);

    if (m_showDebugInfo)
    {
        // 디버그 정보 출력
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        std::wstring debugText = L"Dungeon: " + std::to_wstring(m_tileObjects.size()) + L" tiles";
        TextOut(hdc, 10, 10, debugText.c_str(), debugText.length());

        std::wstring roomInfo = L"Room: " + std::to_wstring(m_mapData.roomWidth) +
            L"x" + std::to_wstring(m_mapData.roomHeight);
        TextOut(hdc, 10, 30, roomInfo.c_str(), roomInfo.length());
    }
}

void DungeonGenerator::OnDestroy()
{
    ClearDungeon();
}

void DungeonGenerator::LoadMapFromFile(const std::wstring& filePath)
{
    std::wifstream file(filePath);
    if (!file.is_open())
    {
        std::wcout << L"맵 파일을 열 수 없습니다: " << filePath << std::endl;
        return;
    }

    std::vector<std::wstring> lines;
    std::wstring line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    file.close();

    if (ParseMapFile(lines))
    {
        m_isLoaded = true;
        std::wcout << L"맵 파일 로드 성공: " << filePath << std::endl;
        std::wcout << L"방 크기: " << m_mapData.roomWidth << L"x" << m_mapData.roomHeight << std::endl;
        std::wcout << L"그라운드 타일: " << m_mapData.groundTiles.size() << L"개" << std::endl;
        std::wcout << L"장식 타일: " << m_mapData.decorationTiles.size() << L"개" << std::endl;
    }
    else
    {
        std::wcout << L"맵 파일 파싱 실패: " << filePath << std::endl;
    }
}

void DungeonGenerator::GenerateDungeon()
{
    if (!m_isLoaded)
    {
        std::wcout << L"맵 데이터가 로드되지 않았습니다." << std::endl;
        return;
    }

    // 기존 던전 제거
    ClearDungeon();

    std::wcout << L"던전 생성 시작..." << std::endl;

    // 1. 그라운드 타일 생성
    CreateGroundTiles();

    // 2. 장식 타일 생성
    CreateDecorationTiles();

    // 3. 콜라이더 적용
    ApplyColliders();

    m_isGenerated = true;
    std::wcout << L"던전 생성 완료: " << m_tileObjects.size() << L"개 타일" << std::endl;
}

void DungeonGenerator::ClearDungeon()
{
    for (auto& tileObj : m_tileObjects)
    {
        if (tileObj.tileObject)
        {
            delete tileObj.tileObject;
        }
    }
    m_tileObjects.clear();
    m_isGenerated = false;
}

Object* DungeonGenerator::CreateTileObject(const std::wstring& tileName, int gridX, int gridY, bool hasCollider)
{
	Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
    if (!currentScene)
    {
        std::wcout << L"현재 씬이 없습니다. 타일 오브젝트를 생성할 수 없습니다." << std::endl;
        return nullptr;
	}

    // string 변환
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, tileName.c_str(), (int)tileName.size(), NULL, 0, NULL, NULL);
    string tileNamestring(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, tileName.c_str(), (int)tileName.size(), &tileNamestring[0], sizeNeeded, NULL, NULL);

    // 그리드와 타일 기반으로 이름 정하기
    string objectName;
    if (tileNamestring.empty())
    {
        objectName = "Collider_x" + std::to_string(gridX) + "_y" + std::to_string(gridY);
    }
    else
    {
        objectName = tileNamestring + "_x" + std::to_string(gridX) + "_y" + std::to_string(gridY);
    }

    //이름 설정
    Object* tileObject = currentScene->CreateGameObject(objectName);

    // 위치 설정
    SetTilePosition(tileObject, gridX, gridY);

    // SpriteRenderer 컴포넌트 추가
    SpriteRenderer* spriteRenderer = tileObject->AddComponent<SpriteRenderer>();
    spriteRenderer->SetSprite(tileName);
	spriteRenderer->SetSize(64.0f, 64.0f);

    // 실제 리소스 로드 로직은 ResourceManager 구현에 따라 달라질 수 있음

    // 콜라이더가 필요한 경우 추가
    BoxCollider* boxCollider = nullptr;
    if (hasCollider)
    {
        boxCollider = tileObject->AddComponent<BoxCollider>();
		Vector2 colliderSize(m_tileSize, m_tileSize);
        boxCollider->SetSize(colliderSize);
    }

    // TileObject 구조체 생성 및 추가
    TileObject tileObjInfo;
    tileObjInfo.tileObject = tileObject;
    tileObjInfo.spriteRenderer = spriteRenderer;
    tileObjInfo.boxCollider = boxCollider;
    tileObjInfo.gridX = gridX;
    tileObjInfo.gridY = gridY;
    tileObjInfo.hasCollider = hasCollider;

    m_tileObjects.push_back(tileObjInfo);

    return tileObject;
}

void DungeonGenerator::SetTilePosition(Object* tileObject, int gridX, int gridY)
{
    if (!tileObject) return;

    float worldX = gridX * m_tileSize;
    float worldY = gridY * m_tileSize;

    tileObject->SetPosition(worldX, worldY);
}

bool DungeonGenerator::ParseMapFile(const std::vector<std::wstring>& lines)
{
    try
    {
        int lineIndex = 0;
        std::wstring currentSection = L"";

        // 초기화
        m_mapData.cliffLayer.clear();
        m_mapData.colliderLayer.clear();
        m_mapData.groundTiles.clear();
        m_mapData.decorationTiles.clear();

        while (lineIndex < lines.size())
        {
            std::wstring line = Trim(lines[lineIndex]);

            // 빈 줄이나 주석 건너뛰기
            if (line.empty() || line[0] == L'#')
            {
                lineIndex++;
                continue;
            }

            // 헤더 정보 파싱
            if (line.find(L'=') != std::wstring::npos)
            {
                size_t pos = line.find(L'=');
                std::wstring key = Trim(line.substr(0, pos));
                std::wstring value = Trim(line.substr(pos + 1));

                if (key == L"GridSize")
                    m_mapData.gridSize = std::stoi(value);
                else if (key == L"RoomWidth")
                    m_mapData.roomWidth = std::stoi(value);
                else if (key == L"RoomHeight")
                    m_mapData.roomHeight = std::stoi(value);
                else if (key == L"RoomType")
                    m_mapData.roomType = std::stoi(value);
                else if (key == L"RoomTypeName")
                    m_mapData.roomTypeName = value;

                lineIndex++;
                continue;
            }

            // 섹션 헤더 체크
            if (line[0] == L'[' && line.back() == L']')
            {
                currentSection = line.substr(1, line.length() - 2);
                lineIndex++;
                continue;
            }

            // 섹션 내용 파싱
            if (currentSection == L"GroundLayer")
            {
                ParseGroundLayer(lines, lineIndex);
            }
            else if (currentSection == L"DecorationLayer")
            {
                ParseDecorationLayer(lines, lineIndex);
            }
            else if (currentSection == L"CliffLayer")
            {
                ParseCliffLayer(lines, lineIndex);
                break; // CliffLayer 파싱 후 바로 ColliderLayer로 넘어감
            }
            else
            {
                lineIndex++;
            }
        }

        // ColliderLayer 파싱
        for (int i = lineIndex; i < lines.size(); i++)
        {
            if (Trim(lines[i]) == L"[ColliderLayer]")
            {
                lineIndex = i + 1;
                ParseColliderLayer(lines, lineIndex);
                break;
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "맵 파일 파싱 중 오류: " << e.what() << std::endl;
        return false;
    }
}

void DungeonGenerator::ParseGroundLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    while (lineIndex < lines.size())
    {
        std::wstring line = Trim(lines[lineIndex]);

        if (line.empty() || line[0] == L'[')
            break;

        if (StartsWith(line, L"TileCount="))
        {
            lineIndex++;
            continue;
        }

        // 타일 데이터 파싱: "TileName X Y"
        auto parts = SplitString(line, L' ');
        if (parts.size() >= 3)
        {
            MapData::TileInfo tile;
            tile.tileName = parts[0];
            tile.x = std::stoi(parts[1]);
            tile.y = std::stoi(parts[2]);
            m_mapData.groundTiles.push_back(tile);
        }

        lineIndex++;
    }
}

void DungeonGenerator::ParseDecorationLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    while (lineIndex < lines.size())
    {
        std::wstring line = Trim(lines[lineIndex]);

        if (line.empty() || line[0] == L'[')
            break;

        if (StartsWith(line, L"TileCount="))
        {
            lineIndex++;
            continue;
        }

        // 타일 데이터 파싱: "TileName X Y"
        auto parts = SplitString(line, L' ');
        if (parts.size() >= 3)
        {
            MapData::TileInfo tile;
            tile.tileName = parts[0];
            tile.x = std::stoi(parts[1]);
            tile.y = std::stoi(parts[2]);
            m_mapData.decorationTiles.push_back(tile);
        }

        lineIndex++;
    }
}

void DungeonGenerator::ParseCliffLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    m_mapData.cliffLayer.resize(m_mapData.roomHeight);

    for (int y = 0; y < m_mapData.roomHeight && lineIndex < lines.size(); y++)
    {
        std::wstring line = Trim(lines[lineIndex]);
        if (line.length() >= m_mapData.roomWidth)
        {
            m_mapData.cliffLayer[y].resize(m_mapData.roomWidth);
            for (int x = 0; x < m_mapData.roomWidth; x++)
            {
                m_mapData.cliffLayer[y][x] = (line[x] == L'1');
            }
        }
        lineIndex++;
    }
}

void DungeonGenerator::ParseColliderLayer(const std::vector<std::wstring>& lines, int& lineIndex)
{
    m_mapData.colliderLayer.resize(m_mapData.roomHeight);

    for (int y = 0; y < m_mapData.roomHeight && lineIndex < lines.size(); y++)
    {
        std::wstring line = Trim(lines[lineIndex]);
        if (line.length() >= m_mapData.roomWidth)
        {
            m_mapData.colliderLayer[y].resize(m_mapData.roomWidth);
            for (int x = 0; x < m_mapData.roomWidth; x++)
            {
                m_mapData.colliderLayer[y][x] = (line[x] == L'1');
            }
        }
        lineIndex++;
    }
}

void DungeonGenerator::CreateGroundTiles()
{
    for (const auto& tile : m_mapData.groundTiles)
    {
        CreateTileObject(tile.tileName, tile.x, tile.y, false);
    }

    std::wcout << L"그라운드 타일 " << m_mapData.groundTiles.size() << L"개 생성" << std::endl;
}

void DungeonGenerator::CreateDecorationTiles()
{
    for (const auto& tile : m_mapData.decorationTiles)
    {
        CreateTileObject(tile.tileName, tile.x, tile.y, false);
    }

    std::wcout << L"장식 타일 " << m_mapData.decorationTiles.size() << L"개 생성" << std::endl;
}

void DungeonGenerator::ApplyColliders()
{
    int colliderCount = 0;

    // 콜라이더 레이어 데이터를 확인하여 기존 타일에 콜라이더 추가
    for (int y = 0; y < m_mapData.roomHeight; y++)
    {
        for (int x = 0; x < m_mapData.roomWidth; x++)
        {
            if (y < m_mapData.colliderLayer.size() &&
                x < m_mapData.colliderLayer[y].size() &&
                m_mapData.colliderLayer[y][x])
            {
                // 해당 위치에 이미 타일이 있는지 확인
                bool foundTile = false;
                for (auto& tileObj : m_tileObjects)
                {
                    if (tileObj.gridX == x && tileObj.gridY == y)
                    {
                        // 기존 타일에 콜라이더 추가
                        if (!tileObj.hasCollider && !tileObj.boxCollider)
                        {
                            BoxCollider* boxCollider = tileObj.tileObject->AddComponent<BoxCollider>();
							Vector2 colliderSize(m_tileSize, m_tileSize);
                            boxCollider->SetSize(colliderSize);
                            //boxCollider->SetLayer(CollisionLayer::Wall);

                            tileObj.boxCollider = boxCollider;
                            tileObj.hasCollider = true;
                            colliderCount++;
                        }
                        foundTile = true;
                        break;
                    }
                }

                // 타일이 없는 위치에는 보이지 않는 콜라이더 타일 생성
                if (!foundTile)
                {
                    Object* colliderTile = CreateTileObject(L"", x, y, true);
                    colliderCount++;
                }
            }
        }
    }

    std::wcout << L"콜라이더 " << colliderCount << L"개 적용" << std::endl;
}

// 유틸리티 함수들
std::vector<std::wstring> DungeonGenerator::SplitString(const std::wstring& str, wchar_t delimiter)
{
    std::vector<std::wstring> result;
    std::wstringstream ss(str);
    std::wstring item;

    while (std::getline(ss, item, delimiter))
    {
        if (!item.empty())
            result.push_back(item);
    }

    return result;
}

std::wstring DungeonGenerator::Trim(const std::wstring& str)
{
    size_t start = str.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";

    size_t end = str.find_last_not_of(L" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool DungeonGenerator::StartsWith(const std::wstring& str, const std::wstring& prefix)
{
    return str.length() >= prefix.length() &&
        str.substr(0, prefix.length()) == prefix;
}